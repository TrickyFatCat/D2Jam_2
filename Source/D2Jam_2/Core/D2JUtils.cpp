// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JUtils.h"

#include "D2JGameInstance.h"
#include "Kismet/GameplayStatics.h"

UD2JGameInstance* UD2JUtils::GetJamInstance(UObject* WorldContextObject)
{
	return Cast<UD2JGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

bool UD2JUtils::GetLevelSaveData(UObject* WorldContextObject, const FString& LevelName, FLevelData& OutLevelData)
{
	if (!IsValid(WorldContextObject))
	{
		return false;
	}

	UD2JGameInstance* JamInstance = GetJamInstance(WorldContextObject);

	if (!IsValid(JamInstance))
	{
		return false;
	}

	TArray<FLevelData> LevelData;
	JamInstance->GetLevelData(LevelData);

	if (LevelData.IsEmpty())
	{
		return false;
	}

	auto Predicate = [LevelName](const FLevelData& Data) { return Data.LevelName == LevelName; };
	const FLevelData* TargetLevelData = LevelData.FindByPredicate(Predicate);

	if (!TargetLevelData)
	{
		return false;
	}
	OutLevelData = *TargetLevelData;
	return true;
}
