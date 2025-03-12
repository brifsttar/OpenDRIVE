#include "OpenDriveSolver.h"
#include "CoordTranslate.h"

UOpenDriveSolver::UOpenDriveSolver()
	: _odr(roadmanager::Position::GetOpenDrive()),
	_currentLane{ 0, 0, 0 },
	_position(), 
	_blockCount(0),
	_blockSize(0.f)
{
	_position.SetLanePos(0, 0, 0, 0.);
	_position.SetSnapLaneTypes(roadmanager::Lane::LANE_TYPE_ANY);
}

UOpenDriveSolver::UOpenDriveSolver(LaneRef currentLane, int lineType)
	: _odr(roadmanager::Position::GetOpenDrive()),
	_currentLane(currentLane),
	_position(),
	_blockCount(0),
	_blockSize(0.f)
{
	_position.SetLanePos(0, 0, 0, 0.);
	Initialize(currentLane, lineType);
}

void UOpenDriveSolver::Initialize(LaneRef currentLane, int lineType) {
	_currentLane = currentLane;
	_position.SetSnapLaneTypes(lineType);
	_laneLength = _currentLane.LaneSection->GetLength();
}

TArray<FTransform> UOpenDriveSolver::MakeTransformArray(float step) {
	if (_blockCount == 0 || FMath::Abs(step - _blockSize) > KINDA_SMALL_NUMBER) {
		_blockCount = FMath::Max(1, static_cast<int>(_laneLength / step));
		_blockSize = _laneLength / static_cast<float>(_blockCount);
	}

	double s = _currentLane.LaneSection->GetS();
	TArray<FTransform> TList;

	for (int i = 0; i < _blockCount; i++) {
		TList.Add(MakeTransform(_blockSize / 2 + i * _blockSize + s));
	}

	return TList;
}

FTransform UOpenDriveSolver::MakeTransform(double offset) {
	FindPoint(offset);
	FVector sp = CoordTranslate::OdrToUe::ToLocation(_position);
	FRotator rot = CoordTranslate::OdrToUe::ToRotation(_position);
	FVector sc = FVector(_blockSize, 1, 10);
	return FTransform(rot, sp, sc);
}

void UOpenDriveSolver::MakeTransformArrayV2(RoadData& road) {
	int lsIndex = 0, geoIndex = 0;
	double SNow = 0;

	while (SNow <= road.RoadRef->GetLength() && geoIndex < road.A_Geometry.Num() && lsIndex < road.A_LaneSection.Num()) {
		double geoEndS = road.A_Geometry[geoIndex]->GetS() + road.A_Geometry[geoIndex]->GetLength();
		double laneEndS = road.A_LaneSection[lsIndex].SectionRef->GetS() + road.A_LaneSection[lsIndex].SectionRef->GetLength();

		if (geoEndS < laneEndS) {
			if (!road.A_LaneSection[lsIndex].SectionRef) {
				UE_LOG(LogTemp, Error, TEXT("SectionRef est NULL dans MakeTransformArrayV2 !"));
				continue;
			}
			UpdateTransform(road.RoadRef, &(road.A_LaneSection[lsIndex]), SNow, geoEndS - SNow);
			SNow = geoEndS;
			geoIndex++;
		}
		else {
			if (!road.A_LaneSection[lsIndex].SectionRef) {
				UE_LOG(LogTemp, Error, TEXT("SectionRef est NULL dans MakeTransformArrayV2 !"));
				continue;
			}
			UpdateTransform(road.RoadRef, &(road.A_LaneSection[lsIndex]), SNow, laneEndS - SNow);
			SNow = laneEndS;
			lsIndex++;
		}
	}
}


void UOpenDriveSolver::UpdateTransform(roadmanager::Road* road, LaneSectionData* LSData, double offset, double length) {
	float stepSize = 2.0f;
	int numSteps = FMath::Max(1, static_cast<int>(length / stepSize));

	for (int i = 0; i < LSData->A_Lane.Num(); i++) {
		for (int step = 0; step <= numSteps; step++) {
			double currentOffset = offset + step * stepSize;

			FindPoint(currentOffset, road, LSData->A_Lane[i].LaneRef);
			FVector sp = CoordTranslate::OdrToUe::ToLocation(_position);
			FRotator rot = CoordTranslate::OdrToUe::ToRotation(_position);
			FVector sc = FVector(length, LSData->SectionRef->GetWidth(offset, LSData->A_Lane[i].LaneRef->GetId()), 1) / 2;

			LSData->A_Lane[i].A_Transform.Add(FTransform(rot, sp, sc));
		}
	}
}

void UOpenDriveSolver::SetRoad(roadmanager::Road* road) {
	_currentLane.Road = road;
}

void UOpenDriveSolver::SetRoad(int id) {
	_currentLane.Road = _odr->GetRoadByIdx(id);
}

void UOpenDriveSolver::SetLane(roadmanager::Lane* lane) {
	_currentLane.Lane = lane;
}

void UOpenDriveSolver::SetLane(int id) {
	_currentLane.Lane = _currentLane.LaneSection->GetLaneByIdx(id);
}

TArray<UOpenDriveSolver::LaneRef> UOpenDriveSolver::GetAllLanesOfType(roadmanager::Lane::LaneType lineType) {
	TArray<LaneRef> laneList;
	int nbRoad = _odr->GetNumOfRoads();

	for (int i = 0; i < nbRoad; i++) {
		if (roadmanager::Road* road = _odr->GetRoadByIdx(i)) {
			for (int j = 0, nbLaneSection = road->GetNumberOfLaneSections(); j < nbLaneSection; j++) {
				if (roadmanager::LaneSection* laneSection = road->GetLaneSectionByIdx(j)) {
					for (int k = 0, nbLane = laneSection->GetNumberOfLanes(); k < nbLane; k++) {
						if (roadmanager::Lane* lane = laneSection->GetLaneByIdx(k)) {
							if (lane->GetId() != 0 && (lane->GetLaneType() == lineType || lineType == roadmanager::Lane::LANE_TYPE_ANY)) {
								laneList.Emplace(road, laneSection, lane);
							}
						}
					}
				}
			}
		}
	}

	return laneList;
}

TArray<UOpenDriveSolver::LaneData> UOpenDriveSolver::GetLane(roadmanager::Road* roadRef, roadmanager::LaneSection* sectionRef, roadmanager::Lane::LaneType laneTypeMask) {
	TArray<LaneData> A_lane;
	const int nbLane = sectionRef->GetNumberOfLanes();

	for (int i = 0; i < nbLane; i++) {
		roadmanager::Lane* lane = sectionRef->GetLaneByIdx(i);
		if (lane && lane->GetId() != 0 && (laneTypeMask & lane->GetLaneType())) {
			A_lane.Add(LaneData{ lane, lane->GetLaneType() });
		}
	}
	return A_lane;
}

TArray<UOpenDriveSolver::LaneSectionData> UOpenDriveSolver::GetLaneSection(roadmanager::Road* roadRef, roadmanager::Lane::LaneType laneTypeMask) {
	TArray<LaneSectionData> A_ls;
	const int nbLaneSection = roadRef->GetNumberOfLaneSections();

	for (int i = 0; i < nbLaneSection; i++) {
		roadmanager::LaneSection* section = roadRef->GetLaneSectionByIdx(i);
		if (section) {
			A_ls.Add(LaneSectionData{ section, GetLane(roadRef, section, laneTypeMask) });
		}
	}
	return A_ls;
}

TArray<roadmanager::Geometry*> UOpenDriveSolver::GetGeometries(roadmanager::Road* roadRef) {
	TArray<roadmanager::Geometry*> A_geo;
	const int nbGeometry = roadRef->GetNumberOfGeometries();

	for (int i = 0; i < nbGeometry; i++) {
		roadmanager::Geometry* geo = roadRef->GetGeometry(i);
		if (geo) {
			A_geo.Add(geo);
		}
	}
	return A_geo;
}


UOpenDriveSolver::RoadData UOpenDriveSolver::GetRoad(int roadId, roadmanager::Lane::LaneType laneTypeMask) {
	RoadData road;
	road.RoadRef = _odr->GetRoadByIdx(roadId);
	if (!road.RoadRef) return road;

	road.A_LaneSection = GetLaneSection(road.RoadRef, laneTypeMask);
	road.A_Geometry = GetGeometries(road.RoadRef);

	MakeTransformArrayV2(road);

	return road;
}


TArray<UOpenDriveSolver::RoadData> UOpenDriveSolver::getAllRoad(roadmanager::Lane::LaneType laneTypeMask) {
	TArray<RoadData> A_road;
	RoadData road;

	int nbRoad = _odr->GetNumOfRoads();
	for (int i = 0; i < nbRoad; i++) {
		road = GetRoad(i, laneTypeMask);
		if (!road.RoadRef) continue;
		A_road.Add(road);
	}

	return A_road;
}

TArray<FTransform> UOpenDriveSolver::extractRoadTransform(TArray<RoadData>& A_road) {
	TArray<FTransform> A_transform;
	for (RoadData road : A_road) {
		for (LaneSectionData ls : road.A_LaneSection) {
			for (LaneData lane : ls.A_Lane) {
				A_transform.Append(lane.A_Transform);
			}
		}
	}
	return A_transform;
}

void UOpenDriveSolver::FindPoint(double offset, roadmanager::Road* road, roadmanager::Lane* lane) {
	if (lane == nullptr || road == nullptr) {
		_position.SetLanePos(_currentLane.Road->GetId(), _currentLane.Lane->GetId(), offset, 0.);
	}
	else {
		_position.SetLanePos(road->GetId(), lane->GetId(), offset, 0.);
	}
}

int32 UOpenDriveSolver::GetLaneIDFromLane(const roadmanager::Lane* Lane) {
	if (!Lane) {
		UE_LOG(LogTemp, Warning, TEXT("Lane est null, ID non récupéré"));
		return -100;
	}

	return Lane->GetId();
}
