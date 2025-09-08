// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JGameMode.h"

#include "Kismet/GameplayStatics.h"

void AD2JGameMode::StartPlay()
{
	Super::StartPlay();

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	if (IsValid(CameraManager))
	{
		CameraManager->StartCameraFade(1.0f, 0.0f, CameraFadeOutDuration, FLinearColor::Black, true, false);
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle,
										this,
										&AD2JGameMode::HandleCameraFadeOutFinished,
										CameraFadeOutDuration,
										false);
	}
}

void AD2JGameMode::HandleCameraFadeOutFinished()
{
	if (GetPreparationDuration() > 0.0f)
	{
		Execute_StartPreparation(this);
	}
	else
	{
		Execute_StartGame(this);
	}
}
