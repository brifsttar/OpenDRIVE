// Fill out your copyright notice in the Description page of Project Settings.
#include "OpenDriveRoadEd.h"

// Sets default values
AOpenDriveRoadEd::AOpenDriveRoadEd()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	LaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/OpenDRIVE/EditorRessources/LaneSplineEdMesh"));
	LaneMaterial = LoadObject<UMaterial>(nullptr, TEXT("/OpenDRIVE/EditorRessources/LaneSplineEdMaterial"));
	LeftMaterial = LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/MI_LeftRoad"));
	RightMaterial = LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/MI_RightRoad"));
	SidewalkMaterial = LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorRessources/MI_Sidewalk"));

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
	default:
		LaneType = "Any";
		break;
	}

	
	DrawLane();

	//DrawLanes(road_);
}

void AOpenDriveRoadEd::DrawLanes(roadmanager::Road* road)
{
	double roadLen = road->GetLength();
	double laneLength;

	roadmanager::Position position;
	roadmanager::Position prevPosition;
	roadmanager::LaneSection* laneSection;
	roadmanager::Lane* lane;

	FVector sp;
	float YScale;

	for (int i = 0; i < road->GetNumberOfLaneSections(); i++)
	{
		laneSection = road->GetLaneSectionByIdx(i);

		if (!laneSection) continue;

		laneLength = laneSection->GetLength();

		for (int j = 0; j < laneSection->GetNumberOfLanes(); j++)
		{
			lane = laneSection->GetLaneByIdx(j);

			if (!lane || lane->GetId() == 0) continue;

			USplineComponent* LaneSpline = NewObject<USplineComponent>(this);
			LaneSpline->SetupAttachment(RootComponent);
			LaneSpline->RegisterComponent();
			LaneSpline->ClearSplinePoints();

			//Start Point
			position.Init();
			position.SetSnapLaneTypes(lane->GetLaneType());
			position.SetLanePos(road->GetId(), lane->GetId(), 0., 0.);
			position.SetHeadingRelativeRoadDirection(0.);
			sp = CoordTranslate::OdrToUe::ToLocation(position);
			sp.Z = roadOffset;

			LaneSpline->AddSplineWorldPoint(sp);

			YScale = (laneSection->GetWidth(0., lane->GetId()) * 100) / baseMeshSize;
			LaneSpline->SetScaleAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, FVector(1.0f, YScale, 1.0f));

			prevPosition = position;

			for (int s = 5; s < laneLength; s += 5)
			{
				position.Init();
				position.SetSnapLaneTypes(lane->GetLaneType());
				position.SetLanePos(road->GetId(), lane->GetId(), s, 0.);
				position.SetHeadingRelativeRoadDirection(0.);

				if (FMath::Abs(prevPosition.GetH() - position.GetH()) > FMath::DegreesToRadians(0.))
				{
					sp = CoordTranslate::OdrToUe::ToLocation(position);
					sp.Z = roadOffset;
					LaneSpline->AddSplineWorldPoint(sp);
					YScale = (laneSection->GetWidth(s, lane->GetId()) * 100) / baseMeshSize;
					LaneSpline->SetScaleAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, FVector(1.0f, YScale, 1.0f));

					prevPosition = position;
				}
			}

			//Final point
			position.Init();
			position.SetSnapLaneTypes(lane->GetLaneType());
			position.SetLanePos(road->GetId(), lane->GetId(), laneLength, 0.);
			position.SetHeadingRelativeRoadDirection(0.);
			sp = CoordTranslate::OdrToUe::ToLocation(position);
			sp.Z += roadOffset;

			LaneSpline->AddSplineWorldPoint(sp);

			YScale = (laneSection->GetWidth(laneLength, lane->GetId()) * 100) / baseMeshSize;
			LaneSpline->SetScaleAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, FVector(1.0f, YScale, 1.0f));

			//Mesh init 
			for (int l = 0; l < LaneSpline->GetNumberOfSplinePoints() - 1; l++)
			{
				USplineMeshComponent* newSplineMesh = NewObject<USplineMeshComponent>(this);
				newSplineMesh->SetupAttachment(RootComponent);
				newSplineMesh->SetMobility(EComponentMobility::Movable);
				newSplineMesh->SetStaticMesh(LaneMesh);

				switch (lane->GetLaneType())
				{
				case(roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):

					if (lane->GetId() > 0)
					{
						newSplineMesh->SetMaterial(0, LeftMaterial);

					}
					else
					{
						newSplineMesh->SetMaterial(0, RightMaterial);
					}
					break;

				case(roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK):

					newSplineMesh->SetMaterial(0, SidewalkMaterial);
					break;
				}

				newSplineMesh->SetForwardAxis(ESplineMeshAxis::X);

				const FVector StartPoint = LaneSpline->GetLocationAtSplinePoint(l, ESplineCoordinateSpace::Local);
				const FVector StartTangent = LaneSpline->GetTangentAtSplinePoint(l, ESplineCoordinateSpace::Local);
				const FVector EndPoint = LaneSpline->GetLocationAtSplinePoint(l + 1, ESplineCoordinateSpace::Local);
				const FVector EndTangent = LaneSpline->GetTangentAtSplinePoint(l + 1, ESplineCoordinateSpace::Local);

				newSplineMesh->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, false);

				const FVector StartScale = LaneSpline->GetScaleAtSplinePoint(l);
				const FVector EndScale = LaneSpline->GetScaleAtSplinePoint(l + 1);

				newSplineMesh->SetStartScale(FVector2D(StartScale.Y, StartScale.Z), false);
				newSplineMesh->SetEndScale(FVector2D(EndScale.Y, EndScale.Z), false);

				newSplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				newSplineMesh->UpdateRenderStateAndCollision();

				newSplineMesh->RegisterComponent();
			}

			LaneSpline->DestroyComponent();
		}
	}
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
	for (int i = 0; i < LaneSpline->GetNumberOfSplinePoints() - 1; i++)
	{
		USplineMeshComponent* newSplineMesh = NewObject<USplineMeshComponent>(this);
		newSplineMesh->SetupAttachment(RootComponent);
		newSplineMesh->SetMobility(EComponentMobility::Movable);
		newSplineMesh->SetStaticMesh(LaneMesh);

		switch (Lane->GetLaneType())
		{
		case(roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):

			if (Lane->GetId() > 0)
			{
				newSplineMesh->SetMaterial(0, LeftMaterial);

			}
			else
			{
				newSplineMesh->SetMaterial(0, RightMaterial);
			}
			break;

		case(roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK):

			newSplineMesh->SetMaterial(0, SidewalkMaterial);
			break;
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



