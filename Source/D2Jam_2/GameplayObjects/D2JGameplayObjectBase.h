// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayObject/GameplayObjectInterface.h"
#include "D2JGameplayObjectBase.generated.h"

class UGameplayObjectStateControllerComponent;

UCLASS()
class D2JAM_2_API AD2JGameplayObjectBase : public AActor, public IGameplayObjectInterface
{
	GENERATED_BODY()

public:
	AD2JGameplayObjectBase();

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	virtual bool ActivateGameplayObject_Implementation(const bool bTransitImmediately) override;

	virtual bool DeactivateGameplayObject_Implementation(const bool bTransitImmediately) override;

protected:
	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UGameplayObjectStateControllerComponent> StateControllerComponent = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="GameplayObject")
	bool bIsActiveOnStart = true;
};
