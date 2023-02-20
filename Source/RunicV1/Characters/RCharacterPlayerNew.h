// Tyler Call 2022

#pragma once

#include "CoreMinimal.h"
#include "RCharacterAbilityBase.h"
#include "RunicV1/AbilitySystem/Attributes/RAttributeSetBasePlayer.h"
#include "RunicV1/AbilitySystem/GameplayAbilities/RGameplayAbilityBase.h"
#include "RCharacterPlayerNew.generated.h"

/**
 * 
 */
UCLASS()
class RUNICV1_API ARCharacterPlayerNew : public ARCharacterAbilityBase
{
	GENERATED_BODY()
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	ARCharacterPlayerNew();

	//override IsAlive() function
	virtual bool IsAlive() override;

	//apply gameplay effect to initialize attributes (Not currently used - attributes are initialized in character BP)
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitAttributes();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	float TurnRateGamepad;

protected:

	//Attribute set variable
	UPROPERTY(BlueprintReadOnly, Category = Attributes)
	class URAttributeSetBasePlayer* AttributeSetBase;

	//Bool for enabling movement input
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Input)
	bool CanMoveInput;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
