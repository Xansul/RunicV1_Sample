// Tyler Call 2022


#include "RAbilityTeleport.h"
#include "RunicV1/AbilitySystem/GameplayEffects/RTeleportCooldownEffect.h"
#include "RunicV1/Characters/RCharacterAbilityBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

URAbilityTeleport::URAbilityTeleport()
{
	//set default variables
	//teleport distance
	TeleportDistance = 1000.0f;

	//no cost
	Cost = 0.0f;

	//instancing
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

	//cooldown effect
	CooldownGameplayEffectClass = URTeleportCooldownEffect::StaticClass();

	//input
	InputID = RAbilityInputID::Teleport;

	//associated tags
	ActivationOwnedTags = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("State.Busy")));
	ActivationBlockedTags = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("State.Busy")));
}

void URAbilityTeleport::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//line trace logic
	//create FHitResult
	FHitResult HitResult(ForceInit);

	//create collision query params
	FCollisionQueryParams InParams = FCollisionQueryParams(TEXT("TeleportTrace"), true, Cast<AActor>(ActorInfo->OwnerActor));

	//vector setup
	//get actor reference component
	UStaticMeshComponent* StartComp;

	if (Cast<ARCharacterAbilityBase>(ActorInfo->OwnerActor) != NULL)
	{
		StartComp = Cast<ARCharacterAbilityBase>(ActorInfo->OwnerActor)->RunicAttachment;

		//get beginning vector
		FVector BeginVector = StartComp->GetComponentLocation();

		//get ending vector (forward vector of StartComp * teleport distance, all added to beginning vector)
		FVector EndVector = UKismetMathLibrary::GetForwardVector(StartComp->GetComponentRotation());
		EndVector = EndVector * TeleportDistance;
		EndVector += BeginVector;

		//line trace
		GetWorld()->LineTraceSingleByChannel(HitResult, BeginVector, EndVector, ECollisionChannel::ECC_Camera, InParams);

		//after line trace
		//check if trace resulted in hit
		if (HitResult.bBlockingHit == true)
		{
			//spawn attached emitter for effect
			UParticleSystem* ParticleEffect = Cast<ARCharacterAbilityBase>(ActorInfo->OwnerActor)->TeleportEffect;
			UGameplayStatics::SpawnEmitterAttached(ParticleEffect, StartComp);

			//teleport to hit distance
			ActorInfo->OwnerActor->TeleportTo(HitResult.Location, StartComp->GetComponentRotation());

			//commit ability
			CommitAbility(Handle, ActorInfo, ActivationInfo);

			//end ability
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
		else if (HitResult.bBlockingHit == false)
		{
			//spawn attached emitter for effect
			UParticleSystem* ParticleEffect = Cast<ARCharacterAbilityBase>(ActorInfo->OwnerActor)->TeleportEffect;
			UGameplayStatics::SpawnEmitterAttached(ParticleEffect, StartComp);

			//teleport to initial ending vector
			ActorInfo->OwnerActor->TeleportTo(EndVector, StartComp->GetComponentRotation());

			//commit ability
			CommitAbility(Handle, ActorInfo, ActivationInfo);

			//end ability
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		//fallback end ability
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

}