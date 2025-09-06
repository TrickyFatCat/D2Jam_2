// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JPlayerCharacter.h"

#include "GameStateControllerInterface.h"
#include "TrickyGameModeLibrary.h"


AD2JPlayerCharacter::AD2JPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AD2JPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AD2JPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AD2JPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AD2JPlayerCharacter::AddStar()
{
	if (Stars.IncreaseValue(1))
	{
		Stars.ClampToMax();
		OnStarAdded.Broadcast(Stars);
	}

	if (Stars.ReachedMaxValue())
	{
		UTrickyGameModeLibrary::FinishGame(this, EGameResult::Win);
	}
}

void AD2JPlayerCharacter::GetStarsData(FTrickyPropertyInt& OutStarsData) const
{
	OutStarsData = Stars;
}

