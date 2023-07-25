#pragma once

#include "CoreMinimal.h"    
#include "Components/ActorComponent.h"
#include "OpenDrivePosition.h"
#include "Components/SplineComponent.h"
#include "PedestrianCrossingComponent.generated.h"

USTRUCT(BlueprintType)
struct FSidewalksInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FVector position;

	UPROPERTY(BlueprintReadWrite)
	int laneId1;

	UPROPERTY(BlueprintReadWrite)
	int laneId2;
};

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
	 * Finds the sidewalk on the road's opposite side. 
	 * @param Odc The OpenDRIVEPosition
	 */
	UFUNCTION(BlueprintCallable, CallInEditor)
	void GetOppositeSidewalkPosition(UOpenDrivePosition* Odc, FSidewalksInfo& sidewalkInfo);

	/**
	 * Creates a simple linear trajectory between the starting point and the opposite sidewalk.
	 * @param Odc The OpenDRIVEPosition
	 * @param spline The actor's spline component to modify
	 */
	UFUNCTION(BlueprintCallable, CallInEditor)
	void CreateTrajectoryToOppositeSidewalk(UOpenDrivePosition* Odc, FSidewalksInfo& sidewalkInfo, USplineComponent* spline);
};
