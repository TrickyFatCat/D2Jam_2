// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JTeleporterBase.h"

#include "Components/SphereComponent.h"
#include "Components/MeshComponent.h"
#include "GameplayObject/GameplayObjectStateControllerComponent.h"
#include "Kismet/GameplayStatics.h"


AD2JTeleporterBase::AD2JTeleporterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ActivationTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("ActivationTrigger"));
	ActivationTrigger->SetupAttachment(GetRootComponent());
	ActivationTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ActivationTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	ActivationTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
}

void AD2JTeleporterBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);


	const EGameplayObjectState InitialState = bIsActiveOnStart
		                                          ? EGameplayObjectState::Active
		                                          : EGameplayObjectState::Inactive;
	StateControllerComponent->SetInitialState(InitialState);

	MeshComponent->SetHiddenInGame(!bIsActiveOnStart);

	const ECollisionEnabled::Type CollisionType = bIsActiveOnStart
		                                              ? ECollisionEnabled::QueryOnly
		                                              : ECollisionEnabled::NoCollision;
	ActivationTrigger->SetCollisionEnabled(CollisionType);
}

void AD2JTeleporterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		ActivationTrigger->OnComponentBeginOverlap.AddUniqueDynamic(this,
		                                                            &AD2JTeleporterBase::HandleTriggerBeginOverlap);

		StateControllerComponent->OnGameplayObjectStateChanged.AddDynamic(this,
		                                                                 &AD2JTeleporterBase::HandleStateChanged);
	}
}

void AD2JTeleporterBase::HandleStateChanged(UGameplayObjectStateControllerComponent* Component,
                                            EGameplayObjectState NewState,
                                            bool bChangedImmediately)
{
	
	switch (NewState)
	{
	case EGameplayObjectState::Active:
		ActivationTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		MeshComponent->SetHiddenInGame(false);
		break;

	case EGameplayObjectState::Inactive:
		ActivationTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComponent->SetHiddenInGame(true);
		break;
	}
}

void AD2JTeleporterBase::HandleTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                   AActor* OtherActor,
                                                   UPrimitiveComponent* OtherComp,
                                                   int32 OtherBodyIndex,
                                                   bool bFromSweep,
                                                   const FHitResult& SweepResult)
{
	if (ActivationDelay <= 0.f)
	{
		HandleActivationTimerFinished();
		return;
	}

	GetWorldTimerManager().SetTimer(ActivationTimerHandle,
	                                this,
	                                &AD2JTeleporterBase::HandleActivationTimerFinished,
	                                ActivationDelay);
}

void AD2JTeleporterBase::HandleActivationTimerFinished()
{
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	if (IsValid(CameraManager))
	{
		CameraManager->StartCameraFade(0.f, 1.f, CameraFadeInDuration, FLinearColor::Black, true, true);
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle,
		                                this,
		                                &AD2JTeleporterBase::HandleCameraFadeInFinished,
		                                CameraFadeInDuration,
		                                false);
	}
}

void AD2JTeleporterBase::HandleCameraFadeInFinished()
{
}
