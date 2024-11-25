#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "RoadManager.hpp"
#include "OpenDriveEditorLane.generated.h"

UCLASS(Transient)
class OPENDRIVEEDITOR_API AOpenDriveEditorLane : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOpenDriveEditorLane();

	/**
	 * Initializes the lane parameters and draw it 
	 * @param road_ The road Id
	 * @param laneSection_ The Roadmanager's lane section 
	 * @param lane_ The Roadmanager's lane
	 * @param offset_ The road offset
	 * @param step_ The step (the lower it is, the more precise it will be)  
	 */
	void Initialize(roadmanager::Road* road_, roadmanager::LaneSection* laneSection_, roadmanager::Lane* lane_, float offset_, float step_);

	/**
	* Gets the road Id
	* @return The Road Id
	*/
	FORCEINLINE int GetRoadId() const { return _road->GetId(); };

	/**
	* Gets the junction Id
	* @return The Junction Id
	*/
	FORCEINLINE int GetJunctionId() const { return _road->GetJunction(); };

	/**
	* Gets the lane type
	* @return The lane type 
	*/
	FString GetLaneType() const;

	/**
	* Gets the lane Id
	* @return The lane Id
	*/
	FORCEINLINE int GetLaneId() const { return _lane->GetId(); };

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
    * @param _isVisible New visibility
    */
	void SetArrowVisibility(bool _isVisible);

protected : 

	/**
	 * Draws the lane
	 * @param step The distance between each lane's points
	 * @param offset The road's Z's offset
	 */
	void DrawLane(double step, float offset);

	/**
	* Sets a lane's spline point
	* @param laneSpline_ The lane spline where the point is added
	* @param position Reference to roadmanager::Position
	* @param s Distance to move along lane
	* @param offset
	*/
	void SetLanePoint(USplineComponent* laneSpline_, roadmanager::Position& position, double s, float offset);

	/**
	* Checks the distance between the last spline point and his predecessor : if the distance is too short, we remove its predecessor.
	* @param laneSpline_ The Lane's spline
	* @param step_ The step used
	*/
	void CheckLastTwoPointsDistance(USplineComponent* laneSpline_, float step_);

	/**
	* Sets the arrow meshes along the lane's spline
	* @param laneSpline_ The lane's spline
	* @param mesh The arrow mesh
	* @param isJunction True if it's a junction
	*/
	void SetArrowMeshes(USplineComponent* laneSpline_, TObjectPtr<UStaticMesh> mesh, bool isJunction);

	/**
	* Sets the colored spline meshes along the lane's spline
	* @param laneSpline_ The lane's spline
	*/
	void SetColoredLaneMeshes(USplineComponent* laneSpline_);

private : 

	UPROPERTY()
	TArray<UStaticMeshComponent*> _arrowMeshes;

	UPROPERTY()
	TObjectPtr<UStaticMesh> _laneMeshPtr;

	UPROPERTY()
	float _baseMeshSize;

	UPROPERTY()
	int _roadDirection = 0;

	roadmanager::Road* _road;
	roadmanager::LaneSection* _laneSection;
	roadmanager::Lane* _lane;
};
