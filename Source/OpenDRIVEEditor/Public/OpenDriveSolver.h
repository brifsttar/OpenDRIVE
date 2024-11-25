#pragma once
#include "CoreMinimal.h"
#include <RoadManager.hpp>
#include "OpenDriveSolver.generated.h"

/*
 * The UOpenDriveSolver class is designed to handle 
 * and manipulate road network data based on the OpenDRIVE format. 
 * This class provides various functionalities to work with roads, lanes, and their geometries.
 */
UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveSolver : public UObject
{
	GENERATED_BODY()

public:

	struct LaneId;

	struct LaneRef 
	{
		roadmanager::Road* Road;
		roadmanager::LaneSection* LaneSection;
		roadmanager::Lane* Lane;
	};

	struct LaneData 
	{
		roadmanager::Lane* LaneRef;
		roadmanager::Lane::LaneType LaneType;
		TArray<FTransform> A_Transform;
	};

	struct LaneSectionData 
	{
		roadmanager::LaneSection* SectionRef;
		TArray<LaneData> A_Lane;
	};

	struct RoadData 
	{
		roadmanager::Road* RoadRef;
		TArray<LaneSectionData> A_LaneSection;
		TArray<roadmanager::Geometry*> A_Geometry;
	};

	UOpenDriveSolver();
	UOpenDriveSolver(LaneRef currentLane, int lineType = roadmanager::Lane::LANE_TYPE_ANY);

	void Initialize(LaneRef currentLine, int lineType = roadmanager::Lane::LANE_TYPE_ANY);

	TArray<FTransform> MakeTransformArray(float step);
	FTransform MakeTransform(double offset);
	void MakeTransformArrayV2(RoadData& road);
	void UpdateTransform(roadmanager::Road* road, LaneSectionData* LSData, double offset, double length);

	void SetRoad(roadmanager::Road* road);
	void SetRoad(int id);

	void SetLane(roadmanager::Lane* Lane);
	void SetLane(int id);

	TArray<UOpenDriveSolver::LaneRef> GetAllLanesOfType(roadmanager::Lane::LaneType lineType = roadmanager::Lane::LANE_TYPE_ANY);
	TArray<UOpenDriveSolver::RoadData> getAllRoad(roadmanager::Lane::LaneType laneTypeMask = roadmanager::Lane::LANE_TYPE_ANY);
	TArray<FTransform> extractRoadTransform(TArray<RoadData>& A_road);

private:

	TArray<UOpenDriveSolver::LaneData> GetLane(roadmanager::Road* roadRef, roadmanager::LaneSection*, roadmanager::Lane::LaneType laneTypeMask = roadmanager::Lane::LANE_TYPE_ANY);
	TArray<UOpenDriveSolver::LaneSectionData> GetLaneSection(roadmanager::Road* roadRef, roadmanager::Lane::LaneType laneTypeMask = roadmanager::Lane::LANE_TYPE_ANY);
	TArray<roadmanager::Geometry*> GetGeometries(roadmanager::Road* roadRef);
	RoadData GetRoad(int roadId, roadmanager::Lane::LaneType laneTypeMask = roadmanager::Lane::LANE_TYPE_ANY);
	void FindPoint(double offset, roadmanager::Road* road = nullptr, roadmanager::Lane* lane = nullptr);

	LaneRef _currentLane;
	roadmanager::OpenDrive* _odr;
	double _laneLength;
	int _blockCount;
	float _blockSize;
	//Used to convert to world pos
	roadmanager::Position _position;
};
