// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JExitTeleporter.h"

#include "Kismet/GameplayStatics.h"


AD2JExitTeleporter::AD2JExitTeleporter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AD2JExitTeleporter::HandleCameraFadeInFinished()
{
	Super::HandleCameraFadeInFinished();

	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}
