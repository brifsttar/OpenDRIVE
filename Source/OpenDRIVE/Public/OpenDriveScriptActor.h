// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "OpenDriveAsset.h"
#include "RoadManager.hpp"
#include "OpenDriveScriptActor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(OpenDriveLog, Log, All);

/**
 * 
 */
UCLASS()
class OPENDRIVE_API AOpenDriveScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	AOpenDriveScriptActor();

	UPROPERTY(EditAnywhere, Category = "Scenario")
	UOpenDriveAsset *OpenDriveAsset;

	UPROPERTY(/*VisibleAnywhere, BlueprintReadOnly, Category = "Scenario"*/)
	FFilePath OpenDriveFile;

#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;

	virtual void CheckForErrors() override;
#endif

protected:
	virtual void BeginPlay() override;

	void LoadOpenDrive();

public:
	
	virtual void PostLoad() override;
};
