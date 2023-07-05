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

	void Initialize(int roadId_, int junctionId_, roadmanager::LaneSection* laneSection_, roadmanager::Lane* lane_);

	void DrawLanes(roadmanager::Road* road);

	void DrawLane();

	inline int GetRoadId() { return RoadId; };

	inline int GetJunctionId() { return JunctionId; };

	inline FString GetLaneType() { return LaneType; };

	inline int GetLaneId() { return Lane->GetId(); };

private : 

	TObjectPtr<UStaticMesh> LaneMesh;

	float roadOffset = 50.f;

	float baseMeshSize;

	int RoadId = 0; 
	int JunctionId = 0;
	FString LaneType = "Any";

	roadmanager::LaneSection* LaneSection;
	roadmanager::Lane* Lane;
};
