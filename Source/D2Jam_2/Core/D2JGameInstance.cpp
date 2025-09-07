// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JGameInstance.h"

#include "D2JGameMode.h"
#include "D2JSaveGame.h"
#include "D2Jam_2/PlayerCharacter/D2JPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

bool UD2JGameInstance::SaveGameData()
{
	if (!IsValid(SaveGame))
	{
		SaveGame = Cast<UD2JSaveGame>(UGameplayStatics::CreateSaveGameObject(UD2JSaveGame::StaticClass()));
	}

	UWorld* World = GetWorld();

	if (!IsValid(World) || !World->IsGameWorld())
	{
		return false;
	}

	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World);
	UE_LOG(LogTemp, Warning, TEXT("Current Level Name: %s"), *CurrentLevelName);

	if (CurrentLevelName == "")
	{
		return false;
	}

	auto Predicate = [CurrentLevelName](const FLevelData& LevelData)
	{
		return CurrentLevelName == LevelData.LevelName;
	};
	const bool HasLevelData = SaveGame->SaveGameData.ContainsByPredicate(Predicate);
	const AD2JPlayerCharacter* Player = Cast<AD2JPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
	const AD2JGameMode* GameMode = Cast<AD2JGameMode>(World->GetAuthGameMode());

	if (HasLevelData)
	{
		FLevelData* CurrentLevelData = SaveGame->SaveGameData.FindByPredicate(Predicate);
		CurrentLevelData->Fails = FMath::Min(CurrentLevelData->Fails, Player->GetFailureCounter());
		CurrentLevelData->Time = FMath::Min(CurrentLevelData->Time, GameMode->Execute_GetGameElapsedTime(GameMode));
	}
	else
	{
		FLevelData NewLevelData;
		NewLevelData.LevelName = CurrentLevelName;
		NewLevelData.Fails = Player->GetFailureCounter();
		NewLevelData.Time = GameMode->Execute_GetGameElapsedTime(GameMode);
		SaveGame->SaveGameData.Add(NewLevelData);
	}

	return UGameplayStatics::SaveGameToSlot(SaveGame, SaveGameName, 0);
}

bool UD2JGameInstance::LoadGameData(TArray<FLevelData>& OutSaveGameData)
{
	SaveGame = Cast<UD2JSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveGameName, 0));

	if (!IsValid(SaveGame))
	{
		return false;
	}

	SavedLevelData = SaveGame->SaveGameData;
	OutSaveGameData = SavedLevelData;
	OnSaveGameLoaded.Broadcast();
	return true;
}

void UD2JGameInstance::GetSavedLevelData(TArray<FLevelData>& OutSavedLevelData)
{
	OutSavedLevelData = SavedLevelData;
}