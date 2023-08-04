#include "OpenDriveCrosswalk.h"

// Sets default values
AOpenDriveCrosswalk::AOpenDriveCrosswalk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	UDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("CrosswalkDecal"));
	SetRootComponent(UDecal);

	_openDriveComponent = CreateDefaultSubobject<UOpenDriveComponent>(TEXT("OpenDriveComponent"));
	_openDriveComponent->SetupAttachment(RootComponent);
}

void AOpenDriveCrosswalk::SetCrosswalkBorderPositions()
{
	TSides.Empty();
	SetActorRotation(FRotator::ZeroRotator);

	UOpenDrivePosition* openDrivePosition = _openDriveComponent->GetTrackPosition();
	openDrivePosition->SetTransform(GetActorTransform());
	_roadId = openDrivePosition->GetRoadId();
	
	int currentLaneId = 0;

	roadmanager::Position position = openDrivePosition->GetTrackPosition();
	position.SetLanePos(_roadId, 0, position.GetS(), 0.);
	position.SetHeadingRelativeRoadDirection(0.);
	SetActorTransform(CoordTranslate::OdrToUe::ToTransfrom(position));
	AddActorLocalRotation(FQuat::MakeFromEuler(FVector(90.f, -90.f, 0.f)));

	roadmanager::LaneSection* laneSection = position.GetRoad()->GetLaneSectionByS(openDrivePosition->GetS());
	roadmanager::Lane* lane;
	
	while (true)
	{
		currentLaneId--;

		lane = laneSection->GetLaneById(currentLaneId);

		if (lane == nullptr)
		{
			break;
		}
		else if (lane->GetLaneType() == roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK)
		{
			roadmanager::Position newPos;
			newPos.Init();
			newPos.SetSnapLaneTypes(lane->GetLaneType());
			newPos.SetLanePos(_roadId, lane->GetId(), position.GetS(), 0.);
			newPos.SetOffset(0.);
			newPos.MoveAlongS(0.);
			_side1 = CoordTranslate::OdrToUe::ToLocation(newPos);
			TSides.Add(lane->GetId(), _side1);
			break;
		}
	}
	currentLaneId = 0;
	while (true)
	{
		currentLaneId++;

		lane = laneSection->GetLaneById(currentLaneId);

		if (lane == nullptr)
		{
			break;
		}
		else if (lane->GetLaneType() == roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK)
		{
			roadmanager::Position newPos;
			newPos.Init();
			newPos.SetSnapLaneTypes(lane->GetLaneType());
			newPos.SetLanePos(_roadId, lane->GetId(), position.GetS(), 0.);
			newPos.SetOffset(0.);
			newPos.MoveAlongS(0.);
			_side2 = CoordTranslate::OdrToUe::ToLocation(newPos);
			TSides.Add(lane->GetId(), _side2);
			break;
		}
	}

	UDecal->DecalSize.Z = (FVector::Distance(_side1, _side2) / 3);
}





