// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "OpenDriveAsset.h"
#include "OpenDriveWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class OPENDRIVE_API AOpenDriveWorldSettings : public AWorldSettings {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "OpenDRIVE")
	UOpenDriveAsset* OpenDriveAsset;

#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;

	virtual void CheckForErrors() override;
#endif

protected:
	virtual void BeginPlay() override;
};
