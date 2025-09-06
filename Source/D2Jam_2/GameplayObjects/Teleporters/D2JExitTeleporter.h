// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "D2JTeleporterBase.h"
#include "D2JExitTeleporter.generated.h"

UCLASS()
class D2JAM_2_API AD2JExitTeleporter : public AD2JTeleporterBase
{
	GENERATED_BODY()

public:
	AD2JExitTeleporter();

protected:
	virtual void HandleCameraFadeInFinished() override;
};
