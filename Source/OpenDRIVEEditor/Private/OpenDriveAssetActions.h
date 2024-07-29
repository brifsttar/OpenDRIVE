// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AssetTypeActions_Base.h"
#include "OpenDriveAsset.h"

class FOpenDRIVEAssetActions : public FAssetTypeActions_Base {
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return true; }
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;

	void ReimportOdrAsset(TArray<TWeakObjectPtr<UOpenDriveAsset>> OdrAssets);

	void ExportXODR(TArray<TWeakObjectPtr<UOpenDriveAsset>> OdrAssets);
};
