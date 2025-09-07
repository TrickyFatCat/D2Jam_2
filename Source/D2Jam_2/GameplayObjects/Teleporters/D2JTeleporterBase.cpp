// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JTeleporterBase.h"

#include "TrickyGameModeBase.h"
#include "TrickyGameModeLibrary.h"
#include "TrickyUtilityLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/MeshComponent.h"
#include "Components/TimelineComponent.h"
#include "GameplayObject/GameplayObjectStateControllerComponent.h"
#include "Kismet/GameplayStatics.h"


class ATrickyGameModeBase;

AD2JTeleporterBase::AD2JTeleporterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ActivationTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("ActivationTrigger"));
	ActivationTrigger->SetupAttachment(GetRootComponent());
	ActivationTrigger->SetCollisionProfileName(TEXT("PawnTrigger"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	AnimationComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("AnimationComponent"));
}

#if WITH_EDITOR
void AD2JTeleporterBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);


	const EGameplayObjectState InitialState = bIsActiveOnStart
		                                          ? EGameplayObjectState::Active
		                                          : EGameplayObjectState::Inactive;
	StateControllerComponent->SetInitialState(InitialState);

	MeshComponent->SetHiddenInGame(!bIsActiveOnStart, true);

	const ECollisionEnabled::Type CollisionType = bIsActiveOnStart
		                                              ? ECollisionEnabled::QueryOnly
		                                              : ECollisionEnabled::NoCollision;
	ActivationTrigger->SetCollisionEnabled(CollisionType);

	if (IsValid(AnimationComponent))
	{
		UTrickyUtilityLibrary::CalculateTimelinePlayRate(AnimationComponent, ActivationDelay);
		AnimationComponent->SetFloatCurve(AnimationCurve, "Progress");
	}
}
#endif

void AD2JTeleporterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		ActivationTrigger->OnComponentBeginOverlap.AddUniqueDynamic(this,
		                                                            &AD2JTeleporterBase::HandleTriggerBeginOverlap);

		StateControllerComponent->OnGameplayObjectStateChanged.AddDynamic(this,
		                                                                  &AD2JTeleporterBase::HandleStateChanged);

		FOnTimelineFloat TeleportAnimationDelegate;
		TeleportAnimationDelegate.BindUFunction(this, TEXT("AnimateTargetActor"));
		AnimationComponent->SetTimelineLength(1.f);
		AnimationComponent->AddInterpFloat(AnimationCurve, TeleportAnimationDelegate, NAME_None, "Progress");

		FOnTimelineEvent FinishAnimationDelegate;
		FinishAnimationDelegate.BindUFunction(this, TEXT("FinishAnimation"));
		AnimationComponent->SetTimelineFinishedFunc(FinishAnimationDelegate);
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
		MeshComponent->SetHiddenInGame(false, true);
		break;

	case EGameplayObjectState::Inactive:
		ActivationTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComponent->SetHiddenInGame(true, true);
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
	if (!IsValid(OtherActor))
	{
		return;
	}
	ATrickyGameModeBase* GameMode = UTrickyGameModeLibrary::GetTrickyGameMode(this);

	if (IsValid(GameMode))
	{
		GameMode->Execute_FinishGame(GameMode, EGameResult::Win);
	}

	if (ActivationDelay <= 0.f)
	{
		FinishAnimation();
		return;
	}

	TargetActor = OtherActor;
	TargetActor->SetActorEnableCollision(ECollisionEnabled::NoCollision);
	TargetActor->SetCanBeDamaged(false);
	TargetActor->SetActorLocationAndRotation(GetActorLocation(), GetActorRotation());
	TargetLocation = GetActorLocation();
	TargetLocation.Z += ActorDeltaLocation;
	AnimationComponent->PlayFromStart();

	ActivationTrigger->OnComponentBeginOverlap.RemoveDynamic(this, &AD2JTeleporterBase::HandleTriggerBeginOverlap);
}

void AD2JTeleporterBase::HandleCameraFadeInFinished()
{
}

void AD2JTeleporterBase::AnimateTargetActor(const float Progress)
{
	if (!TargetActor.IsValid())
	{
		return;
	}

	FVector NewLocation = FMath::Lerp(GetActorLocation(), TargetLocation, Progress);
	TargetActor->SetActorLocation(NewLocation);
}

void AD2JTeleporterBase::FinishAnimation()
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
