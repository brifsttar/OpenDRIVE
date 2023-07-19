#include "PedestrianCrossingComponent.h"

UPedestrianCrossingComponent::UPedestrianCrossingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UPedestrianCrossingComponent::GetOppositeSidewalkPosition(UOpenDrivePosition* Odc, FSidewalksInfo& sidewalkInfo)
{
	Odc->SetTransform(GetOwner()->GetActorTransform());

	roadmanager::Position startPos = Odc->GetTrackPosition();
	roadmanager::LaneSection* laneSection = startPos.GetRoad()->GetLaneSectionByS(Odc->GetS());

	roadmanager::Lane* lane;
	roadmanager::Position targetPos;

	int roadId = Odc->GetRoadId();
	int currentLaneId = Odc->GetLaneId();
	sidewalkInfo.laneId1 = currentLaneId;

	laneSection->GetNumberOfLanes();

	if (currentLaneId > 0)
	{
		UE_LOG(LogClass, Warning, TEXT("going to right"));

		bool bStop = false;
		while (bStop == false)
		{
			currentLaneId--;
			lane = laneSection->GetLaneById(currentLaneId);

			if (lane == nullptr)
			{
				sidewalkInfo.position = FVector::ZeroVector;
				return;
			}
			else if (lane->GetLaneType() == roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK)
			{
				UE_LOG(LogClass, Warning, TEXT("found sidewalk"));

				targetPos.Init();
				targetPos.SetSnapLaneTypes(lane->GetLaneType());
				targetPos.SetLanePos(roadId, lane->GetId(), startPos.GetS(), 0.);
				sidewalkInfo.laneId2 = lane->GetId();
				sidewalkInfo.position = CoordTranslate::OdrToUe::ToLocation(targetPos);
				return;
			}
		}
	}
	else if (currentLaneId < 0)
	{
		UE_LOG(LogClass, Warning, TEXT("going to left"));

		bool bStop = false;
		while (bStop == false)
		{
			currentLaneId++;
			lane = laneSection->GetLaneById(currentLaneId);

			if (lane == nullptr)
			{
				sidewalkInfo.position = FVector::ZeroVector;
				return;
			}
			else if (lane->GetLaneType() == roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK)
			{
				UE_LOG(LogClass, Warning, TEXT("found sidewalk"));

				targetPos.Init();
				targetPos.SetSnapLaneTypes(lane->GetLaneType());
				targetPos.SetLanePos(roadId, lane->GetId(), startPos.GetS(), 0.);
				sidewalkInfo.laneId2 = lane->GetId();
				sidewalkInfo.position = CoordTranslate::OdrToUe::ToLocation(targetPos);
				return;
			}
		}
	}

	sidewalkInfo.position = FVector::ZeroVector;
	return;
}

void UPedestrianCrossingComponent::CreateTrajectoryToOppositeSidewalk(UOpenDrivePosition* Odc, FSidewalksInfo& sidewalkInfo, USplineComponent* spline)
{
	spline->ClearSplinePoints();

	spline->AddSplineWorldPoint(GetOwner()->GetActorLocation());

	GetOppositeSidewalkPosition(Odc, sidewalkInfo);

	spline->AddSplineWorldPoint(sidewalkInfo.position);
}

void UPedestrianCrossingComponent::AlterateTrajectory(USplineComponent* spline, TrajectoryType trajectoryType)
{
	if (spline->GetNumberOfSplinePoints() > 2) return;

	FVector start = spline->GetWorldLocationAtSplinePoint(0);
	FVector end = spline->GetWorldLocationAtSplinePoint(spline->GetNumberOfSplinePoints() - 1);

	spline->RemoveSplinePoint(spline->GetNumberOfSplinePoints() - 1);

	FVector middlePoint((start.X + end.X) / 2, (start.Y + end.Y) / 2, (start.Z + end.Z) / 2);
	FVector point1((start.X + middlePoint.X) / 2, (start.Y + middlePoint.Y) / 2, (start.Z + middlePoint.Z) / 2);
	FVector point2((end.X + middlePoint.X) / 2, (end.Y + middlePoint.Y) / 2, (end.Z + middlePoint.Z) / 2);

	switch (trajectoryType)
	{
	case(TrajectoryType::Clumsy):

		middlePoint = FVector(middlePoint.X - FMath::FRandRange(-100., 100.), middlePoint.Y - FMath::FRandRange(-100., 100.), middlePoint.Z);
		spline->AddSplineWorldPoint(middlePoint);
		spline->AddSplineWorldPoint(end);
		break;

	case(TrajectoryType::Drunk):

		middlePoint = FVector(middlePoint.X + FMath::FRandRange(-200., 200.), middlePoint.Y + FMath::FRandRange(-200., 200.), middlePoint.Z);
		point1 = FVector(point1.X + FMath::FRandRange(-200., 200.), point1.Y + FMath::FRandRange(-200., 200.), point1.Z);
		point2 = FVector(point2.X + FMath::FRandRange(-200., 200.), point2.Y + FMath::FRandRange(-200., 200.), point2.Z);

		spline->AddSplineWorldPoint(point1);
		spline->AddSplineWorldPoint(middlePoint);
		spline->AddSplineWorldPoint(point2);
		spline->AddSplineWorldPoint(end);
		break;
	}
}

	


