// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JStarPickup.h"

#include "Components/SphereComponent.h"
#include "D2Jam_2/GameplayObjects/Teleporters/D2JTeleporterBase.h"
#include "D2Jam_2/PlayerCharacter/D2JPlayerInterface.h"
#include "GameplayObject/GameplayObjectStateControllerComponent.h"
#include "Kismet/GameplayStatics.h"


AD2JStarPickup::AD2JStarPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	StateControllerComponent = CreateDefaultSubobject<UGameplayObjectStateControllerComponent>(TEXT("StateController"));

	bDestroyAfterActivation = false;
}

#if WITH_EDITOR
void AD2JStarPickup::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const EGameplayObjectState InitialState = bIsActiveOnStart
		                                          ? EGameplayObjectState::Active
		                                          : EGameplayObjectState::Inactive;
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
	ID2JPlayerInterface* PlayerInterface = Cast<ID2JPlayerInterface>(Activator);
	PlayerInterface->AddStar();
	PlayerInterface->SetSpawnLocation(GetActorLocation());

	if (IsValid(NextStar))
	{
		Execute_ActivateGameplayObject(NextStar, true);
	}

	Execute_DeactivateGameplayObject(StateControllerComponent, true);

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
		MeshComponent->SetHiddenInGame(false);

		if (SublevelToLoad.GetAssetName() != "")
		{
			FLatentActionInfo LatentActionInfo;
			LatentActionInfo.CallbackTarget = this;
			LatentActionInfo.ExecutionFunction = FName("OnLevelLoaded");
			LatentActionInfo.UUID = GetUniqueID();
			LatentActionInfo.Linkage = 0;
			UGameplayStatics::LoadStreamLevelBySoftObjectPtr(this,
			                                                 SublevelToLoad,
			                                                 true,
			                                                 true,
			                                                 LatentActionInfo);
		}
		break;

	case EGameplayObjectState::Inactive:
		MeshComponent->SetHiddenInGame(true);
		ActivationTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		MeshComponent->SetHiddenInGame(true);

		if (TeleporterToActivate.IsValid())
		{
			Execute_ActivateGameplayObject(TeleporterToActivate.Get(), true);
		}
		break;
	}
}

void AD2JStarPickup::OnLevelLoaded()
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Log, TEXT("Sublevel loaded."))
#endif
}
