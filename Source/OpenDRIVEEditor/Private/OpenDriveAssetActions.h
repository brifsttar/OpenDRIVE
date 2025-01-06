#pragma once

#include "AssetTypeActions_Base.h"
#include "OpenDriveAsset.h"
#include "IOpenDriveModuleInterface.h"

class FOpenDriveAssetActions : public FAssetTypeActions_Base, public IOpenDriveModuleListenerInterface
{
public :

	/* IOpenDriveModuleListenerInterface */
	virtual void OnStartupModule() override;
	virtual void OnShutdownModule() override;

	/* AssetsTypeActions interface */
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return true; }
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;

	/* OpenDrive's actions */
	void ReimportOdrAsset(TArray<TWeakObjectPtr<UOpenDriveAsset>> OdrAssets);
};
