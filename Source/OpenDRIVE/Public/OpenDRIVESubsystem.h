#pragma once	

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OpenDriveAsset.h"
#include "OpenDRIVESubsystem.generated.h"

/**
* 
*/
UCLASS()
class OPENDRIVE_API UOpenDRIVESubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public :

	void LoadOpenDrive(UOpenDriveAsset* newOpenDriveAsset);

protected : 

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

};
