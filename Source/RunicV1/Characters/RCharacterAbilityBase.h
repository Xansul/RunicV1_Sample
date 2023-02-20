// Tyler Call 2022

#pragma once

#include "CoreMinimal.h"
#include "RCharacterBaseNew.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "RCharacterAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class RUNICV1_API ARCharacterAbilityBase : public ARCharacterBaseNew, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	//constructor
	ARCharacterAbilityBase();

	//Implement IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//Override PossesedBy function to initialize ASC
	virtual void PossessedBy(AController* NewController) override;

	//Transform variable that shows where to spawn runic effects (NO LONGER IN USE)
	UPROPERTY(BlueprintReadWrite)
	FTransform RunicTransform;

	//Component that shows where to attach runic effects
	UPROPERTY(BlueprintReadWrite)
	UStaticMeshComponent* RunicAttachment;

	//Bool that reflects if character is blocking
	UPROPERTY(BlueprintReadWrite, Category = "Shield")
	bool IsBlocking;

	//Variable for base flow return on successful shield cast
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shield")
	float FlowReturnBase;

	//Shield angle check - determines if shield will block damage
	UFUNCTION(BlueprintCallable, Category = "Shield")
	void ShieldCheck(AActor* DamageSource, float ShieldTimer, float BaseDamage);

	//Particle system used for teleport effect
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Teleport")
	UParticleSystem* TeleportEffect;

	virtual void Death(ARCharacterBaseNew* Character) override;

	//Combo system
	//*

	//Bool that reflects if character is attacking (used for combo system)
	UPROPERTY(BlueprintReadWrite, Category = "Combo")
	bool IsAttacking;

	//Bool that reflects if attack should be saved (used for combo system, check paragon character BP for reference)
	UPROPERTY(BlueprintReadWrite, Category = "Combo")
	bool AttackSaved;

	//Variable that keeps track of attack count (used for combo system)
	UPROPERTY(BlueprintReadWrite, Category = "Combo")
	int AttackCount;

	//Active montage array used for combo system - pulls from selected array options below
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combo")
	TArray<UAnimMontage*> ActiveMontageArray;

	//Default empty montage array
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combo")
	TArray<UAnimMontage*> EmptyMontageArray = {};

	//Sword montage array
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combo")
	TArray<UAnimMontage*> SwordMontageArray;

	//Function that handles initial attack in combo system (check paragon character BP for reference)
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ComboAttack();

	//Function that handles saved attack in combo system (check paragon character BP for reference)
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ComboAttackSaved();

	//Function that handles attack reset in combo system (check paragon character BP for reference)
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ComboReset();

	//Function that handles montage execution for combo system (check paragon character BP for reference)
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ComboPlayMontage();

	//*

	//Socket names used for equipping items
	//*

	//Right hand socket
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Items")
	FName RightHandSocket;

	//Left hand socket
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Items")
	FName LeftHandSocket;

	//*

	//Anim BPs used for character, can be changed when equipping weapons
	//*

	//Default unarmed
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animations")
	UAnimBlueprint* UnarmedAnimBP;

	//Default sword
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animations")
	UAnimBlueprint* SwordAnimBP;

	//*

	//Variable to hold box collision used for character interaction boundaries - set by individual character BPs
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	UActorComponent* InteractionBox;

	//Function that is called when character attempts to interact - takes in array of objects
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact(TArray<AActor*> ActorArray);

protected:

	UPROPERTY(BlueprintReadOnly)
	class URAbilitySystemComponent* AbilitySystemComponent;
};
