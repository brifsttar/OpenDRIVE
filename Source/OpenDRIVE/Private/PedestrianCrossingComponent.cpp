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


	


