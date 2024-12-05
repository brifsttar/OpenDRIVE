#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "RoadManager.hpp"
#include "OpenDriveEditorLane.generated.h"

UCLASS(Transient, Hidden, NotBlueprintable, NotBlueprintType, NotPlaceable)
class OPENDRIVEEDITOR_API AOpenDriveEditorLane : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOpenDriveEditorLane();

	/**
	 * Initializes the lane parameters and draw it 
	 * @param RoadIn The road Id
	 * @param LaneSectionIn The Roadmanager's lane section 
	 * @param LaneIn The Roadmanager's lane
	 * @param Offset The road offset
	 * @param Step The step (the lower it is, the more precise it will be)  
	 */
	void Initialize(roadmanager::Road* RoadIn, roadmanager::LaneSection* LaneSectionIn, roadmanager::Lane* LaneIn, float Offset, float Step);

	/**
	* Gets the road Id
	* @return The Road Id
	*/
	FORCEINLINE int GetRoadId() const { return Road->GetId(); };

	/**
	* Gets the junction Id
	* @return The Junction Id
	*/
	FORCEINLINE int GetJunctionId() const { return Road->GetJunction(); };

	/**
	* Gets the lane type
	* @return The lane type 
	*/
	FString GetLaneType() const;

	/**
	* Gets the lane Id
	* @return The lane Id
	*/
	FORCEINLINE int GetLaneId() const { return Lane->GetId(); };

	/**
	* @return The successor's road Id
	*/
	int GetSuccessorId() const;

	/**
	* @return The predecessor's road Id
	*/
	int GetPredecessorId() const;

	/**
    * Sets arrows visibility
    * @param bIsVisible New visibility
    */
	void SetArrowVisibility(bool bIsVisible);

protected : 

	/**
	 * Draws the lane
	 * @param Step The distance between each lane's points
	 * @param Offset The road's Z's offset
	 */
	void DrawLane(double Step, float Offset);

	/**
	* Sets a lane's spline point
	* @param LaneSpline The lane spline where the point is added
	* @param Position Reference to roadmanager::Position
	* @param S Distance to move along lane
	* @param Offset
	*/
	void SetLanePoint(USplineComponent* LaneSpline, roadmanager::Position& Position, double S, float Offset) const;

	/**
	* Checks the distance between the last spline point and his predecessor : if the distance is too short, we remove its predecessor.
	* @param LaneSpline The Lane's spline
	* @param Step The step used
	*/
	static void CheckLastTwoPointsDistance(USplineComponent* LaneSpline, float Step);

	/**
	* Sets the arrow meshes along the lane's spline
	* @param LaneSpline The lane's spline
	* @param Mesh The arrow mesh
	* @param bIsJunction True if it's a junction
	*/
	void SetArrowMeshes(const USplineComponent* LaneSpline, const TObjectPtr<UStaticMesh>& Mesh, bool bIsJunction);

	/**
	* Sets the colored spline meshes along the lane's spline
	* @param LaneSpline The lane's spline
	*/
	void SetColoredLaneMeshes(USplineComponent* LaneSpline);

private : 

	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> ArrowMeshes;

	UPROPERTY()
	TObjectPtr<UStaticMesh> LaneMeshPtr;

	UPROPERTY()
	float BaseMeshSize;

	UPROPERTY()
	int RoadDirection = 0;

	roadmanager::Road* Road;
	roadmanager::LaneSection* LaneSection;
	roadmanager::Lane* Lane;
};
