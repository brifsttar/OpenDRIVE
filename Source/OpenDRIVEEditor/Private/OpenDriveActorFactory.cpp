#include "OpenDriveActorFactory.h"
#include "AssetRegistry/AssetData.h"
#include "OpenDRIVEActor.h"
#include "OpenDriveAsset.h"

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
		NewActor->SetActorLabel("OpenDRIVEActor");
		AOpenDRIVEActor* openDriveActor = CastChecked<AOpenDRIVEActor>(NewActor);
		openDriveActor->OpenDriveAsset = openDriveAsset_;
	}
}

bool UOpenDriveActorFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	return(AssetData.IsValid() && AssetData.GetClass()->IsChildOf(UOpenDriveAsset::StaticClass()));
}
