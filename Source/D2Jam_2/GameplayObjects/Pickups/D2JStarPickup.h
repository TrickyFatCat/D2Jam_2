// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayObject/GameplayObjectInterface.h"
#include "Pickup/PickupAutomaticBase.h"
#include "D2JStarPickup.generated.h"

class AD2JTeleporterBase;
class UGameplayObjectStateControllerComponent;

UCLASS()
class D2JAM_2_API AD2JStarPickup : public APickupAutomaticBase, public IGameplayObjectInterface
{
	GENERATED_BODY()

public:
	AD2JStarPickup();

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void PostInitializeComponents() override;

public:
	virtual bool ActivateGameplayObject_Implementation(const bool bTransitImmediately) override;

	virtual bool DeactivateGameplayObject_Implementation(const bool bTransitImmediately) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Components")
	TObjectPtr<UGameplayObjectStateControllerComponent> StateControllerComponent = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Pickup")
	bool bIsActiveOnStart = true;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<AD2JStarPickup> NextStar = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Pickup")
	TSoftObjectPtr<UWorld> SublevelToLoad = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Pickup")
	TWeakObjectPtr<AD2JTeleporterBase> TeleporterToActivate = nullptr;

	virtual bool CanBeActivated_Implementation(AActor* Activator) override;

	virtual void HandleActivationSuccess_Implementation(AActor* Activator) override;

	UFUNCTION()
	void HandleStateChanged(UGameplayObjectStateControllerComponent* Component,
	                        EGameplayObjectState NewState,
	                        bool bChangedImmediately);

	UFUNCTION()
	void OnLevelLoaded();
};
