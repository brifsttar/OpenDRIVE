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

void AOpenDriveRoadEd::Initialize(float roadOffset_)
{
	roadOffset = roadOffset_;
	roadmanager::OpenDrive* Odr = roadmanager::Position::GetOpenDrive();
	roadmanager::Road* road = 0;
	size_t nrOfRoads = Odr->GetNumOfRoads();

	for (int i = 0; i < (int)nrOfRoads; i++)
	{
		road = Odr->GetRoadByIdx(i);
		if (!road) continue;

		DrawLanes(road);
	}

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
			double t = 0.;
			position.Init();
			position.SetSnapLaneTypes(lane->GetLaneType());
			position.SetLanePos(road->GetId(), lane->GetId(), 0, t);
			position.SetHeadingRelativeRoadDirection(0.);
			sp = CoordTranslate::OdrToUe::ToLocation(position);
			sp.Z = roadOffset;

			LaneSpline->AddSplineWorldPoint(sp);

			YScale = (laneSection->GetWidth(laneLength, lane->GetId()) * 100) / baseMeshSize;
			LaneSpline->SetScaleAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, FVector(1.0f, YScale, 1.0f));

			prevPosition = position;

			for (int s = 3; s < laneLength; s += 3)
			{
				position.Init();
				position.SetSnapLaneTypes(lane->GetLaneType());
				position.SetLanePos(road->GetId(), lane->GetId(), s, t);
				position.SetHeadingRelativeRoadDirection(0.);

				sp = CoordTranslate::OdrToUe::ToLocation(position);
				sp.Z = roadOffset;

				if (FMath::Abs(position.GetH() - prevPosition.GetH()) > FMath::DegreesToRadians(2.))
				{
					LaneSpline->AddSplineWorldPoint(sp);

					YScale = (laneSection->GetWidth(laneLength, lane->GetId()) * 100) / baseMeshSize;
					LaneSpline->SetScaleAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, FVector(1.0f, YScale, 1.0f));

					prevPosition = position;
				}
			}

			//Final point
			position.Init();
			position.SetSnapLaneTypes(lane->GetLaneType());
			position.SetLanePos(road->GetId(), lane->GetId(), laneLength, t);
			position.SetHeadingRelativeRoadDirection(0.);
			sp = CoordTranslate::OdrToUe::ToLocation(position);
			sp.Z += roadOffset;

			LaneSpline->AddSplineWorldPoint(sp);

			YScale = (laneSection->GetWidth(laneLength, lane->GetId()) * 100) / baseMeshSize;
			LaneSpline->SetScaleAtSplinePoint(LaneSpline->GetNumberOfSplinePoints() - 1, FVector(1.0f, YScale, 1.0f));

			for (int k = 0; k < LaneSpline->GetNumberOfSplinePoints(); k++)
			{
				LaneSpline->SetSplinePointType(i, ESplinePointType::Curve);
			}

			//Mesh init 
			for (int l = 0; l < LaneSpline->GetNumberOfSplinePoints() - 1; l++)
			{
				USplineMeshComponent* newSplineMesh = NewObject<USplineMeshComponent>(this);
				newSplineMesh->SetupAttachment(RootComponent);
				newSplineMesh->SetMobility(EComponentMobility::Movable);
				newSplineMesh->SetStaticMesh(LaneMesh);

				switch (lane->GetLaneType())
				{
				case(roadmanager::Lane::LaneType::LANE_TYPE_DRIVING) :

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

				LanesMeshesArray.Add(newSplineMesh);
			}

			LaneSpline->DestroyComponent();
		}
	}
}


