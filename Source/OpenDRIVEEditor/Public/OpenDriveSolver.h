// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <RoadManager.hpp>
#include <CoordTranslate.h>
#include "OpenDriveSolver.generated.h"

/**
 * 
 */
UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveSolver : public UObject
{
	GENERATED_BODY()

public:

	struct LaneId;

	struct LaneRef {
		roadmanager::Road* road;
		roadmanager::LaneSection* laneSection;
		roadmanager::Lane* lane;
	};

	struct LaneData {
		roadmanager::Lane* laneRef;
		roadmanager::Lane::LaneType laneType;
	};

	struct LaneSectionData {
		roadmanager::LaneSection* sectionRef;
		TArray<LaneData> A_lane;
	};

	struct RoadData {
		roadmanager::Road* roadRef;
		TArray<LaneSectionData> A_laneSection;
		TArray<roadmanager::Geometry*> A_geometry;
	};


	UOpenDriveSolver();
	UOpenDriveSolver(LaneRef currentLane, int lineType = roadmanager::Lane::LANE_TYPE_ANY);


	void Initialize(LaneRef currentLine, int lineType = roadmanager::Lane::LANE_TYPE_ANY);

	TArray<FTransform> MakeTransformArray(float step);

	FTransform MakeTransform(double offset);

	TArray<FTransform> MakeTransformArrayV2(RoadData road);


	void SetRoad(roadmanager::Road* road);
	void SetRoad(int id);

	void SetLane(roadmanager::Lane* Lane);
	void SetLane(int id);

	

	TArray<UOpenDriveSolver::LaneRef> GetAllLanesOfType(roadmanager::Lane::LaneType lineType = roadmanager::Lane::LANE_TYPE_ANY);


	UOpenDriveSolver::RoadData getRoad(int roadId);
	TArray<UOpenDriveSolver::RoadData> getAllRoad();

private:

	roadmanager::OpenDrive* _odr;

	void FindPoint(double offset);

	LaneRef _currentLane;


	double _laneLength;
	int _blockCount;
	float _blockSize;
	//Used to convert to world pos
	roadmanager::Position _position;
};
