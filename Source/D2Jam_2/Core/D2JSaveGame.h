// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "D2JSaveGame.generated.h"

USTRUCT(Blueprintable)
struct FLevelData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveGameData")
	FString LevelName = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveGameData")
	float Time = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveGameData")
	int32 Fails = 0;
};
/**
 * 
 */
UCLASS()
class D2JAM_2_API UD2JSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "SaveGameData")
	TArray<FLevelData> SaveGameData;
};
