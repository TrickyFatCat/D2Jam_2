// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "D2JPlayerInterface.h"
#include "TrickyPropertiesLibrary.h"
#include "GameFramework/Character.h"
#include "D2JPlayerCharacter.generated.h"

enum class EGameInactivityReason : uint8;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStarAddedDynamicSignature, const FTrickyPropertyInt&, CurrentStars);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllStarsGatheredDynamicSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFailureCounterIncreasedDynamicSignature, int32, CurrentFailures);

UCLASS()
class D2JAM_2_API AD2JPlayerCharacter : public ACharacter, public ID2JPlayerInterface
{
	GENERATED_BODY()

public:
	AD2JPlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintAssignable, Category="Player")
	FOnStarAddedDynamicSignature OnStarAdded;

	UPROPERTY(BlueprintAssignable, Category="Player")
	FOnAllStarsGatheredDynamicSignature OnAllStarsGathered;

	UPROPERTY(BlueprintAssignable, Category="Player")
	FOnFailureCounterIncreasedDynamicSignature OnFailureCounterIncreased;

	virtual void AddStar() override;

	virtual void GetStarsData(FTrickyPropertyInt& OutStarsData) const override;

	virtual void SetSpawnLocation(const FVector NewSpawnLocation) override;

	virtual void ToggleInput(const bool bIsEnabled) override;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Player")
	FTrickyPropertyInt Stars{0, 0, 3};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player")
	float DeathDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player")
	float RespawnFadeInDuration = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player")
	float RespawnDuration = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player")
	float RespawnFadeOutDuration = 0.2f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Player")
	int32 FailureCounter = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Player")
	FVector SpawnLocation = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	float YawOffset = -45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* MappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction = nullptr;

	UFUNCTION(BlueprintCallable)
	void CalculateDirectionFromControls(const FVector2D& ControlValue, FVector& OutDirection) const;

	UFUNCTION()
	void Move(const FInputActionValue& Value);
	
	UFUNCTION()
	void StartRespawn();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRespawnStarted();
	
	UFUNCTION()
	void FinishRespawn();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRespawnFinished();

	UFUNCTION()
	void HandleRespawnFinished();

	UFUNCTION(BlueprintCallable)
	void Respawn();

	UFUNCTION()
	void HandleAnyDamageTaken(AActor* DamagedActor,
	                          float Damage,
	                          const UDamageType* DamageType,
	                          AController* InstigatedBy,
	                          AActor* DamageCauser);

	UFUNCTION()
	void HandleGameStarted();

	UFUNCTION()
	void HandleGameStopped(const EGameInactivityReason InactivityReason);

	UFUNCTION()
	void HandleGameFinished(const EGameResult Result);
};
