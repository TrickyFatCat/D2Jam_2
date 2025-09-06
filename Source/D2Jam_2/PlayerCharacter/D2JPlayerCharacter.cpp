// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JPlayerCharacter.h"

#include "D2JPlayerMovementComponent.h"
#include "GameStateControllerInterface.h"
#include "TrickyGameModeLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"

AD2JPlayerCharacter::AD2JPlayerCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UD2JPlayerMovementComponent>(TEXT("CharMoveComp")))
{
	PrimaryActorTick.bCanEverTick = true;
}

void AD2JPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (IsValid(PlayerController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void AD2JPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AD2JPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Movement
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AD2JPlayerCharacter::Move);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AD2JPlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction,
		                                   ETriggerEvent::Completed,
		                                   this,
		                                   &AD2JPlayerCharacter::StopJumping);
	}
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

void AD2JPlayerCharacter::CalculateDirectionFromControls(const FVector2D& ControlValue, FVector& OutDirection) const
{
	FVector2D Direction2D = FVector2D::Zero();
	double ControlLength = 0.0;
	ControlValue.ToDirectionAndLength(Direction2D, ControlLength);

	const FVector Direction3D = FVector(Direction2D, 0.0f);
	const FRotator Rotation = UKismetMathLibrary::ComposeRotators(Direction3D.Rotation(),
	                                                              FRotator(0.f, YawOffset, 0.f));
	OutDirection = Rotation.Quaternion().GetForwardVector();
}

void AD2JPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D ControlVector = Value.Get<FVector2D>();
	const double ControlLength = ControlVector.Length();
	FVector MovementDirection = FVector::ZeroVector;
	CalculateDirectionFromControls(ControlVector, MovementDirection);

	AddMovementInput(MovementDirection, ControlLength);
}
