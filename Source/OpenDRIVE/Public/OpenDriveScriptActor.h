// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "OpenDriveAsset.h"
#include "OpenDriveScriptActor.generated.h"

/**
 * 
 */
UCLASS(/*Deprecated*/)
class OPENDRIVE_API AOpenDriveScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	UPROPERTY(/*EditAnywhere, Category = "OpenDRIVE"*/)
	UOpenDriveAsset *OpenDriveAsset;

	UPROPERTY(/*VisibleAnywhere, BlueprintReadOnly, Category = "OpenDRIVE"*/)
	FFilePath OpenDriveFile;

protected:
	virtual void BeginPlay() override;

	void LoadOpenDrive();

public:
	
	virtual void PostLoad() override;
};
