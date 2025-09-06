// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrickyPropertiesLibrary.h"
#include "UObject/Interface.h"
#include "D2JPlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UD2JPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class D2JAM_2_API ID2JPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, Category=D2JPlayerInterface)
	virtual void AddStar() = 0;

	UFUNCTION(BlueprintCallable, Category=D2JPlayerInterface)
	virtual void GetStarsData(FTrickyPropertyInt& OutStarsData) const = 0;

	UFUNCTION()
	virtual void SetSpawnLocation(const FVector NewSpawnLocation) = 0;
};
