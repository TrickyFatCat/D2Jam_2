// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JPlayerCharacter.h"

#include "D2JPlayerMovementComponent.h"
#include "GameStateControllerInterface.h"
#include "TrickyGameModeLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TrickyGameModeBase.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
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

	SpawnLocation = GetActorLocation();

	OnTakeAnyDamage.AddUniqueDynamic(this, &AD2JPlayerCharacter::HandleAnyDamageTaken);

	ATrickyGameModeBase* GameMode = UTrickyGameModeLibrary::GetTrickyGameMode(this);

	if (IsValid(GameMode))
	{
		GameMode->OnGameStarted.AddDynamic(this, &AD2JPlayerCharacter::HandleGameStarted);
		GameMode->OnGameStopped.AddDynamic(this, &AD2JPlayerCharacter::HandleGameStopped);
		GameMode->OnGameFinished.AddDynamic(this, &AD2JPlayerCharacter::HandleGameFinished);

		if (GameMode->GetCurrentState() == ETrickyGameState::Inactive)
		{
			ToggleInput(false);
		}
	}
}

void AD2JPlayerCharacter::Jump()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		OnJumpStarted.Broadcast();
	}
	
	Super::Jump();
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
		OnAllStarsGathered.Broadcast();
	}
}

void AD2JPlayerCharacter::GetStarsData(FTrickyPropertyInt& OutStarsData) const
{
	OutStarsData = Stars;
}

void AD2JPlayerCharacter::SetSpawnLocation(const FVector NewSpawnLocation)
{
	SpawnLocation = NewSpawnLocation;
	SpawnLocation.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

void AD2JPlayerCharacter::ToggleInput(const bool bIsEnabled)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (!IsValid(PlayerController))
	{
		return;
	}

	if (bIsEnabled)
	{
		EnableInput(PlayerController);
	}
	else
	{
		DisableInput(PlayerController);
	}

	GetMovementComponent()->StopMovementImmediately();
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

void AD2JPlayerCharacter::StartRespawn()
{
	OnRespawnStarted();

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	if (!IsValid(CameraManager))
	{
		Respawn();
		return;
	}

	CameraManager->StartCameraFade(0.0f, 1.0f, RespawnFadeInDuration, FLinearColor::Black, false, true);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle,
	                                this,
	                                &AD2JPlayerCharacter::Respawn,
	                                RespawnFadeInDuration,
	                                false);
}

void AD2JPlayerCharacter::FinishRespawn()
{
	OnRespawnFinished();
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	if (!IsValid(CameraManager))
	{
		SetCanBeDamaged(true);
		ToggleInput(true);
		return;
	}

	CameraManager->StartCameraFade(1.0f, 0.0f, RespawnFadeOutDuration, FLinearColor::Black, false, false);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle,
	                                this,
	                                &AD2JPlayerCharacter::HandleRespawnFinished,
	                                RespawnFadeOutDuration,
	                                false);
}

void AD2JPlayerCharacter::HandleRespawnFinished()
{
	SetCanBeDamaged(true);
	ToggleInput(true);
}

void AD2JPlayerCharacter::Respawn()
{
	SetActorLocation(SpawnLocation);
	GetMesh()->GetAnimInstance()->Montage_Stop(0.f, nullptr);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle,
	                                this,
	                                &AD2JPlayerCharacter::FinishRespawn,
	                                RespawnDuration,
	                                false);
}

void AD2JPlayerCharacter::HandleAnyDamageTaken(AActor* DamagedActor,
                                               float Damage,
                                               const UDamageType* DamageType,
                                               AController* InstigatedBy,
                                               AActor* DamageCauser)
{
	FailureCounter++;
	OnFailureCounterIncreased.Broadcast(FailureCounter);
	ToggleInput(false);
	SetCanBeDamaged(false);
}

void AD2JPlayerCharacter::HandleGameStarted()
{
	ToggleInput(true);
}

void AD2JPlayerCharacter::HandleGameStopped(const EGameInactivityReason InactivityReason)
{
	ToggleInput(false);
}

void AD2JPlayerCharacter::HandleGameFinished(const EGameResult Result)
{
	ToggleInput(false);
}
