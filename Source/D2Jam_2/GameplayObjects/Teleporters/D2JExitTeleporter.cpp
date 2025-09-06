// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JExitTeleporter.h"

#include "D2Jam_2/PlayerCharacter/D2JPlayerInterface.h"
#include "Kismet/GameplayStatics.h"


AD2JExitTeleporter::AD2JExitTeleporter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AD2JExitTeleporter::HandleTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                   AActor* OtherActor,
                                                   UPrimitiveComponent* OtherComp,
                                                   int32 OtherBodyIndex,
                                                   bool bFromSweep,
                                                   const FHitResult& SweepResult)
{
	Super::HandleTriggerBeginOverlap(OverlappedComponent,
	                                 OtherActor,
	                                 OtherComp,
	                                 OtherBodyIndex,
	                                 bFromSweep,
	                                 SweepResult);
	
	if (!OtherActor->GetClass()->ImplementsInterface(UD2JPlayerInterface::StaticClass()))
	{
		return;
	}

	ID2JPlayerInterface* PlayerInterface = Cast<ID2JPlayerInterface>(OtherActor);
	PlayerInterface->ToggleInput(false);
}

void AD2JExitTeleporter::HandleCameraFadeInFinished()
{
	Super::HandleCameraFadeInFinished();

	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}
