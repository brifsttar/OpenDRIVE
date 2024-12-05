#include "OpenDriveEditorLane.h"
#include "CoordTranslate.h"
#include "Components/SplineMeshComponent.h"

// Sets default values
AOpenDriveEditorLane::AOpenDriveEditorLane(): Road(nullptr), LaneSection(nullptr), Lane(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
	bEditable = true;
	LaneMeshPtr = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/EditorLandscapeResources/SplineEditorMesh"));
	BaseMeshSize = LaneMeshPtr->GetBoundingBox().GetSize().Y; // The mesh's width. Used to set our lanes widths correctly.
	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	RootComponent = RootComp;
}

void AOpenDriveEditorLane::Initialize(roadmanager::Road* RoadIn, roadmanager::LaneSection* LaneSectionIn, roadmanager::Lane* LaneIn, const float Offset, const float Step)
{
	Road = RoadIn;
	LaneSection = LaneSectionIn;
	Lane = LaneIn;
	DrawLane(Step, Offset);
}

void AOpenDriveEditorLane::DrawLane(double Step, float Offset)
{
	roadmanager::Position Position;

	double LaneLength = LaneSection->GetLength();
	double s = LaneSection->GetS();

	// Spline component creation
	USplineComponent* LaneSpline = NewObject<USplineComponent>(this);
	LaneSpline->SetupAttachment(RootComponent);
	LaneSpline->RegisterComponent();
	LaneSpline->ClearSplinePoints();

	// Start point
	Position.Init();
	Position.SetSnapLaneTypes(roadmanager::Lane::LANE_TYPE_ANY);
	SetLanePoint(LaneSpline, Position, s, Offset);

	//Driving direction
	RoadDirection = Position.GetDrivingDirectionRelativeRoad();

	// Add a lane spline point every 5 meters
	s += Step;
	for (s ; s < LaneSection->GetS() + LaneLength; s += Step)
	{
		SetLanePoint(LaneSpline, Position, s, Offset);
	}

	// Final point
	SetLanePoint(LaneSpline, Position, LaneSection->GetS() + LaneLength, Offset);
	if (LaneLength > Step)
	{
		CheckLastTwoPointsDistance(LaneSpline, Step);
	}

	bool bIsInJunction = GetJunctionId() != -1;

	//arrow meshes 
	TObjectPtr<UStaticMesh> Mesh;
	switch (Lane->GetLaneType())
	{
	case(roadmanager::Lane::LaneType::LANE_TYPE_RESTRICTED):
		Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Meshes/Warning"));
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_BIKING):
		Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Meshes/Bike"));
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):
		Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Meshes/BetterArrow"));
		break;
	default:
		Mesh = nullptr;
		break;
	}
	if (Mesh != nullptr)
	{
		SetArrowMeshes(LaneSpline, Mesh, bIsInJunction);
	}
	//if its not a junction, we draw the spline meshes, else we just keep the spline line 
	if (bIsInJunction == false)
	{
		// Spline meshes creation 
		SetColoredLaneMeshes(LaneSpline);
	}
	else
	{
		//set driving lane's spline's color in junction
		switch (Lane->GetLaneType())
		{
		case (roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):
			LaneSpline->EditorUnselectedSplineSegmentColor = Lane->GetId() > 0 ? FLinearColor::Red : FLinearColor::Green;
			break;
		case(roadmanager::Lane::LaneType::LANE_TYPE_RESTRICTED):
			LaneSpline->EditorUnselectedSplineSegmentColor = FLinearColor(1, 0.5, 0);
			break;
		case(roadmanager::Lane::LaneType::LANE_TYPE_BIKING):
			LaneSpline->EditorUnselectedSplineSegmentColor = FLinearColor(0, 0, 1);
			break;
		default:
			LaneSpline->EditorUnselectedSplineSegmentColor = FLinearColor(0.25, 0.25, 0.25);
			break;
		}
	}
}

FString AOpenDriveEditorLane::GetLaneType() const
{
	FString Type;

	switch (Lane->GetLaneType())
	{
	case(roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):
		Type = "Driving road";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_BIKING):
		Type = "Bike path";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK):
		Type = "Sidewalk lane";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_PARKING):
		Type = "Parking slot(s)";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_BORDER):
		Type = "Border";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_RAIL):
		Type = "Rail";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_TRAM):
		Type = "Tram";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_SHOULDER):
		Type = "Shoulder";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_RESTRICTED):
		Type = "Restricted lane";
		break;
	case(roadmanager::Lane::LaneType::LANE_TYPE_MEDIAN):
		Type = "Median";
		break;
	default:
		Type = "None";
		break;
	}

	return Type;
}

int AOpenDriveEditorLane::GetSuccessorId() const
{
	const roadmanager::RoadLink* Link = Road->GetLink(roadmanager::LinkType::SUCCESSOR);
	return  Link != nullptr ? Link->GetElementId() :  -1;
}

int AOpenDriveEditorLane::GetPredecessorId() const
{
	const roadmanager::RoadLink* Link = Road->GetLink(roadmanager::LinkType::PREDECESSOR);
	return  Link != nullptr ? Link->GetElementId() : -1;
}

void AOpenDriveEditorLane::SetLanePoint(USplineComponent* LaneSpline, roadmanager::Position& Position, const double S, const float Offset) const
{
	Position.SetLanePos(GetRoadId(), GetLaneId(), S, 0.);

	FVector SP = CoordTranslate::OdrToUe::ToLocation(Position);
	SP.Z += Offset;
	LaneSpline->AddSplineWorldPoint(SP);
	const FRotator Rotator = CoordTranslate::OdrToUe::ToRotation(Position);
	LaneSpline->SetRotationAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, Rotator, ESplineCoordinateSpace::World);

	const float Yscale = (LaneSection->GetWidth(Position.GetS(), Lane->GetId()) * 100) / BaseMeshSize;
	LaneSpline->SetScaleAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, FVector(1.0f, Yscale, 1.0f));
}

void AOpenDriveEditorLane::CheckLastTwoPointsDistance(USplineComponent* LaneSpline, const float Step)
{
	const float Dist = FVector::Distance(LaneSpline->GetWorldLocationAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 2),
		LaneSpline->GetWorldLocationAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1));
	if ((Dist / 100) < Step / 3)
	{
		LaneSpline->RemoveSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 2);
	}
}

void AOpenDriveEditorLane::SetArrowMeshes(const USplineComponent* LaneSpline, const TObjectPtr<UStaticMesh>& Mesh, const bool bIsJunction)
{
	for (int i = 1; i < LaneSpline->GetNumberOfSplinePoints() - 1; i += 2)
	{
		UStaticMeshComponent* NewStaticMesh = NewObject<UStaticMeshComponent>(this);
		NewStaticMesh->SetupAttachment(RootComponent);
		NewStaticMesh->SetMobility(EComponentMobility::Movable);
		NewStaticMesh->SetStaticMesh(Mesh);
		NewStaticMesh->SetWorldLocation(LaneSpline->GetWorldLocationAtSplinePoint(i) + FVector(0.f,0.f,5.0f));
		FRotator Rotation = LaneSpline->GetRotationAtSplinePoint(i, ESplineCoordinateSpace::World);
		Rotation.Yaw += RoadDirection == -1 ? 180.0 : 0.0;
		NewStaticMesh->SetWorldRotation(Rotation);

		if (bIsJunction == true)
		{
			NewStaticMesh->SetWorldScale3D(NewStaticMesh->GetComponentScale() / 2);
		}
		else
		{
			NewStaticMesh->SetVisibility(false);
		}

		NewStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		NewStaticMesh->RegisterComponent();
		ArrowMeshes.Add(NewStaticMesh);
	}
}

void AOpenDriveEditorLane::SetColoredLaneMeshes(USplineComponent* LaneSpline)
{	
	for (int i = 0; i < LaneSpline->GetNumberOfSplinePoints() - 1; i++)
	{
		USplineMeshComponent* NewSplineMesh = NewObject<USplineMeshComponent>(this);
		NewSplineMesh->SetupAttachment(RootComponent);
		NewSplineMesh->SetMobility(EComponentMobility::Movable);
		NewSplineMesh->SetStaticMesh(LaneMeshPtr);

		switch (Lane->GetLaneType())
		{
		case(roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):
			NewSplineMesh->SetMaterial(0, Lane->GetId() > 0 ? LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/MI_LeftRoad")) : LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/MI_RightRoad")));
			break;

		case(roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK):
			NewSplineMesh->SetMaterial(0, LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/MI_Sidewalk")));
			break;

		case(roadmanager::Lane::LaneType::LANE_TYPE_SHOULDER):
			NewSplineMesh->SetMaterial(0, LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/MI_Shoulder")));
			break;
		
		case(roadmanager::Lane::LaneType::LANE_TYPE_PARKING):
			NewSplineMesh->SetMaterial(0, LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/MI_Parking")));
			break;
			
		case(roadmanager::Lane::LaneType::LANE_TYPE_BIKING):
			NewSplineMesh->SetMaterial(0, LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/MI_Biking")));
			break;

		case(roadmanager::Lane::LaneType::LANE_TYPE_RESTRICTED):
			NewSplineMesh->SetMaterial(0, LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/MI_RestrictedRoad")));
			break;

		default:
			NewSplineMesh->SetMaterial(0, LoadObject<UMaterial>(nullptr, TEXT("/OpenDRIVE/EditorRessources/Materials/M_LaneSplineEd")));
			break;
		}

		NewSplineMesh->SetForwardAxis(ESplineMeshAxis::X);

		// Start and end points (position and tangent)
		const FVector StartPoint = LaneSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector StartTangent = LaneSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector EndPoint = LaneSpline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		const FVector EndTangent = LaneSpline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		NewSplineMesh->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, false);

		// Start and end scale 
		const FVector StartScale = LaneSpline->GetScaleAtSplinePoint(i);
		const FVector EndScale = LaneSpline->GetScaleAtSplinePoint(i + 1);
		NewSplineMesh->SetStartScale(FVector2D(StartScale.Y, StartScale.Z), false);
		NewSplineMesh->SetEndScale(FVector2D(EndScale.Y, EndScale.Z), false);

		// Turn off all collision
		NewSplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Update spline mesh render and register it so it can be visible in editor viewport
		NewSplineMesh->UpdateRenderStateAndCollision();
		NewSplineMesh->RegisterComponent();
	}

	// To regain some performance, we can destroy the spline. Now that we have generated the meshes, spline points are no longer needed.
	LaneSpline->DestroyComponent();
}

void AOpenDriveEditorLane::SetArrowVisibility(const bool bIsVisible)
{
	if (GetJunctionId() == -1)
	{
		for (UStaticMeshComponent* Arrow : ArrowMeshes)
		{
			Arrow->SetVisibility(bIsVisible);
		}
	}
}
