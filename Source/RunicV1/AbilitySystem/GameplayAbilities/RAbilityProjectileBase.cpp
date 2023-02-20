// Tyler Call 2022


#include "RAbilityProjectileBase.h"

// Sets default values
ARAbilityProjectileBase::ARAbilityProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARAbilityProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARAbilityProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

