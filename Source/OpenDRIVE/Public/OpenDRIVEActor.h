#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpenDriveAsset.h"
#include "Engine/ViewportStatsSubsystem.h"
#include "OpenDRIVEActor.generated.h"

/*
* Actor that hold the OpenDriveAsset containing the.xodr file.
* Drag and drop one of these actors in your scene.
* You can only drop one in each scene.
**/
UCLASS()
class OPENDRIVE_API AOpenDRIVEActor : public AActor
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	AOpenDRIVEActor();

	UPROPERTY(EditAnywhere, Category = "OpenDRIVE")
	UOpenDriveAsset* OpenDriveAsset;

	virtual void PostLoad() override;

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void BeginDestroy() override;

	virtual void OnConstruction(const FTransform& Transform) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private : 

	/**
	* Loads the OpenDRIVE's file
	*/
	void LoadOpenDrive();

	/*
	* Checks if there's more than one OpenDRIVEActor in the current scene
	**/
	bool CheckForMultipleOpenDRIVEActors(TArray<AActor*>& actors);

	/*
	* Checks if there's more than one OpenDRIVEActor in the current scene
	**/
	bool CheckForMultipleOpenDRIVEActors();

	void OpenDRIVEError(bool bError);
	int32 _displayDelegateIdx;
};
