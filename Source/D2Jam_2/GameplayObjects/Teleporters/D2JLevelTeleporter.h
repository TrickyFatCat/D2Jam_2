// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "D2JTeleporterBase.h"
#include "D2Jam_2/Core/D2JSaveGame.h"
#include "D2JLevelTeleporter.generated.h"

class UD2JGameInstance;

UCLASS()
class D2JAM_2_API AD2JLevelTeleporter : public AD2JTeleporterBase
{
	GENERATED_BODY()

public:
	AD2JLevelTeleporter();

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Teleporter")
	bool bIsFinish = false;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Teleporter", meta=(EditCondition="!bIsFinish"))
	TSoftObjectPtr<UWorld> RequiredLevel = nullptr;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Teleporter")
	TSoftObjectPtr<UWorld> LevelToLoad = nullptr;

	virtual void HandleStateChanged(UGameplayObjectStateControllerComponent* Component,
	                                EGameplayObjectState NewState,
	                                bool bChangedImmediately) override;

	virtual void HandleTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	                                       AActor* OtherActor,
	                                       UPrimitiveComponent* OtherComp,
	                                       int32 OtherBodyIndex,
	                                       bool bFromSweep,
	                                       const FHitResult& SweepResult) override;

	virtual void HandleCameraFadeInFinished() override;

	UFUNCTION()
	void HandleAllStarsGathered();

	UFUNCTION(BlueprintPure)
	void GetLevelToLoadName(FString& OutName) const;

	UPROPERTY(BlueprintReadOnly, Category="Teleporter")
	FLevelData LevelSaveData;

	UFUNCTION()
	void HandleSaveDataLoaded();
};
