// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveSolver.h"

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
 



/**
* 
*
*
*/
void UOpenDriveSolver::Initialize(LaneRef currentLane, int lineType){

	_currentLane = currentLane;


	_position.SetSnapLaneTypes(lineType);


	_laneLength = _currentLane.laneSection->GetLength();
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
	double s = _currentLane.laneSection->GetS();

	double middleOfLane = _currentLane.laneSection->GetLength() / 2 + s; //TODO CHECK LINE TYPE

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

TArray<FTransform> UOpenDriveSolver::MakeTransformArrayV2(RoadData road){
	//road.A_laneSection[0].sectionRef->GetLength()


	int lsIndex, geoIndex, SNow = 0;

	while (SNow <= road.roadRef->GetLength()) {
		if (road.A_geometry[geoIndex]->GetS() + road.A_geometry[geoIndex]->GetLength() <
		road.A_laneSection[lsIndex].sectionRef->GetS() + road.A_laneSection[lsIndex].sectionRef->GetLength()) {






		}else {

		}





	}









	return TArray<FTransform>();
}

void UOpenDriveSolver::SetRoad(roadmanager::Road* road){
	_currentLane.road = road;
}

void UOpenDriveSolver::SetRoad(int id){
	_currentLane.road = _odr->GetRoadByIdx(id);
}

void UOpenDriveSolver::SetLane(roadmanager::Lane* lane){
	_currentLane.lane = lane;
}

void UOpenDriveSolver::SetLane(int id){
	_currentLane.lane = _currentLane.laneSection->GetLaneByIdx(id);
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

UOpenDriveSolver::RoadData UOpenDriveSolver::getRoad(int roadId){
	RoadData road;
	int nbLane, nbLaneSection, nbGeometry = 0;
	int i, j = 0;
	roadmanager::Lane* lane = 0;
	roadmanager::Geometry* geo = 0;

	road.roadRef = _odr->GetRoadByIdx(roadId);
	if (!road.roadRef) return road;

	nbLaneSection = road.roadRef->GetNumberOfLaneSections();						//LANE SECTION
	for (i = 0; i < nbLaneSection; i++) {
		LaneSectionData section;
		section.sectionRef = road.roadRef->GetLaneSectionByIdx(j);
		if (!section.sectionRef) continue;

		nbLane = section.sectionRef->GetNumberOfLanes();							//LANE
		for (j = 0; j < nbLane; j++) {
			lane = section.sectionRef->GetLaneByIdx(j);
			if (!lane || lane->GetId() == 0) continue;
			section.A_lane.Add(LaneData(lane, lane->GetLaneType()));
		}
		road.A_laneSection.Add(section);
	}

	nbGeometry = road.roadRef->GetNumberOfGeometries();
	for (i = 0; i < nbGeometry; i++) {
		geo = road.roadRef->GetGeometry(i);
		if (!geo) continue;
		road.A_geometry.Add(geo);
	}
	return road;
}

TArray<UOpenDriveSolver::RoadData> UOpenDriveSolver::getAllRoad(){
	
	TArray<RoadData> A_road;
	RoadData road;

	int nbRoad = _odr->GetNumOfRoads();												//ROAD
	for (int i = 0; i < nbRoad; i++) {
		road = getRoad(i);
		if (!road.roadRef) continue;
		A_road.Add(road);
	}

	return A_road;
}




/**
* Used to set the point that will be read by roadrunner for it's calcul
* Need to be called before computing value 
* @param offset - The offset (S value in opendrive) from the begining of the line to read the point at
*/
void UOpenDriveSolver::FindPoint(double offset){
	_position.SetLanePos(_currentLane.road->GetId(), _currentLane.lane->GetId(), offset, 0.);
}


