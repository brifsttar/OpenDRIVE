#include "OpenDriveCrossingComponent.h"

UOpenDriveCrossingComponent::UOpenDriveCrossingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UOpenDriveCrossingComponent::CreatePathToOppositeSidewalk(UOpenDrivePosition* Odp, float endSOffset, ECrossingPathType pathType, TArray<FVector>& positions)
{
	Odp->SetTransform(GetOwner()->GetActorTransform());

	roadmanager::Position startPos = Odp->GetTrackPosition();
	positions.Add(CoordTranslate::OdrToUe::ToLocation(startPos));

	roadmanager::LaneSection* laneSection = startPos.GetRoad()->GetLaneSectionByS(Odp->GetS());
	roadmanager::Lane* lane;
	roadmanager::Position targetPos;

	int roadId = Odp->GetRoadId();
	int currentLaneId = Odp->GetLaneId();
	int oppositeLaneId = currentLaneId;

	int direction = 1;
	if (currentLaneId > 0)
	{
		direction = -1;
	}

	bool bStop = false;
	while (bStop == false)
	{
		oppositeLaneId += direction;
		lane = laneSection->GetLaneById(oppositeLaneId);

		if (lane == nullptr)
		{
			break;
		}
		else if (lane->GetLaneType() == roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK)
		{
			switch (pathType)
			{
			case(ECrossingPathType::SIMPLE):
				targetPos.Init();
				targetPos.SetSnapLaneTypes(lane->GetLaneType());
				targetPos.SetLanePos(roadId, lane->GetId(), startPos.GetS(), 0.);
				positions.Add(CoordTranslate::OdrToUe::ToLocation(targetPos));
				targetPos.MoveAlongS(endSOffset);
				positions.Add(CoordTranslate::OdrToUe::ToLocation(targetPos));
				break;

			case(ECrossingPathType::STRAIGHT):
				targetPos.Init();
				targetPos.SetSnapLaneTypes(lane->GetLaneType());
				targetPos.SetLanePos(roadId, lane->GetId(), startPos.GetS(), 0.);
				targetPos.MoveAlongS(endSOffset);
				positions.Add(CoordTranslate::OdrToUe::ToLocation(targetPos));
				break;

			case(ECrossingPathType::SIDEWALK):
				if (FindNearCrosswalk(roadId, currentLaneId, oppositeLaneId, 5000.f, positions) == false)
				{
					targetPos.Init();
					targetPos.SetSnapLaneTypes(lane->GetLaneType());
					targetPos.SetLanePos(roadId, lane->GetId(), startPos.GetS(), 0.);
					targetPos.MoveAlongS(endSOffset);
					positions.Add(CoordTranslate::OdrToUe::ToLocation(targetPos));
				}
				break;
			}
			break;
		}
	}
}

bool UOpenDriveCrossingComponent::FindNearCrosswalk(int roadID, int currentlaneId, int targetlaneId, float SearchAreaRadius, TArray<FVector>& crosswalkPath)
{
	FCollisionShape sphere = FCollisionShape::MakeSphere(SearchAreaRadius * 100);

	TArray<FHitResult> Out;

	FVector location = GetOwner()->GetActorLocation();

	bool bhit = GetWorld()->SweepMultiByChannel(Out, location, location, FQuat::Identity, ECC_WorldDynamic, sphere);

	if (bhit == true)
	{
		for (FHitResult& hit : Out)
		{
			AOpenDriveCrosswalk* crosswalk = Cast<AOpenDriveCrosswalk>(hit.GetActor());

			if (crosswalk != nullptr)
			{
				if (crosswalk->GetRoadId() == roadID)
				{
					FVector* p = crosswalk->GetPositionOnSidewalkId(currentlaneId);
					if (p != nullptr)
					{
						crosswalkPath.Add(*p);
					}
					p = crosswalk->GetPositionOnSidewalkId(targetlaneId);
					if (p != nullptr)
					{
						crosswalkPath.Add(*p);
					}
					return true;
				}
			}
		}
		return false;
	}
	else
	{
		return false;
	}
}



	

