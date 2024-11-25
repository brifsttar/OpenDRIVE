#include "OpenDriveSolver.h"
#include "CoordTranslate.h"

UOpenDriveSolver::UOpenDriveSolver(){

	_odr = roadmanager::Position::GetOpenDrive();

	_currentLane = { 0,0,0 };
	_position.SetLanePos(0, 0, 0, 0.);
	_position.SetSnapLaneTypes(roadmanager::Lane::LANE_TYPE_ANY);
}

UOpenDriveSolver::UOpenDriveSolver(LaneRef currentLane, int lineType){
	_odr = roadmanager::Position::GetOpenDrive();

	_position.SetLanePos(0, 0, 0, 0.);
	Initialize(currentLane, lineType);
}
 
void UOpenDriveSolver::Initialize(LaneRef currentLane, int lineType){
	_currentLane = currentLane;
	_position.SetSnapLaneTypes(lineType);
	_laneLength = _currentLane.LaneSection->GetLength();
}

/**
* Will Extract an array of transform from the requested line
* @param step - the size of a step in meter
* @return The array of the transform composing the line
*/
TArray<FTransform> UOpenDriveSolver::MakeTransformArray(float step){
	_blockCount = (int)(_laneLength / step) + 1;
	_blockSize = _laneLength / (float)_blockCount;

	//get the starting point of the lane
	double s = _currentLane.LaneSection->GetS();

	double middleOfLane = _currentLane.LaneSection->GetLength() / 2 + s; //TODO CHECK LINE TYPE

	TArray<FTransform> TList;

	for (int i = 0; i < _blockCount; i++) {
		TList.Add(MakeTransform(_blockSize / 2 + i * _blockSize + s));
	}

	return TList;
}

/**
* Will extract a transform from the opendrive at the requested offset
* @param offset - The offset (S value in opendrive) from the begining of the line to read the point at
* @return The transform at the point
*/
FTransform UOpenDriveSolver::MakeTransform(double offset){
	FindPoint(offset);
	FVector sp = CoordTranslate::OdrToUe::ToLocation(_position);
	FRotator rot = CoordTranslate::OdrToUe::ToRotation(_position);
	FVector sc = FVector(_blockSize, 1, 10);

	FTransform T(rot, sp, sc);

	return T;
}

void UOpenDriveSolver::MakeTransformArrayV2(RoadData& road){

	//TODO HANDLE ARC GEOMETRY
	int lsIndex = 0, geoIndex = 0, SNow = 0;

	while (SNow <= road.RoadRef->GetLength() && geoIndex < road.A_Geometry.Num() && lsIndex< road.A_LaneSection.Num()) {
		if (road.A_Geometry[geoIndex]->GetS() + road.A_Geometry[geoIndex]->GetLength() <
		road.A_LaneSection[lsIndex].SectionRef->GetS() + road.A_LaneSection[lsIndex].SectionRef->GetLength()) {
			//remaining geo is shorter
			UpdateTransform(road.RoadRef, &(road.A_LaneSection[lsIndex]), SNow, road.A_Geometry[geoIndex]->GetS() + road.A_Geometry[geoIndex]->GetLength() - SNow);
			SNow = road.A_Geometry[geoIndex]->GetS() + road.A_Geometry[geoIndex]->GetLength();
			geoIndex++;
		}else {
			//remaining lanesection  is shorter
			UpdateTransform(road.RoadRef, &(road.A_LaneSection[lsIndex]), SNow, road.A_LaneSection[lsIndex].SectionRef->GetS() + road.A_LaneSection[lsIndex].SectionRef->GetLength() - SNow);
			SNow = road.A_LaneSection[lsIndex].SectionRef->GetS() + road.A_LaneSection[lsIndex].SectionRef->GetLength();
			lsIndex++;
		}
	}
}

void UOpenDriveSolver::UpdateTransform(roadmanager::Road* road,  LaneSectionData* LSData, double offset, double length){
	float middleOfSection = offset + length / 2;
	for (int i = 0; i < LSData->A_Lane.Num(); i++) {
		FindPoint(middleOfSection, road, LSData->A_Lane[i].LaneRef);
		FVector sp = CoordTranslate::OdrToUe::ToLocation(_position);
		FRotator rot = CoordTranslate::OdrToUe::ToRotation(_position);
		FVector sc = FVector(
			length,																		//LENGHT
			LSData->SectionRef->GetWidth(offset, LSData->A_Lane[i].LaneRef->GetId()),		//WIDTH
			1)/2;																			//HEIGHT
		LSData->A_Lane[i].A_Transform.Add(FTransform(rot, sp, sc));
	}
}

void UOpenDriveSolver::SetRoad(roadmanager::Road* road){
	_currentLane.Road = road;
}

void UOpenDriveSolver::SetRoad(int id){
	_currentLane.Road = _odr->GetRoadByIdx(id);
}

void UOpenDriveSolver::SetLane(roadmanager::Lane* lane){
	_currentLane.Lane = lane;
}

void UOpenDriveSolver::SetLane(int id){
	_currentLane.Lane = _currentLane.LaneSection->GetLaneByIdx(id);
}

/**
* Used to exctract all the Lane of one type from the opendrive
* @param lineType - requested linetype to get, will return all the line type if left empty
* @return A List of laneRef
*/
TArray<UOpenDriveSolver::LaneRef> UOpenDriveSolver::GetAllLanesOfType(roadmanager::Lane::LaneType lineType){
	roadmanager::Road* road = 0;
	roadmanager::LaneSection* laneSection = 0;
	roadmanager::Lane* lane = 0;
	int nbRoad = 0;
	int nbLaneSection = 0;
	int nbLane = 0;
	TArray<LaneRef> laneList;

	//We iterate over all the existing road section								//FOR EACH
	nbRoad = _odr->GetNumOfRoads();												//ROAD
	for (int i = 0; i < nbRoad; i++) {
		road = _odr->GetRoadByIdx(i);
		if (!road) continue;

		nbLaneSection = road->GetNumberOfLaneSections();						//LANE SECTION
		for (int j = 0; j < nbLaneSection; j++) {
			laneSection = road->GetLaneSectionByIdx(j);
			if (!laneSection) continue;

			nbLane = laneSection->GetNumberOfLanes();							//LANE
			for (int k = 0; k < nbLane; k++) {
				lane = laneSection->GetLaneByIdx(k);

				//Get the lane if the type match
				if (!lane || lane->GetId() == 0) continue;
				if (lane->GetLaneType() == lineType || lineType == roadmanager::Lane::LANE_TYPE_ANY) {
					laneList.Add(LaneRef{ road, laneSection, lane });
				}
			}
		}
	}


	return laneList;
}

/**
* Used to extract all the lane of a section
* @param 
*
*/
TArray<UOpenDriveSolver::LaneData> UOpenDriveSolver::GetLane(roadmanager::Road* roadRef, roadmanager::LaneSection* sectionRef, roadmanager::Lane::LaneType laneTypeMask){
	TArray<LaneData> A_lane;
	roadmanager::Lane* lane = 0;
	int nbLane = sectionRef->GetNumberOfLanes();
	for (int i = 0; i < nbLane; i++) {
		lane = sectionRef->GetLaneByIdx(i);
		if (!lane || lane->GetId() == 0) continue;
		if (!(laneTypeMask & lane->GetLaneType())) continue;	//filter linetype
		LaneData ld = LaneData{ lane, lane->GetLaneType() };
		A_lane.Add(ld);
	}
	return A_lane;
}

/**
* Will get all the lane section of the road
* @param roadRef - reference of the road
* @param laneTypeMask - bitMask for the type of lane to extract from the file (all of them if left empty)
*/
TArray<UOpenDriveSolver::LaneSectionData> UOpenDriveSolver::GetLaneSection(roadmanager::Road* roadRef, roadmanager::Lane::LaneType laneTypeMask){
	TArray<LaneSectionData> A_ls;
	roadmanager::LaneSection* section;
	int nbLaneSection = roadRef->GetNumberOfLaneSections();
	for (int i = 0; i < nbLaneSection; i++) {
		section = roadRef->GetLaneSectionByIdx(i);
		if (!section) continue;
		A_ls.Add(LaneSectionData{ section, GetLane(roadRef, section, laneTypeMask) });
	}
	return A_ls;
}

/**
* Will get the geometry of the road from the opendrive
* @param roadRef - referencce to the road
* @return An array of geometry
*/
TArray<roadmanager::Geometry*> UOpenDriveSolver::GetGeometries(roadmanager::Road* roadRef){
	TArray<roadmanager::Geometry*> A_geo;
	roadmanager::Geometry* geo = 0;

	int nbGeometry = roadRef->GetNumberOfGeometries();
	for (int i = 0; i < nbGeometry; i++) {
		geo = roadRef->GetGeometry(i);
		if (!geo) continue;
		A_geo.Add(geo);
	}
	return A_geo;
}

/**
* Will get a road from the opendrive
* @param roadId - id of the road to read
* @param laneTypeMask - bitMask for the type of lane to extract from the file (all of them if left empty)
*/
UOpenDriveSolver::RoadData UOpenDriveSolver::GetRoad(int roadId, roadmanager::Lane::LaneType laneTypeMask ){
	RoadData road;

	road.RoadRef = _odr->GetRoadByIdx(roadId);
	if (!road.RoadRef) return road;
	road.A_LaneSection = GetLaneSection(road.RoadRef, laneTypeMask);
	road.A_Geometry = GetGeometries(road.RoadRef);

	MakeTransformArrayV2(road);

	return road;
}

/**
* Will get all the road in the opendrive
* @param laneType - bitMask for the type of lane to extract from the file
* @return An array of road 
*/
TArray<UOpenDriveSolver::RoadData> UOpenDriveSolver::getAllRoad(roadmanager::Lane::LaneType laneTypeMask){
	
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

/**
* Used to extract the transform of the lane of all the road passed in argument
* @param A_road - Array of road to evaluate
* @return An array of transform for the lane part
*/
TArray<FTransform> UOpenDriveSolver::extractRoadTransform(TArray<RoadData>& A_road){
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

/**
* Used to set the point that will be read by roadrunner for it's calcul
* Need to be called before computing value 
* @param offset - The offset (S value in opendrive) from the begining of the line to read the point at
* @param road - pointer to the road to compute
* @param lane - pointer to the lane to compute
*/
void UOpenDriveSolver::FindPoint(double offset, roadmanager::Road* road, roadmanager::Lane* lane) {
	if (lane == nullptr || road == nullptr) {
		_position.SetLanePos(_currentLane.Road->GetId(), _currentLane.Lane->GetId(), offset, 0.);
	} else {
		_position.SetLanePos(road->GetId(), lane->GetId(), offset, 0.);
	}
}
