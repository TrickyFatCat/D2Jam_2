// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "D2Jam_2/GameplayObjects/D2JGameplayObjectBase.h"
#include "D2JTeleporterBase.generated.h"

class USphereComponent;

UCLASS()
class D2JAM_2_API AD2JTeleporterBase : public AD2JGameplayObjectBase
{
	GENERATED_BODY()

public:
	AD2JTeleporterBase();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Component")
	TObjectPtr<USphereComponent> ActivationTrigger = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UMeshComponent> MeshComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Teleporter")
	float ActivationDelay = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Teleporter")
	float CameraFadeInDuration = 0.5f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Teleporter")
	TSoftObjectPtr<UWorld> SublevelToActivate = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Teleporter")
	FTimerHandle ActivationTimerHandle;

	UFUNCTION()
	virtual void HandleStateChanged(UGameplayObjectStateControllerComponent* Component,
	                                EGameplayObjectState NewState,
	                                bool bChangedImmediately);
	
	UFUNCTION()
	virtual void HandleTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	                                       AActor* OtherActor,
	                                       UPrimitiveComponent* OtherComp,
	                                       int32 OtherBodyIndex,
	                                       bool bFromSweep,
	                                       const FHitResult& SweepResult);

	UFUNCTION()
	void HandleActivationTimerFinished();

	UFUNCTION()
	virtual void HandleCameraFadeInFinished();
};
