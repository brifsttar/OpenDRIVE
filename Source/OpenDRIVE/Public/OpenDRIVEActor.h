#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpenDriveAsset.h"
#include "OpenDRIVEActor.generated.h"

/*
* Actor that hold the OpenDriveAsset containing the.xodr file.
* Drag and drop one of these actors in your scene.
* You can only drop one in each scene : if you want to change the .xodr, just edit the OpenDriveAsset property.
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

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private : 

#if WITH_EDITOR
	/*
	* Called when actor is placed in editor (this is where we check if there's already one actor in the scene).
	**/
	virtual void PostEditMove(bool bFinished) override;
#endif
	void LoadOpenDrive();

	/*
	* Checks if there's more than one OpenDRIVEActor in the current scene
	**/
	bool CheckForMultipleOpenDRIVEActors();

	/*
	* Checks if there's more than one OpenDRIVEActor in the current scene
	**/
	bool CheckForMultipleOpenDRIVEActors(TArray<AActor*> &actors);
};
