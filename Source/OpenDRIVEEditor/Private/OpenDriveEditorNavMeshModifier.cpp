// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveEditorNavMeshModifier.h"


void AOpenDriveEditorNavMeshModifier::OnConstruction(const FTransform& transform){
	Super::OnConstruction(transform);
	_splineMeshBuilder->Rebuild();
}

// Sets default values
AOpenDriveEditorNavMeshModifier::AOpenDriveEditorNavMeshModifier()
{
	PrimaryActorTick.bCanEverTick = true;


	_spline = CreateDefaultSubobject<USplineComponent>(FName("Spline"));
	_splineMeshBuilder = CreateDefaultSubobject<USplineMeshBuilder>(FName("splineMeshBuilder"));
	_splineMeshBuilder->Spline = _spline;
	_splineMeshBuilder->Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/EditorLandscapeResources/SplineEditorMesh"));
	_baseMeshSize = _splineMeshBuilder->Mesh->GetBoundingBox().GetSize().Y; // The mesh's width. Used to set our lanes widths correctly.
	_splineMeshBuilder->AddNavAreas = true;
	//_splineMeshBuilder->Rebuild();
}

void AOpenDriveEditorNavMeshModifier::Initialize(roadmanager::Road* road_, roadmanager::LaneSection* laneSection_, roadmanager::Lane* lane_, float offset_, float step_)
{
	_road = road_;
	_laneSection = laneSection_;
	_lane = lane_;

	_step = step_;
	_offset = offset_;
	


	_laneLength = _laneSection->GetLength();
	_blockCount = (int)(_laneLength / step_) + 1;
	_blockSize = _laneLength / (float)_blockCount;

	_position.Init();
	_position.SetSnapLaneTypes(_lane->GetLaneType());

}



void AOpenDriveEditorNavMeshModifier::CreateSpline() {
	_spline->ClearSplinePoints();

	double s = _laneSection->GetS();

	//StartPoint
	SetLanePoint(s);

	// Add a lane spline point every Step meters
	s += _step;
	for (s; s < _laneSection->GetS() + _laneLength; s += _step){
		SetLanePoint(s);
	}

	//Final point
	SetLanePoint(_laneSection->GetS() + _laneLength);

	//TODO FINAL POINT DIST


}

void AOpenDriveEditorNavMeshModifier::SetLanePoint(double s)
{
	_position.SetLanePos(_road->GetId(), _lane->GetId(), s, 0.);

	FVector sp;
	sp = CoordTranslate::OdrToUe::ToLocation(_position);
	sp.Z += _offset;
	_spline->AddSplineWorldPoint(sp);
	FRotator rot = CoordTranslate::OdrToUe::ToRotation(_position);
	_spline->SetRotationAtSplinePoint(_spline->GetNumberOfSplinePoints() - 1, rot, ESplineCoordinateSpace::World);

	float Yscale = (_laneSection->GetWidth(_position.GetS(), _lane->GetId()) * 100) / _baseMeshSize;
	_spline->SetScaleAtSplinePoint(_spline->GetNumberOfSplinePoints() - 1, FVector(1.0f, Yscale, 1.0f));
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

