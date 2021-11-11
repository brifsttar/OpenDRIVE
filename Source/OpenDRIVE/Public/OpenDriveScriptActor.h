// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
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

	UPROPERTY(EditAnywhere, Category = "Scenario", meta = (FilePathFilter = "xodr"))
	FFilePath OpenDriveFile;

	static roadmanager::OpenDrive *GetOpenDrive();

#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;

	virtual void CheckForErrors() override;
#endif

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

	void LoadOpenDrive();

public:
	
	virtual void PostLoad() override;
};
