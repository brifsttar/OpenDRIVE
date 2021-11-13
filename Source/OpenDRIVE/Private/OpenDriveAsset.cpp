// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDriveAsset.h"
#include "EditorFramework/AssetImportData.h"

void UOpenDriveAsset::PostInitProperties() {
	if (!HasAnyFlags(RF_ClassDefaultObject)) {
		AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	}
	Super::PostInitProperties();
}
