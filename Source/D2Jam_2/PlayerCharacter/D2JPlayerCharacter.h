// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "D2JPlayerInterface.h"
#include "TrickyPropertiesLibrary.h"
#include "GameFramework/Character.h"
#include "D2JPlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStarAddedDynamicSignature, const FTrickyPropertyInt&, CurrentStars);

UCLASS()
class D2JAM_2_API AD2JPlayerCharacter : public ACharacter, public ID2JPlayerInterface
{
	GENERATED_BODY()

public:
	AD2JPlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintAssignable, Category="Player")
	FOnStarAddedDynamicSignature OnStarAdded;

	virtual void AddStar() override;

	virtual void GetStarsData(FTrickyPropertyInt& OutStarsData) const override;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Player")
	FTrickyPropertyInt Stars {0, 0, 3};
};
