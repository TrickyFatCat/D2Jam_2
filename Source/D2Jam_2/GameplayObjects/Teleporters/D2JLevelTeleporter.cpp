// Fill out your copyright notice in the Description page of Project Settings.


#include "D2JLevelTeleporter.h"

#include "TrickyGameModeBase.h"
#include "Kismet/GameplayStatics.h"


AD2JLevelTeleporter::AD2JLevelTeleporter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AD2JLevelTeleporter::HandleStateChanged(UGameplayObjectStateControllerComponent* Component,
                                             EGameplayObjectState NewState,
                                             bool bChangedImmediately)
{
	Super::HandleStateChanged(Component, NewState, bChangedImmediately);

	if (NewState == EGameplayObjectState::Active)
	{
		if (LevelToLoad.GetAssetName() != "")
		{
			FLatentActionInfo LatentActionInfo;
			UGameplayStatics::LoadStreamLevelBySoftObjectPtr(this,
			                                                 SublevelToActivate,
			                                                 true,
			                                                 true,
			                                                 LatentActionInfo);
		}
	}
}

void AD2JLevelTeleporter::HandleTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                    AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp,
                                                    int32 OtherBodyIndex,
                                                    bool bFromSweep,
                                                    const FHitResult& SweepResult)
{
	Super::HandleTriggerBeginOverlap(OverlappedComponent,
	                                 OtherActor,
	                                 OtherComp,
	                                 OtherBodyIndex,
	                                 bFromSweep,
	                                 SweepResult);

	ATrickyGameModeBase* GameMode = Cast<ATrickyGameModeBase>(GetWorld()->GetAuthGameMode());
	GameMode->Execute_FinishGame(GameMode, EGameResult::Win);
}

void AD2JLevelTeleporter::HandleCameraFadeInFinished()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, LevelToLoad, true);
}
