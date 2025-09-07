// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "D2JSaveGame.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "D2JUtils.generated.h"

class UD2JGameInstance;
/**
 * 
 */
UCLASS()
class D2JAM_2_API UD2JUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Utils", meta=(DefaultToSelf="WorldContextObject"))
	static UD2JGameInstance* GetJamInstance(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Utils", meta=(AutoCreateRefTerm="LevelName", DefaultToSelf="WorldContextObject"))
	static bool GetLevelSaveData(UObject* WorldContextObject, UPARAM(ref) const FString& LevelName, FLevelData& OutLevelData);
};
