// Fill out your copyright notice in the Description page of Project Settings.
#include "OpenDriveRoadEd.h"

// Sets default values
AOpenDriveRoadEd::AOpenDriveRoadEd()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	LaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/OpenDRIVE/EditorRessources/LaneSplineEdMesh"));

	baseMeshSize = LaneMesh->GetBoundingBox().GetSize().Y;
}

void AOpenDriveRoadEd::Initialize(int roadId_, int junctionId_, roadmanager::LaneSection* laneSection_, roadmanager::Lane* lane_)
{
	RoadId = roadId_;
	JunctionId = junctionId_;
	LaneSection = laneSection_;
	Lane = lane_;

	switch (Lane->GetLaneType())
	{
	case(roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):
		LaneType = "Driving road";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK):
		LaneType = "Sidewalk lane";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_PARKING):
		LaneType = "Parking slot(s)";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_BORDER):
		LaneType = "Border";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_RAIL):
		LaneType = "Rail";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_TRAM):
		LaneType = "Tram";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_SHOULDER):
		LaneType = "Shoulder";
		break;
	default:
		LaneType = "Any";
		break;
	}

	DrawLane();
}

void AOpenDriveRoadEd::DrawLane()
{
	/*roadmanager positions*/
	roadmanager::Position position;
	roadmanager::Position prevPosition;

	/*spline point vector, lane scale, lane length*/
	FVector sp;
	float Yscale = 1.0f;
	double laneLength = LaneSection->GetLength();

	USplineComponent* LaneSpline = NewObject<USplineComponent>(this);
	LaneSpline->SetupAttachment(RootComponent);
	LaneSpline->RegisterComponent();
	LaneSpline->ClearSplinePoints();

	/*start point*/
	position.Init();
	position.SetSnapLaneTypes(Lane->GetLaneType());
	position.SetLanePos(RoadId, Lane->GetId(), 0., 0.);
	position.SetHeadingRelativeRoadDirection(0.);
	sp = CoordTranslate::OdrToUe::ToLocation(position);
	sp.Z = roadOffset;
	LaneSpline->AddSplineWorldPoint(sp);
	Yscale = (LaneSection->GetWidth(0., Lane->GetId()) * 100) / baseMeshSize;
	LaneSpline->SetScaleAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, FVector(1.0f, Yscale, 1.0f));
	prevPosition = position;

	/*had a lane spline point every 5 meters, 
	only if there's and angle big enough between the current point and the previous one*/
	for (int s = 5; s < laneLength; s += 5)
	{
		position.Init();
		position.SetSnapLaneTypes(Lane->GetLaneType());
		position.SetLanePos(RoadId, Lane->GetId(), s, 0.);
		position.SetHeadingRelativeRoadDirection(0.);

		if (FMath::Abs(prevPosition.GetH() - position.GetH()) > FMath::DegreesToRadians(0.))
		{
			sp = CoordTranslate::OdrToUe::ToLocation(position);
			sp.Z = roadOffset;
			LaneSpline->AddSplineWorldPoint(sp);
			Yscale = (LaneSection->GetWidth(s, Lane->GetId()) * 100) / baseMeshSize;
			LaneSpline->SetScaleAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, FVector(1.0f, Yscale, 1.0f));

			prevPosition = position;
		}
	}

	/*final point*/
	position.Init();
	position.SetSnapLaneTypes(Lane->GetLaneType());
	position.SetLanePos(RoadId, Lane->GetId(), laneLength, 0.);
	position.SetHeadingRelativeRoadDirection(0.);
	sp = CoordTranslate::OdrToUe::ToLocation(position);
	sp.Z += roadOffset;
	LaneSpline->AddSplineWorldPoint(sp);
	Yscale = (LaneSection->GetWidth(laneLength, Lane->GetId()) * 100) / baseMeshSize;
	LaneSpline->SetScaleAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, FVector(1.0f, Yscale, 1.0f));

	/*spline meshes creation*/

	UMaterialInstance* meshMaterial = nullptr;

	switch (Lane->GetLaneType())
	{
	case(roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):

		if (Lane->GetId() > 0)
		{
			meshMaterial = LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/MI_LeftRoad"));
		}
		else
		{
			meshMaterial = LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/MI_RightRoad"));
		}
		break;

	case(roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK):

		meshMaterial = LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/MI_Sidewalk"));
		break;
	}

	for (int i = 0; i < LaneSpline->GetNumberOfSplinePoints() - 1; i++)
	{
		USplineMeshComponent* newSplineMesh = NewObject<USplineMeshComponent>(this);
		newSplineMesh->SetupAttachment(RootComponent);
		newSplineMesh->SetMobility(EComponentMobility::Movable);
		newSplineMesh->SetStaticMesh(LaneMesh);

		if (meshMaterial != nullptr)
		{
			newSplineMesh->SetMaterial(0, meshMaterial);
		}
		
		newSplineMesh->SetForwardAxis(ESplineMeshAxis::X);

		const FVector StartPoint = LaneSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector StartTangent = LaneSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector EndPoint = LaneSpline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		const FVector EndTangent = LaneSpline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

		newSplineMesh->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, false);

		const FVector StartScale = LaneSpline->GetScaleAtSplinePoint(i);
		const FVector EndScale = LaneSpline->GetScaleAtSplinePoint(i + 1);

		newSplineMesh->SetStartScale(FVector2D(StartScale.Y, StartScale.Z), false);
		newSplineMesh->SetEndScale(FVector2D(EndScale.Y, EndScale.Z), false);

		newSplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		newSplineMesh->UpdateRenderStateAndCollision();

		newSplineMesh->RegisterComponent();
	}

	LaneSpline->DestroyComponent();
}



