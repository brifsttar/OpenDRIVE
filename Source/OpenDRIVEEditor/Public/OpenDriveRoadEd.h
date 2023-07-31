// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "RoadManager.hpp"
#include "CoordTranslate.h"
#include "OpenDriveRoadEd.generated.h"

UCLASS()
class OPENDRIVEEDITOR_API AOpenDriveRoadEd : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOpenDriveRoadEd();

	/**
	 * Initializes the lane parameters and draw it 
	 * @param roadId The road Id
	 * @param junctionId The junction Id
	 * @param laneSection The Roadmanager's lane section 
	 * @param lane The Roadmanager's lane
	 * @param offset The road offset
	 * @param step The step (the lower it is, the more precise it will be)  
	 */
	void Initialize(int roadId_, int junctionId_, int successorId_, int predecessorId_, roadmanager::LaneSection* laneSection_, roadmanager::Lane* lane_, float offset_, float step_);

	/**
	* Sets the laneType string to display in the editor mode widget
	*/
	void SetLaneType();

	/**
	 * Draws the lane 
	 * @param step The distance between each lane's points
	 * @param offset The road's Z's offset
	 */
	void DrawLane(double step, float offset);

	/**
	* Gets the road Id
	* @return The Road Id
	*/
	inline int GetRoadId() { return _roadId; };

	/**
	* Gets the junction Id
	* @return The Junction Id
	*/
	inline int GetJunctionId() { return _junctionId; };

	/**
	* Gets the lane type
	* @return The lane type 
	*/
	inline FString GetLaneType() { return _laneType; };

	/**
	* Gets the lane Id
	* @return The lane Id
	*/
	inline int GetLaneId() { return _lane->GetId(); };

	/**
	* @return The successor's road Id
	*/
	inline int GetSuccessorId() { return _successorId; };

	/**
	* @return The predecessor's road Id
	*/
	inline int GetPredecessorId() { return _predecessorId; };

	/**
	* Sets a lane's spline point
	* @param laneSpline_ The lane spline where the point is added 
	* @param position Reference to roadmanager::Position 
	* @param ds Distance to move along lane
	*/
	void SetLanePoint(USplineComponent* laneSpline_, roadmanager::Position& position, double s, float offset);

	/**
	* Checks the distance between the last spline point and his predecessor : if the distance is too short, we remove its predecessor.
	* @param laneSpline The Lane's spline
	* @param step The step used
	*/
	void CheckLastTwoPointsDistance(USplineComponent* laneSpline_, float step_);

	/**
	* Sets the arrow meshes along the lane's spline
	* @param laneSpline_ The lane's spline 
	*/
	void SetArrowMeshes(USplineComponent* laneSpline_, bool _isJunction);

	/**
	* Sets the colored spline meshes along the lane's spline
	* @param laneSpline_ The lane's spline 
	*/
	void SetColoredLaneMeshes(USplineComponent* laneSpline_);

	/**
    * Sets arrows visibility
    * @param _isVisible New visibility
    */
	void SetArrowVisibility(bool _isVisible);

private : 

	TArray<UStaticMeshComponent*> FArrowMeshes;

	TObjectPtr<UStaticMesh> _laneMeshPtr;
	TObjectPtr<UStaticMesh> _laneArrowMeshPtr;
	float _baseMeshSize;

	int _roadId = 0; 
	int _junctionId = 0;
	int _roadDirection = 0;
	int _predecessorId = 0;
	int _successorId = 0;
	FString _laneType = "Any";

	roadmanager::LaneSection* _laneSection;
	roadmanager::Lane* _lane;
};
