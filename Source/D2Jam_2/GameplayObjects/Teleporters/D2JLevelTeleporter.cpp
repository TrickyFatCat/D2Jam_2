// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JLevelTeleporter.h"

#include "D2Jam_2/Core/D2JGameInstance.h"
#include "D2Jam_2/Core/D2JUtils.h"
#include "D2Jam_2/PlayerCharacter/D2JPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"


AD2JLevelTeleporter::AD2JLevelTeleporter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AD2JLevelTeleporter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AD2JLevelTeleporter::BeginPlay()
{
	Super::BeginPlay();

	if (bIsFinish)
	{
		AD2JPlayerCharacter* Player = Cast<AD2JPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

		if (IsValid(Player) && !Player->OnAllStarsGathered.IsAlreadyBound(
			this, &AD2JLevelTeleporter::HandleAllStarsGathered))
		{
			Player->OnAllStarsGathered.AddDynamic(this, &AD2JLevelTeleporter::HandleAllStarsGathered);
		}
	}
	else
	{
		UD2JGameInstance* GameInstance = Cast<UD2JGameInstance>(UGameplayStatics::GetGameInstance(this));

		if (IsValid(GameInstance))
		{
			GameInstance->OnSaveGameLoaded.AddDynamic(this, &AD2JLevelTeleporter::HandleSaveDataLoaded);
		}
	}
}

void AD2JLevelTeleporter::HandleStateChanged(UGameplayObjectStateControllerComponent* Component,
                                             EGameplayObjectState NewState,
                                             bool bChangedImmediately)
{
	Super::HandleStateChanged(Component, NewState, bChangedImmediately);

	if (NewState == EGameplayObjectState::Active)
	{
		if (LevelToLoad.GetAssetName() != "")
		{
			FLatentActionInfo LatentActionInfo;
			UGameplayStatics::LoadStreamLevelBySoftObjectPtr(this,
			                                                 SublevelToActivate,
			                                                 true,
			                                                 true,
			                                                 LatentActionInfo);
		}
	}
}

void AD2JLevelTeleporter::HandleTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent,
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

	if (bIsFinish)
	{
		UD2JGameInstance* GameInstance = Cast<UD2JGameInstance>(UGameplayStatics::GetGameInstance(this));

		if (IsValid(GameInstance))
		{
			GameInstance->SaveGameData();
		}
	}
}

void AD2JLevelTeleporter::HandleCameraFadeInFinished()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, LevelToLoad, true);
}

void AD2JLevelTeleporter::HandleAllStarsGathered()
{
	Execute_ActivateGameplayObject(this, true);
}

void AD2JLevelTeleporter::GetLevelToLoadName(FString& OutName) const
{
	OutName = LevelToLoad.GetAssetName();
}

void AD2JLevelTeleporter::HandleSaveDataLoaded()
{
	bool bHasSavedData = UD2JUtils::GetLevelSaveData(this, LevelToLoad.GetAssetName(), LevelSaveData);

	if (bHasSavedData)
	{
		Execute_ActivateGameplayObject(this, true);
		OnSaveDataLoaded();
		return;
	}

	if (RequiredLevel.GetAssetName() == "")
	{
		Execute_ActivateGameplayObject(this, true);
		return;
	}

	FLevelData RequiredLevelData;
	bHasSavedData = UD2JUtils::GetLevelSaveData(this, RequiredLevel.GetAssetName(), RequiredLevelData);

	if (bHasSavedData)
	{
		Execute_ActivateGameplayObject(this, true);
		OnSaveDataLoaded();
	}
	else
	{
		Execute_DeactivateGameplayObject(this, true);
	}
}
