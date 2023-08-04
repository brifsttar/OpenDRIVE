#pragma once

#include "CoreMinimal.h"    
#include "Components/ActorComponent.h"
#include "OpenDrivePosition.h"
#include "Components/SplineComponent.h"
#include "OpenDriveCrosswalk.h"
#include "PedestrianCrossingComponent.generated.h"


UENUM(BlueprintType)
	enum class ECrossingPathType : uint8
{
	SIMPLE UMETA(DisplayName = "Simple"),
	SIDEWALK UMETA(DisplayName = "Sidewalk"),
	STRAIGHT UMETA(DisplayName = "Straight")
};


/**
* Add this component on a pedestrian to allow him to cross roads by himself
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENDRIVE_API UPedestrianCrossingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/**
	 * Sets default values for this component's properties
	 */ 
	UPedestrianCrossingComponent();

	/**
	* @param Odp The OpenDRIVEPosition
	* @param startSOffset the S offset on the starting lane 
	* @param endSOffset the S offset on the target lane
	* @param bSimple the player go straight to the target or not
	* @return positions The path to follow
	*/
	UFUNCTION(BlueprintCallable)
	void CreatePathToOppositeSidewalk(UOpenDrivePosition* Odp, float endSOffset, ECrossingPathType pathType,TArray<FVector>& positions);

	/**
	* Add a new spline component following the given positions
	* @param positions the path to follow 
	* @param pathType the pathType 
	*/
	UFUNCTION(BlueprintCallable)
	USplineComponent* AddNewPathSpline(TArray<FVector> positions);

protected : 

	/**
	* Finds the nearest crosswalk to cross road 
	* @param roadID the roadId
	* @param currentLaneId the current laneId (where the pedestrian is)
	* @param targetlaneId the targeted laneId (where the pedestrian wants to go)
	* @param SearchAreaRadius the search area radius (in meter)
	* @return the crosswalkPath the path to follow 
	*/
	void FindNearCrosswalk(int roadID, int currentlaneId, int targetlaneId, float SearchAreaRadius, TArray<FVector>& crosswalkPath);
};
