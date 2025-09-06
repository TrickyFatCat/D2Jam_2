// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JStarPickup.h"

#include "Components/SphereComponent.h"
#include "D2Jam_2/PlayerCharacter/D2JPlayerInterface.h"
#include "GameplayObject/GameplayObjectStateControllerComponent.h"


AD2JStarPickup::AD2JStarPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	StateControllerComponent = CreateDefaultSubobject<UGameplayObjectStateControllerComponent>(TEXT("StateController"));
}

#if WITH_EDITOR
void AD2JStarPickup::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const EGameplayObjectState InitialState = bIsActiveOnStart
		                                          ? EGameplayObjectState::Active
		                                          : EGameplayObjectState::Disabled;
	StateControllerComponent->SetInitialState(InitialState);

	const ECollisionEnabled::Type CollisionType = bIsActiveOnStart
		                                              ? ECollisionEnabled::QueryOnly
		                                              : ECollisionEnabled::NoCollision;
	ActivationTrigger->SetCollisionEnabled(CollisionType);

	MeshComponent->SetHiddenInGame(!bIsActiveOnStart);
}
#endif

void AD2JStarPickup::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (IsValid(StateControllerComponent))
	{
		StateControllerComponent->OnGameplayObjectStateChanged.AddDynamic(this, &AD2JStarPickup::HandleStateChanged);
	}
}

bool AD2JStarPickup::ActivateGameplayObject_Implementation(const bool bTransitImmediately)
{
	return Execute_ActivateGameplayObject(StateControllerComponent, bTransitImmediately);
}

bool AD2JStarPickup::DeactivateGameplayObject_Implementation(const bool bTransitImmediately)
{
	return Execute_DeactivateGameplayObject(StateControllerComponent, bTransitImmediately);
}

bool AD2JStarPickup::CanBeActivated_Implementation(AActor* Activator)
{
	return Activator->GetClass()->ImplementsInterface(UD2JPlayerInterface::StaticClass());
}

void AD2JStarPickup::HandleActivationSuccess_Implementation(AActor* Activator)
{
	Cast<ID2JPlayerInterface>(Activator)->AddStar();
}

void AD2JStarPickup::HandleStateChanged(UGameplayObjectStateControllerComponent* Component,
                                        EGameplayObjectState NewState,
                                        bool bChangedImmediately)
{
	switch (NewState)
	{
	case EGameplayObjectState::Active:
		MeshComponent->SetHiddenInGame(false);
		ActivationTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		break;

	case EGameplayObjectState::Inactive:
		MeshComponent->SetHiddenInGame(true);
		ActivationTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}
