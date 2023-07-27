#include "OpenDriveRoadEd.h"

// Sets default values
AOpenDriveRoadEd::AOpenDriveRoadEd()
{
	PrimaryActorTick.bCanEverTick = false; 
	
	_laneMeshPtr = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/EditorLandscapeResources/SplineEditorMesh"));
	_laneArrowMeshPtr = LoadObject<UStaticMesh>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Meshes/BetterArrow"));

	_baseMeshSize = _laneMeshPtr->GetBoundingBox().GetSize().Y; // The mesh's width. Used to set our lanes widths correctly.
}

void AOpenDriveRoadEd::Initialize(int roadId_, int junctionId_,int successorId_, int predecessorId_,roadmanager::LaneSection* laneSection_, roadmanager::Lane* lane_, float offset_, float step_)
{
	_roadId = roadId_;
	_junctionId = junctionId_;
	_successorId = successorId_;
	_predecessorId = predecessorId_;
	_laneSection = laneSection_;
	_lane = lane_;
	SetLaneType();

	DrawLane(step_, offset_);
}

void AOpenDriveRoadEd::SetLaneType()
{
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
}

void AOpenDriveRoadEd::DrawLane(double step, float offset)
{
	roadmanager::Position position;

	double laneLength = _laneSection->GetLength();
	double s = _laneSection->GetS();

	// Spline component creation
	USplineComponent* LaneSpline = NewObject<USplineComponent>(this);
	LaneSpline->SetupAttachment(RootComponent);
	LaneSpline->RegisterComponent();
	LaneSpline->ClearSplinePoints();

	// Start point
	position.Init();
	position.SetSnapLaneTypes(_lane->GetLaneType());
	SetLanePoint(LaneSpline, position, s, offset);

	//Driving direction
	_roadDirection = position.GetDrivingDirectionRelativeRoad();

	// Had a lane spline point every 5 meters
	s += step;
	for (s ; s < _laneSection->GetS() + laneLength; s += step)
	{
		SetLanePoint(LaneSpline, position, s, offset);
	}

	// Final point
	SetLanePoint(LaneSpline, position, _laneSection->GetS() + laneLength, offset);
	if (laneLength > step)
	{
		CheckLastTwoPointsDistance(LaneSpline, step);
	}

	//arrow meshes 
	if (_lane->GetLaneType() == roadmanager::Lane::LaneType::LANE_TYPE_DRIVING)
	{
		if (_junctionId == -1)
		{
			SetArrowMeshes(LaneSpline, false);
		}
		else
		{
			SetArrowMeshes(LaneSpline, true);
		}
	}

	//if its not a junction, we draw the spline meshes, else we just keep the spline line 
	if (_junctionId == -1)
	{
		// Spline meshes creation 
		SetColoredLaneMeshes(LaneSpline);
	}
	else
	{
		//set driving lane's spline's color in junction
		if (_lane->GetLaneType() == roadmanager::Lane::LaneType::LANE_TYPE_DRIVING)
		{
			LaneSpline->EditorUnselectedSplineSegmentColor = _lane->GetId() > 0 ? FLinearColor::Red : FLinearColor::Green;
		}
	}
}

void AOpenDriveRoadEd::SetLanePoint(USplineComponent* laneSpline_, roadmanager::Position& position, double s, float offset)
{
	position.SetLanePos(_roadId, _lane->GetId(), s, 0.);
	
	FVector sp;
	sp = CoordTranslate::OdrToUe::ToLocation(position);
	sp.Z += offset;
	laneSpline_->AddSplineWorldPoint(sp);
	FRotator rot = FRotator::ZeroRotator;
	rot.Yaw = -FMath::RadiansToDegrees(position.GetH());
	laneSpline_->SetRotationAtSplinePoint(laneSpline_->GetNumberOfSplinePoints() - 1, rot, ESplineCoordinateSpace::World);
	float Yscale = (_laneSection->GetWidth(position.GetS(), _lane->GetId()) * 100) / _baseMeshSize;
	laneSpline_->SetScaleAtSplinePoint(laneSpline_->GetNumberOfSplinePoints() - 1, FVector(1.0f, Yscale, 1.0f));
}

void AOpenDriveRoadEd::CheckLastTwoPointsDistance(USplineComponent* laneSpline_, float step_)
{
	float dist = FVector::Distance(laneSpline_->GetWorldLocationAtSplinePoint(laneSpline_->GetNumberOfSplinePoints() - 2),
		laneSpline_->GetWorldLocationAtSplinePoint(laneSpline_->GetNumberOfSplinePoints() - 1));
	if ((dist / 100) < step_ / 3)
	{
		laneSpline_->RemoveSplinePoint(laneSpline_->GetNumberOfSplinePoints() - 2);
	}
}

void AOpenDriveRoadEd::SetArrowMeshes(USplineComponent* laneSpline_, bool _isJunction)
{
	for (int i = 1; i < laneSpline_->GetNumberOfSplinePoints() - 1; i += 2)
	{
		UStaticMeshComponent* newStaticMesh = NewObject<UStaticMeshComponent>(this);
		newStaticMesh->SetupAttachment(RootComponent);
		newStaticMesh->SetMobility(EComponentMobility::Movable);
		newStaticMesh->SetStaticMesh(_laneArrowMeshPtr);
		newStaticMesh->SetWorldLocation(laneSpline_->GetWorldLocationAtSplinePoint(i) + FVector(0.f,0.f,50.0f));
		FRotator rotation = laneSpline_->GetRotationAtSplinePoint(i, ESplineCoordinateSpace::World);
		rotation.Yaw += _roadDirection == -1 ? FMath::RadiansToDegrees(PI) : 0.f;
		newStaticMesh->SetWorldRotation(rotation);

		if (_isJunction == true)
		{
			newStaticMesh->SetWorldScale3D(newStaticMesh->GetComponentScale() / 2);
		}
		else
		{
			newStaticMesh->SetVisibility(false);
		}

		newStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		newStaticMesh->RegisterComponent();
		_arrowMeshes.Add(newStaticMesh);
	}
}

void AOpenDriveRoadEd::SetColoredLaneMeshes(USplineComponent* laneSpline_)
{	
	for (int i = 0; i < laneSpline_->GetNumberOfSplinePoints() - 1; i++)
	{
		USplineMeshComponent* newSplineMesh = NewObject<USplineMeshComponent>(this);
		newSplineMesh->SetupAttachment(RootComponent);
		newSplineMesh->SetMobility(EComponentMobility::Movable);
		newSplineMesh->SetStaticMesh(_laneMeshPtr);

		switch (_lane->GetLaneType())
		{
		case(roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):
			newSplineMesh->SetMaterial(0, _lane->GetId() > 0 ? LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/MI_LeftRoad")) : LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/MI_RightRoad")));
			break;

		case(roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK):
			newSplineMesh->SetMaterial(0, LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/MI_Sidewalk")));
			break;

		case(roadmanager::Lane::LaneType::LANE_TYPE_SHOULDER):
			newSplineMesh->SetMaterial(0, LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/MI_Shoulder")));
			break;
		
		case(roadmanager::Lane::LaneType::LANE_TYPE_PARKING):
			newSplineMesh->SetMaterial(0, LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/MI_Parking")));

		default:
			newSplineMesh->SetMaterial(0, LoadObject<UMaterial>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/M_LaneSplineEd")));
			break;
		}

		newSplineMesh->SetForwardAxis(ESplineMeshAxis::X);

		// Start and end points (position and tangent)
		const FVector StartPoint = laneSpline_->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector StartTangent = laneSpline_->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector EndPoint = laneSpline_->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		const FVector EndTangent = laneSpline_->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		newSplineMesh->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, false);

		// Start and end scale 
		const FVector StartScale = laneSpline_->GetScaleAtSplinePoint(i);
		const FVector EndScale = laneSpline_->GetScaleAtSplinePoint(i + 1);
		newSplineMesh->SetStartScale(FVector2D(StartScale.Y, StartScale.Z), false);
		newSplineMesh->SetEndScale(FVector2D(EndScale.Y, EndScale.Z), false);

		// Turn off all collision
		newSplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Update spline mesh render and register it so it can be visible in editor viewport
		newSplineMesh->UpdateRenderStateAndCollision();
		newSplineMesh->RegisterComponent();
	}

	// To regain some performance, we can destroy the spline. Now that we have generated the meshes, spline points are no longer needed.
	laneSpline_->DestroyComponent();
}

void AOpenDriveRoadEd::SetArrowVisibility(bool _isVisible)
{
	if (_junctionId == -1)
	{
		for (UStaticMeshComponent* arrow : _arrowMeshes)
		{
			arrow->SetVisibility(_isVisible);
		}
	}
}

