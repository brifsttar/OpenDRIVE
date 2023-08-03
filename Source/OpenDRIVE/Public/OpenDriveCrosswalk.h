#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpenDriveComponent.h"
#include "Components/DecalComponent.h"
#include "OpenDriveCrosswalk.generated.h"

/**
* Drag and drop this actor in your scene to create a crosswalk
*/
UCLASS()
class OPENDRIVE_API AOpenDriveCrosswalk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOpenDriveCrosswalk();

	/**
	* @return roadId the roadId
	*/
	inline int GetRoadId() { return _roadId; };

	/**
	* Gets the crosswalk desired side 
	* @param laneId the lane id 
	* @return the side xyz position
	*/
	inline FVector* GetPositionOnSidewalkId(int laneId) { return TSides.Find(laneId); }

	UPROPERTY(EditDefaultsOnly, Meta = (Category = "Crosswalk"))
	UDecalComponent* UDecal;

protected:

	/**
	* Sets the crosswalk's sides' positions
	*/
	UFUNCTION(BlueprintCallable, CallInEditor, Meta = (Category = "Crosswalk"))
	void SetCrosswalkBorderPositions();

private:
	UOpenDriveComponent* _openDriveComponent;

	UPROPERTY(VisibleAnywhere, Meta = (Category = "Crosswalk"))
	int _roadId = 0;

	UPROPERTY(VisibleAnywhere, Meta = (Category = "Crosswalk"))
	FVector _side1;

	UPROPERTY(VisibleAnywhere, Meta = (Category = "Crosswalk"))
	FVector _side2;

	UPROPERTY(VisibleAnywhere, Meta = (Category = "Crosswalk"))
	TMap<int, FVector> TSides;
};
