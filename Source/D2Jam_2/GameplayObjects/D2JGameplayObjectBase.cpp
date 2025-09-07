// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JGameplayObjectBase.h"

#include "GameplayObject/GameplayObjectStateControllerComponent.h"


AD2JGameplayObjectBase::AD2JGameplayObjectBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	StateControllerComponent = CreateDefaultSubobject<UGameplayObjectStateControllerComponent>(TEXT("StateController"));
}

#if WITH_EDITOR
void AD2JGameplayObjectBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	const EGameplayObjectState InitialState = bIsActiveOnStart
												  ? EGameplayObjectState::Active
												  : EGameplayObjectState::Inactive;
	StateControllerComponent->SetInitialState(InitialState);
}
#endif

void AD2JGameplayObjectBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

bool AD2JGameplayObjectBase::ActivateGameplayObject_Implementation(const bool bTransitImmediately)
{
	return Execute_ActivateGameplayObject(StateControllerComponent, bTransitImmediately);
}

bool AD2JGameplayObjectBase::DeactivateGameplayObject_Implementation(const bool bTransitImmediately)
{
	return Execute_DeactivateGameplayObject(StateControllerComponent, bTransitImmediately);
}
