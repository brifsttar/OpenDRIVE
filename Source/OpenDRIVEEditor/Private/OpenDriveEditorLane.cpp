#include "OpenDriveEditorLane.h"

// Sets default values
AOpenDriveEditorLane::AOpenDriveEditorLane()
{
	PrimaryActorTick.bCanEverTick = false; 
	_laneMeshPtr = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/EditorLandscapeResources/SplineEditorMesh"));
	_baseMeshSize = _laneMeshPtr->GetBoundingBox().GetSize().Y; // The mesh's width. Used to set our lanes widths correctly.
}

void AOpenDriveEditorLane::Initialize(roadmanager::Road* road_, roadmanager::LaneSection* laneSection_, roadmanager::Lane* lane_, float offset_, float step_)
{
	_road = road_;
	_laneSection = laneSection_;
	_lane = lane_;
	
	DrawLane(step_, offset_);
}

void AOpenDriveEditorLane::DrawLane(double step, float offset)
{
	roadmanager::Position position;

	double laneLength = _laneSection->GetLength();
	double s = _laneSection->GetS();

	// Spline component creation
	USplineComponent* laneSpline = NewObject<USplineComponent>(this);
	laneSpline->SetupAttachment(RootComponent);
	laneSpline->RegisterComponent();
	laneSpline->ClearSplinePoints();

	// Start point
	position.Init();
	position.SetSnapLaneTypes(_lane->GetLaneType());
	SetLanePoint(laneSpline, position, s, offset);

	//Driving direction
	_roadDirection = position.GetDrivingDirectionRelativeRoad();

	// Had a lane spline point every 5 meters
	s += step;
	for (s ; s < _laneSection->GetS() + laneLength; s += step)
	{
		SetLanePoint(laneSpline, position, s, offset);
	}

	// Final point
	SetLanePoint(laneSpline, position, _laneSection->GetS() + laneLength, offset);
	if (laneLength > step)
	{
		CheckLastTwoPointsDistance(laneSpline, step);
	}

	bool bIsInJunction = GetJunctionId() != -1;

	//arrow meshes 
	TObjectPtr<UStaticMesh> mesh;
	switch (_lane->GetLaneType())
	{
	case(roadmanager::Lane::LaneType::LANE_TYPE_RESTRICTED):
		mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Meshes/Warning"));
		break;

	case(roadmanager::Lane::LaneType::LANE_TYPE_BIKING):
		mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Meshes/Bike"));
		break;

	case(roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):
		mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Meshes/BetterArrow"));
		break;
	}
	if (mesh != nullptr)
	{
		SetArrowMeshes(laneSpline, mesh, bIsInJunction);
	}
	//if its not a junction, we draw the spline meshes, else we just keep the spline line 
	if (bIsInJunction == false)
	{
		// Spline meshes creation 
		SetColoredLaneMeshes(laneSpline);
	}
	else
	{
		//set driving lane's spline's color in junction
		switch (_lane->GetLaneType())
		{
		case (roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):
			laneSpline->EditorUnselectedSplineSegmentColor = _lane->GetId() > 0 ? FLinearColor::Red : FLinearColor::Green;
			break;

		case(roadmanager::Lane::LaneType::LANE_TYPE_RESTRICTED):
			laneSpline->EditorUnselectedSplineSegmentColor = FLinearColor(1, 0.5, 0);
			break;

		case(roadmanager::Lane::LaneType::LANE_TYPE_BIKING):
			laneSpline->EditorUnselectedSplineSegmentColor = FLinearColor(0, 0, 1);
			break;
		}
	}
}

FString AOpenDriveEditorLane::GetLaneType()
{
	FString type;

	switch (_lane->GetLaneType())
	{
	case(roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):
		type = "Driving road";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_BIKING):
		type = "Bike path";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK):
		type = "Sidewalk lane";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_PARKING):
		type = "Parking slot(s)";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_BORDER):
		type = "Border";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_RAIL):
		type = "Rail";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_TRAM):
		type = "Tram";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_SHOULDER):
		type = "Shoulder";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_RESTRICTED):
		type = "Restricted lane";
		break;
	default:
		type = "Any";
		break;
	}

	return type;
}

inline int AOpenDriveEditorLane::GetSuccessorId()
{
	roadmanager::RoadLink* link = _road->GetLink(roadmanager::LinkType::SUCCESSOR);
	return  link != nullptr ? link->GetElementId() :  0;
}

inline int AOpenDriveEditorLane::GetPredecessorId()
{
	roadmanager::RoadLink* link = _road->GetLink(roadmanager::LinkType::PREDECESSOR);
	return  link != nullptr ? link->GetElementId() : 0;
}

void AOpenDriveEditorLane::SetLanePoint(USplineComponent* laneSpline_, roadmanager::Position& position, double s, float offset)
{
	position.SetLanePos(GetRoadId(), _lane->GetId(), s, 0.);
	
	FVector sp;
	sp = CoordTranslate::OdrToUe::ToLocation(position);
	sp.Z += offset;
	laneSpline_->AddSplineWorldPoint(sp);
	FRotator rot = CoordTranslate::OdrToUe::ToRotation(position);
	laneSpline_->SetRotationAtSplinePoint(laneSpline_->GetNumberOfSplinePoints() - 1, rot, ESplineCoordinateSpace::World);
	
	float Yscale = (_laneSection->GetWidth(position.GetS(), _lane->GetId()) * 100) / _baseMeshSize;
	laneSpline_->SetScaleAtSplinePoint(laneSpline_->GetNumberOfSplinePoints() - 1, FVector(1.0f, Yscale, 1.0f));
}

void AOpenDriveEditorLane::CheckLastTwoPointsDistance(USplineComponent* laneSpline_, float step_)
{
	float dist = FVector::Distance(laneSpline_->GetWorldLocationAtSplinePoint(laneSpline_->GetNumberOfSplinePoints() - 2),
		laneSpline_->GetWorldLocationAtSplinePoint(laneSpline_->GetNumberOfSplinePoints() - 1));
	if ((dist / 100) < step_ / 3)
	{
		laneSpline_->RemoveSplinePoint(laneSpline_->GetNumberOfSplinePoints() - 2);
	}
}

void AOpenDriveEditorLane::SetArrowMeshes(USplineComponent* laneSpline_, TObjectPtr<UStaticMesh> mesh, bool isJunction)
{
	for (int i = 1; i < laneSpline_->GetNumberOfSplinePoints() - 1; i += 2)
	{
		UStaticMeshComponent* newStaticMesh = NewObject<UStaticMeshComponent>(this);
		newStaticMesh->SetupAttachment(RootComponent);
		newStaticMesh->SetMobility(EComponentMobility::Movable);
		newStaticMesh->SetStaticMesh(mesh);
		newStaticMesh->SetWorldLocation(laneSpline_->GetWorldLocationAtSplinePoint(i) + FVector(0.f,0.f,5.0f));
		FRotator rotation = laneSpline_->GetRotationAtSplinePoint(i, ESplineCoordinateSpace::World);
		rotation.Yaw += _roadDirection == -1 ? 180.0 : 0.0;
		newStaticMesh->SetWorldRotation(rotation);

		if (isJunction == true)
		{
			newStaticMesh->SetWorldScale3D(newStaticMesh->GetComponentScale() / 2);
		}
		else
		{
			newStaticMesh->SetVisibility(false);
		}

		newStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		newStaticMesh->RegisterComponent();
		FArrowMeshes.Add(newStaticMesh);
	}
}

void AOpenDriveEditorLane::SetColoredLaneMeshes(USplineComponent* laneSpline_)
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
			break;
			
		case(roadmanager::Lane::LaneType::LANE_TYPE_BIKING):
			newSplineMesh->SetMaterial(0, LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/MI_Biking")));
			break;

		case(roadmanager::Lane::LaneType::LANE_TYPE_RESTRICTED):
			newSplineMesh->SetMaterial(0, LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/MI_RestrictedRoad")));
			break;

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

void AOpenDriveEditorLane::SetArrowVisibility(bool _isVisible)
{
	if (GetJunctionId() == -1)
	{
		for (UStaticMeshComponent* arrow : FArrowMeshes)
		{
			arrow->SetVisibility(_isVisible);
		}
	}
}
