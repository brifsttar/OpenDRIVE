// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OpenDriveAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, hidecategories = (Object))
class OPENDRIVE_API UOpenDriveAsset : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString XodrContent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "OpenDRIVE")
	int RoadCount;

	UPROPERTY(VisibleAnywhere, Instanced, Category = ImportSettings)
	class UAssetImportData *AssetImportData;

	void PostInitProperties() override;
};
