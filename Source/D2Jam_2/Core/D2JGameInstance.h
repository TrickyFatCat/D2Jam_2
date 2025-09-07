// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "D2JSaveGame.h"
#include "Engine/GameInstance.h"
#include "D2JGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveGameLoadedDynamicSignature);

/**
 * 
 */
UCLASS()
class D2JAM_2_API UD2JGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "SaveGame")
	FOnSaveGameLoadedDynamicSignature OnSaveGameLoaded;
	
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool SaveGameData();

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool LoadGameData(TArray<FLevelData>& OutSaveGameData);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void GetSavedLevelData(TArray<FLevelData>& OutSavedLevelData);
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
	TObjectPtr<UD2JSaveGame> SaveGame = nullptr;

	FString SaveGameName = "D2JSaveGame";

	TArray<FLevelData> SavedLevelData;
};
