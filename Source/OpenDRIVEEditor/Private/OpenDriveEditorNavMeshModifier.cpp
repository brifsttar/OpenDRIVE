// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveEditorNavMeshModifier.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include <RoadManager.hpp>
#include <NavigationSystem.h>
#include <OpenDriveComponent.h>


// Sets default values
AOpenDriveEditorNavMeshModifier::AOpenDriveEditorNavMeshModifier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AOpenDriveEditorNavMeshModifier::Initialize(roadmanager::Road* road_, roadmanager::LaneSection* laneSection_, roadmanager::Lane* lane_, float offset_, float step_)
{
	_road = road_;
	_laneSection = laneSection_;
	_lane = lane_;


	


	_laneLength = _laneSection->GetLength();
	_blockCount = (int)(_laneLength / step_) + 1;
	_blockSize = _laneLength / (float)_blockCount;

	_position.Init();
	_position.SetSnapLaneTypes(roadmanager::Lane::LANE_TYPE_SIDEWALK);

}

TArray<FTransform> AOpenDriveEditorNavMeshModifier::makeTransformArray(double offset) {
	//get the starting point of the lane
	double s = _laneSection->GetS();

	TArray<FTransform> TList;
	
	for (int i = 0; i < _blockCount; i++) {
		TList.Add(MakeTransform(_blockSize / 2 + i * _blockSize + s));
	}

	return TList;
}

FTransform AOpenDriveEditorNavMeshModifier::MakeTransform(double offset) {

	findPoint(offset);
	FVector sp = CoordTranslate::OdrToUe::ToLocation(_position);
	FRotator rot = CoordTranslate::OdrToUe::ToRotation(_position);
	FVector sc = FVector(_blockSize, 1, 10);

	FTransform T(rot, sp, sc);

	return T;
}




void AOpenDriveEditorNavMeshModifier::findPoint(double offset) {
	_position.SetLanePos(_road->GetId(), _lane->GetId(), offset, 0.);
}


// Called when the game starts or when spawned
void AOpenDriveEditorNavMeshModifier::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOpenDriveEditorNavMeshModifier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

