#pragma once 
#include "ActorFactories/ActorFactory.h"
#include "OpenDriveActorFactory.generated.h"

UCLASS()
class UOpenDriveActorFactory : public UActorFactory
{
	GENERATED_BODY()

public : 

	UOpenDriveActorFactory(const FObjectInitializer& ObjectInitializer);

protected : 

	// UActorFactory Interface
	virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
	virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
	// UActorFactory end Interface 

};