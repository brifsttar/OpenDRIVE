#include "OpenDriveRoadEd.h"

// Sets default values
AOpenDriveRoadEd::AOpenDriveRoadEd()
{
	PrimaryActorTick.bCanEverTick = false; 
	
	_laneMeshPtr = LoadObject<UStaticMesh>(nullptr, TEXT("/OpenDRIVE/EditorRessources/LaneSplineEdMesh"));

	_baseMeshSize = _laneMeshPtr->GetBoundingBox().GetSize().Y; // The mesh's width. Used to set our lanes widths correctly.
}

void AOpenDriveRoadEd::Initialize(int roadId_, int junctionId_, roadmanager::LaneSection* laneSection_, roadmanager::Lane* lane_, float offset_)
{
	_roadId = roadId_;
	_junctionId = junctionId_;
	_laneSection = laneSection_;
	_lane = lane_;

	_roadOffset = offset_;

	switch (_lane->GetLaneType())
	{
	case(roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):
		_laneType = "Driving road";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK):
		_laneType = "Sidewalk lane";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_PARKING):
		_laneType = "Parking slot(s)";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_BORDER):
		_laneType = "Border";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_RAIL):
		_laneType = "Rail";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_TRAM):
		_laneType = "Tram";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_SHOULDER):
		_laneType = "Shoulder";
		break;
	default:
		_laneType = "Any";
		break;
	}

	DrawLane(false);
}

void AOpenDriveRoadEd::DrawLane(bool perfMode_)
{
	// Roadmanager positions
	roadmanager::Position position;
	roadmanager::Position prevPosition;

	// Spline point vector, lane scale, lane length
	FVector sp;
	float Yscale = 1.0f;
	double laneLength = _laneSection->GetLength();

	// Spline component creation
	USplineComponent* LaneSpline = NewObject<USplineComponent>(this);
	LaneSpline->SetupAttachment(RootComponent);
	LaneSpline->RegisterComponent();
	LaneSpline->ClearSplinePoints();

	// Start point
	position.Init();
	position.SetSnapLaneTypes(_lane->GetLaneType());
	position.SetLanePos(_roadId, _lane->GetId(), 0., 0.);
	position.SetHeadingRelativeRoadDirection(0.);
	sp = CoordTranslate::OdrToUe::ToLocation(position);
	sp.Z = _roadOffset;
	LaneSpline->AddSplineWorldPoint(sp);
	Yscale = (_laneSection->GetWidth(0., _lane->GetId()) * 100) / _baseMeshSize;
	LaneSpline->SetScaleAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, FVector(1.0f, Yscale, 1.0f));
	prevPosition = position;

	// Had a lane spline point every 5 meters
	if (perfMode_ == true)
	{
		for (int s = 1; s < laneLength; s += 1)
		{
			position.Init();
			position.SetSnapLaneTypes(_lane->GetLaneType());
			position.SetLanePos(_roadId, _lane->GetId(), s, 0.);
			position.SetHeadingRelativeRoadDirection(0.);

			if (FMath::Abs(prevPosition.GetH() - position.GetH()) > FMath::DegreesToRadians(0.))
			{
				sp = CoordTranslate::OdrToUe::ToLocation(position);
				sp.Z = _roadOffset;
				LaneSpline->AddSplineWorldPoint(sp);
				Yscale = (_laneSection->GetWidth(s, _lane->GetId()) * 100) / _baseMeshSize;
				LaneSpline->SetScaleAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, FVector(1.0f, Yscale, 1.0f));

				prevPosition = position;
			}
		}
	}
	else
	{
		for (int s = 5; s < laneLength; s += 5)
		{
			position.Init();
			position.SetSnapLaneTypes(_lane->GetLaneType());
			position.SetLanePos(_roadId, _lane->GetId(), s, 0.);
			position.SetHeadingRelativeRoadDirection(0.);

			sp = CoordTranslate::OdrToUe::ToLocation(position);
			sp.Z = _roadOffset;
			LaneSpline->AddSplineWorldPoint(sp);
			Yscale = (_laneSection->GetWidth(s, _lane->GetId()) * 100) / _baseMeshSize;
			LaneSpline->SetScaleAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, FVector(1.0f, Yscale, 1.0f));
		}
	}

	// Final point
	position.Init();
	position.SetSnapLaneTypes(_lane->GetLaneType());
	position.SetLanePos(_roadId, _lane->GetId(), laneLength, 0.);
	position.SetHeadingRelativeRoadDirection(0.);
	sp = CoordTranslate::OdrToUe::ToLocation(position);
	sp.Z += _roadOffset;
	LaneSpline->AddSplineWorldPoint(sp);
	Yscale = (_laneSection->GetWidth(laneLength, _lane->GetId()) * 100) / _baseMeshSize;
	LaneSpline->SetScaleAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, FVector(1.0f, Yscale, 1.0f));

	for (int i = 0; i < LaneSpline->GetNumberOfSplinePoints(); i++)
	{
		LaneSpline->SetSplinePointType(i, ESplinePointType::Linear);
	}

	// Material loading
	UMaterialInstance* meshMaterial = nullptr;
	switch (_lane->GetLaneType())
	{
	case(roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):

		if (_lane->GetId() > 0)
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

	// Spline meshes creation 
	for (int i = 0; i < LaneSpline->GetNumberOfSplinePoints() - 1; i++)
	{
		USplineMeshComponent* newSplineMesh = NewObject<USplineMeshComponent>(this);
		newSplineMesh->SetupAttachment(RootComponent);
		newSplineMesh->SetMobility(EComponentMobility::Movable);
		newSplineMesh->SetStaticMesh(_laneMeshPtr);
		if (meshMaterial != nullptr)
		{
			newSplineMesh->SetMaterial(0, meshMaterial);
		}
		newSplineMesh->SetForwardAxis(ESplineMeshAxis::X);

		// Start and end points (position and tangent)
		const FVector StartPoint = LaneSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector StartTangent = LaneSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector EndPoint = LaneSpline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		const FVector EndTangent = LaneSpline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		newSplineMesh->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, false);

		// Start and end scale 
		const FVector StartScale = LaneSpline->GetScaleAtSplinePoint(i);
		const FVector EndScale = LaneSpline->GetScaleAtSplinePoint(i + 1);
		newSplineMesh->SetStartScale(FVector2D(StartScale.Y, StartScale.Z), false);
		newSplineMesh->SetEndScale(FVector2D(EndScale.Y, EndScale.Z), false);

		// Turn off all collision
		newSplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Update spline mesh render and register it so it can be visible in editor viewport
		newSplineMesh->UpdateRenderStateAndCollision();
		newSplineMesh->RegisterComponent();
	}

	// To regain some performance, we can destroy the spline. Now that we have generated the meshes, spline points are no longer needed.
	LaneSpline->DestroyComponent();
}



