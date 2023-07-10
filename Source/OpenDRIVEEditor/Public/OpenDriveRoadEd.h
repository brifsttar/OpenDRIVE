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
	 */
	void Initialize(int roadId_, int junctionId_, roadmanager::LaneSection* laneSection_, roadmanager::Lane* lane_, float offset_);

	/**
	 * Used to draw the lane 
	 * @param perfMode Performance mode (True for enabling it, False for disabling it)
	 */
	void DrawLane(bool perfMode_);

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

private : 

	TObjectPtr<UStaticMesh> _laneMeshPtr;
	float _baseMeshSize;
	float _roadOffset = 10.f;

	int _roadId = 0; 
	int _junctionId = 0;
	FString _laneType = "Any";

	roadmanager::LaneSection* _laneSection;
	roadmanager::Lane* _lane;
};
