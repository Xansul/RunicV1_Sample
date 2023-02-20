// Tyler Call 2022


#include "RCharacterAbilityBase.h"
#include "../Tools/Interfaces/RInteractInterface.h"
#include "RunicV1/AbilitySystem/Attributes/RAttributeSetBasePlayer.h"
#include "../RAbilitySystemComponent.h"

ARCharacterAbilityBase::ARCharacterAbilityBase()
{
	//ASC setup
	AbilitySystemComponent = CreateDefaultSubobject<URAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);

	//Initialize shield variables
	IsBlocking = false;
	FlowReturnBase = 10.0f;

	//Initialize combo variables
	IsAttacking = false;
	AttackSaved = false;
	AttackCount = 0;
	ActiveMontageArray = EmptyMontageArray;
}

UAbilitySystemComponent* ARCharacterAbilityBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

//Initialize ASC
void ARCharacterAbilityBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

//Check if character is facing damage source - return effect with damage and returned flow in custom Effect
//************ TODO: transfer attribute change logic to RAttributeSetPlayerBase, allow for full effect spec to be passed in and applied

void ARCharacterAbilityBase::ShieldCheck(AActor* DamageSource, float ShieldTimer, float BaseDamage)
{
	//dynamic effect that just has damage
	UGameplayEffect* NewEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName("GE_CustomDamage"));
	NewEffect->Modifiers.SetNum(1);

	NewEffect->Modifiers[0].ModifierMagnitude = FScalableFloat(BaseDamage);
	NewEffect->Modifiers[0].ModifierOp = EGameplayModOp::Additive;
	NewEffect->Modifiers[0].Attribute = URAttributeSetBasePlayer::GetDamageAttribute();
	NewEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

	//return all damage and no flow if character is not blocking, otherwise proceed with calculation
	if (IsBlocking == true)
	{
		//return all damage and no flow if ShieldTimer is at 0, otherwise proceed
		if (!ShieldTimer == 0.0f)
		{
			//get actor rotations
			FRotator DamageRotation = DamageSource->GetActorRotation();
			FRotator SelfRotation = this->GetActorRotation();

			float RotatorResult = SelfRotation.Yaw - DamageRotation.Yaw;

			if (RotatorResult > 100.0f || RotatorResult < -100.0f)
			{
				//dynamic effect that includes flow
				UGameplayEffect* NewEffectFlow = NewObject<UGameplayEffect>(GetTransientPackage(), FName("GE_CustomDamageFlow"));
				NewEffectFlow->Modifiers.SetNum(2);

				//clamp returned flow
				float ReturnedFlow = FlowReturnBase - (ShieldTimer * 2.0f);
				ReturnedFlow = FMath::Clamp(ReturnedFlow, 0.0f, FlowReturnBase);

				//adjust damage based on time shield has been held
				float NewDamage;

				//return 0 damage if shield is held for short time
				if (ShieldTimer < 5.0f)
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Perfect Block!"));

					NewDamage = 0.0f;

					//update new effect
					//damage modifier
					NewEffectFlow->Modifiers[0].ModifierMagnitude = FScalableFloat(NewDamage);
					NewEffectFlow->Modifiers[0].ModifierOp = EGameplayModOp::Additive;
					NewEffectFlow->Modifiers[0].Attribute = URAttributeSetBasePlayer::GetDamageAttribute();

					//Flow modifier
					NewEffectFlow->Modifiers[1].ModifierMagnitude = FScalableFloat(ReturnedFlow);
					NewEffectFlow->Modifiers[1].ModifierOp = EGameplayModOp::Additive;
					NewEffectFlow->Modifiers[1].Attribute = URAttributeSetBasePlayer::GetFlowAttribute();
					NewEffectFlow->DurationPolicy = EGameplayEffectDurationType::Instant;

					AbilitySystemComponent->ApplyGameplayEffectToSelf(NewEffectFlow, 0.0f, AbilitySystemComponent->MakeEffectContext());

				}
				//return 1/4 damage if shield is held longer
				else
				{
					NewDamage = BaseDamage / 4.0f;

					//update new effect
					//damage modifier
					NewEffectFlow->Modifiers[0].ModifierMagnitude = FScalableFloat(NewDamage);
					NewEffectFlow->Modifiers[0].ModifierOp = EGameplayModOp::Additive;
					NewEffectFlow->Modifiers[0].Attribute = URAttributeSetBasePlayer::GetDamageAttribute();

					//Flow modifier
					NewEffectFlow->Modifiers[1].ModifierMagnitude = FScalableFloat(ReturnedFlow);
					NewEffectFlow->Modifiers[1].ModifierOp = EGameplayModOp::Additive;
					NewEffectFlow->Modifiers[1].Attribute = URAttributeSetBasePlayer::GetFlowAttribute();
					NewEffectFlow->DurationPolicy = EGameplayEffectDurationType::Instant;

					AbilitySystemComponent->ApplyGameplayEffectToSelf(NewEffectFlow, 0.0f, AbilitySystemComponent->MakeEffectContext());
				}

			}
			else
			{
				AbilitySystemComponent->ApplyGameplayEffectToSelf(NewEffect, 0.0f, AbilitySystemComponent->MakeEffectContext());
			}
		}
		else
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(NewEffect, 0.0f, AbilitySystemComponent->MakeEffectContext());
		}
	}
	else
	{
		AbilitySystemComponent->ApplyGameplayEffectToSelf(NewEffect, 0.0f, AbilitySystemComponent->MakeEffectContext());
	}
}

//death function - simulate physics and add Dead tag
void ARCharacterAbilityBase::Death(ARCharacterBaseNew* Character)
{
	//get all skeletal mesh components
	TArray<USkeletalMeshComponent*> SkeletalMeshComps;
	Character->GetComponents<USkeletalMeshComponent>(SkeletalMeshComps);

	if (!SkeletalMeshComps.IsEmpty())
	{
		for (auto SkeletalMeshes : SkeletalMeshComps)
		{
			SkeletalMeshes->SetSimulatePhysics(true);
		}
	}

	AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
}

//Combo system (based on Paragon character BP)
//Play montage funtion - plays montage from array based on current attack count
void ARCharacterAbilityBase::ComboPlayMontage()
{
	//loop through elements in montage array
	for (int i = 0; i < ActiveMontageArray.Num(); i++)
	{
		//Execute montage if AttackCount matches the element number
		if (AttackCount == i)
		{
			//Play montage and update attack count
			//Reset AttackCount if on last element
			if (i == (ActiveMontageArray.Num() - 1))
			{
				AttackCount = 0;

				this->PlayAnimMontage(ActiveMontageArray[i]);

				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Montage end"));

				return;
			}
			else
			{
				AttackCount += 1;

				this->PlayAnimMontage(ActiveMontageArray[i]);

				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Montage"));

				return;
			}
		}
	}
}

//Basic combo attack logic (activated by player input)
void ARCharacterAbilityBase::ComboAttack()
{
	//Save attack if already attacking
	if (IsAttacking == true)
	{
		AttackSaved = true;
	}
	//Play montage if not already attacking
	else
	{
		IsAttacking = true;

		ComboPlayMontage();
	}
}

//Combo attack logic when combo has already started (called by AnimNotify in animation BP)
void ARCharacterAbilityBase::ComboAttackSaved()
{
	//Execute attack if attack has been saved (which is changed by ComboAttack() function)
	if (AttackSaved == true)
	{
		AttackSaved = false;

		ComboPlayMontage();
	}
}

//Reset combo (called by AnimNotify in animation BP)
void ARCharacterAbilityBase::ComboReset()
{
	//Clear variables
	AttackCount = 0;
	AttackSaved = false;
	IsAttacking = false;
}

//Interact function
void ARCharacterAbilityBase::Interact(TArray<AActor*> ActorArray)
{
	if (!ActorArray.IsEmpty())
	{
		//Search given array for actors that implement RInteractInterface
		TArray<AActor*> SearchedActorArray;
		for (AActor* Actor : ActorArray)
		{
			if (Actor->GetClass()->ImplementsInterface(URInteractInterface::StaticClass()))
			{
				SearchedActorArray.Add(Actor);
			}
		}

		if (!SearchedActorArray.IsEmpty())
		{
			//Get closest object in searched array
			AActor* ClosestActor = nullptr;
			float Distance = 1000;

			for (AActor* Actor : SearchedActorArray)
			{
				float TempDistance;
				TempDistance = FVector::Dist(this->GetActorLocation(), Actor->GetActorLocation());

				//debug
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Distance to interact object"), TempDistance));

				if (TempDistance < Distance)
				{
					ClosestActor = Actor;
					Distance = TempDistance;
				}
			}

			//Call interface function on actor
			IRInteractInterface::Execute_Interact(ClosestActor, ClosestActor, this);
		}
	}
}