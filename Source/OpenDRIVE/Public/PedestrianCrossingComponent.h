#pragma once

#include "CoreMinimal.h"    
#include "Components/ActorComponent.h"
#include "OpenDrivePosition.h"
#include "Components/SplineComponent.h"
#include "PedestrianCrossingComponent.generated.h"

UENUM(BlueprintType)
enum TrajectoryType
{
	Drunk,
	Clumsy,
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
	 * @return The OpenDrivePosition UObject*
	 */
	inline UOpenDrivePosition* GetOpenDrivePosition() { return OpenDRIVEPosition; };

	/**
	 * Finds the sidewalk on the road's opposite side. 
	 */
	FVector GetOppositeSidewalkPosition();

	/**
	 * Creates a simple linear trajectory between the starting point and the opposite sidewalk.
	 * @param spline The actor's spline component to modify
	 */
	UFUNCTION(BlueprintCallable, CallInEditor)
	void CreateTrajectory(USplineComponent* spline);

	/**
	 * Alterates a the created trajectory
	 * @param spline The spline to modify 
	 * @param trajectoryType The trajectory type 
	 */
	UFUNCTION(BlueprintCallable, CallInEditor)
	void AlterateTrajectory(USplineComponent* spline, TrajectoryType trajectoryType);

protected:

	UOpenDrivePosition* OpenDRIVEPosition;
	
};
