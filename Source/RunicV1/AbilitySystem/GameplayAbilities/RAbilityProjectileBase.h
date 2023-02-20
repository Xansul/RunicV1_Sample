// Tyler Call 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "RAbilityProjectileBase.generated.h"

UCLASS()
class RUNICV1_API ARAbilityProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARAbilityProjectileBase();

	//Variable to hold GameplayAbilitySpec that the projectile can pass to the target
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect", meta = (ExposeOnSpawn = "true"))
	float Damage;

	//Variable to hold initial spawn vector
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect", meta=(ExposeOnSpawn = "true"))
	FVector InitialVector;

	//Variable to hold actor who activated ability
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect", meta = (ExposeOnSpawn = "true"))
	AActor* OwnerActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
