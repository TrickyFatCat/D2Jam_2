// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JTeleporterBase.h"

#include "Components/SphereComponent.h"
#include "D2Jam_2/PlayerCharacter/D2JPlayerInterface.h"
#include "Kismet/GameplayStatics.h"


AD2JTeleporterBase::AD2JTeleporterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ActivationTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("ActivationTrigger"));
	ActivationTrigger->SetupAttachment(GetRootComponent());
	ActivationTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ActivationTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	ActivationTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
}

void AD2JTeleporterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		ActivationTrigger->OnComponentBeginOverlap.AddUniqueDynamic(this,
		                                                            &AD2JTeleporterBase::HandleTriggerBeginOverlap);
	}
}

void AD2JTeleporterBase::HandleTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                   AActor* OtherActor,
                                                   UPrimitiveComponent* OtherComp,
                                                   int32 OtherBodyIndex,
                                                   bool bFromSweep,
                                                   const FHitResult& SweepResult)
{
	if (!OtherActor->GetClass()->ImplementsInterface(UD2JPlayerInterface::StaticClass()))
	{
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
