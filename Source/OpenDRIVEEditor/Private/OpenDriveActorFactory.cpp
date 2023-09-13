#include "OpenDriveActorFactory.h"
#include "AssetRegistry/AssetData.h"
#include "OpenDRIVE/Public/OpenDRIVEActor.h"
#include "OpenDRIVE/Public/OpenDriveAsset.h"

UOpenDriveActorFactory::UOpenDriveActorFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = NSLOCTEXT("OpenDriveAsset", "OpenDriveAssetFactoryDisplayName", "Add OpenDriveActor");
	NewActorClass = AOpenDRIVEActor::StaticClass();
}

void UOpenDriveActorFactory::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);

	if (UOpenDriveAsset* openDriveAsset_ = Cast<UOpenDriveAsset>(Asset))
	{
		AOpenDRIVEActor* openDriveActor = CastChecked<AOpenDRIVEActor>(NewActor);
		openDriveActor->OpenDriveAsset = openDriveAsset_;
		openDriveActor->LoadOpenDrive();
	}
}

void UOpenDriveActorFactory::PostCreateBlueprint(UObject* Asset, AActor* CDO)
{
	if (Asset != nullptr && CDO != nullptr)
	{
		UOpenDriveAsset* openDriveAsset_ = CastChecked<UOpenDriveAsset>(Asset);
		AOpenDRIVEActor* openDriveActor = CastChecked<AOpenDRIVEActor>(CDO);
		openDriveActor->OpenDriveAsset = openDriveAsset_;
		openDriveActor->LoadOpenDrive();
	}
}

bool UOpenDriveActorFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	return(AssetData.IsValid() && AssetData.GetClass()->IsChildOf(UOpenDriveAsset::StaticClass()));
}
