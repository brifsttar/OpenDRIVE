// Fill out your copyright notice in the Description page of Project Settings.


#include "JunctionSignSpawner.h"

#include "RoadManager.hpp"
#include "OpenDriveComponent.h"
#include "Components/BillboardComponent.h"
#include "EngineUtils.h"
#include "CoordTranslate.h"

#include <set>

using CoordTranslate::UuToMeters;

// Sets default values
AJunctionSignSpawner::AJunctionSignSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		// A helper class object we use to find target UTexture2D object in resource package
		ConstructorHelpers::FObjectFinder<UTexture2D> Texture0;
		FConstructorStatics()
			: Texture0(TEXT("Texture2D'/Engine/EditorResources/Waypoint'"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	SpriteTexture = ConstructorStatics.Texture0.Object;
	BillboardComponent->Sprite = SpriteTexture;
	RootComponent = BillboardComponent;

	OpenDrive = CreateDefaultSubobject<UOpenDriveComponent>(TEXT("OpenDRIVE"));
	OpenDrive->SetupAttachment(RootComponent);
}

void AJunctionSignSpawner::SpawnJunctionSigns() {
	if (!JunctionSignClass) return;

	// Find all roads connected to the junction
	std::set<roadmanager::Road*> trafficLightRoads;
	roadmanager::Position p = OpenDrive->OdrPosition();
	if (!p.GetRoad()) return;
	int junctionId = p.GetRoad()->GetJunction();
	roadmanager::Junction *j = p.GetOpenDrive()->GetJunctionById(junctionId);
	if (!j)  return;
	for (unsigned int i = 0; i < j->GetNumberOfConnections(); i++)
	{
		roadmanager::Connection* c = j->GetConnectionByIdx(i);
		roadmanager::Road* r = c->GetIncomingRoad();
		trafficLightRoads.insert(r);
	}

	if (bClearExisting) {
		for (auto &t : JunctionSigns) {
			if (IsValid(t)) t->Destroy();
		}
		JunctionSigns.Empty();
	}

	for (auto& r : trafficLightRoads) {
		int side = 0;
		double s = 0.;
		roadmanager::RoadLink* pred = r->GetLink(roadmanager::LinkType::PREDECESSOR);
		roadmanager::RoadLink* succ = r->GetLink(roadmanager::LinkType::SUCCESSOR);

		// Find link type
		if (pred && pred->GetElementId() == junctionId) {
			side = 1;
		} else if (succ && succ->GetElementId() == junctionId) {
			side = -1;
			s = r->GetLength();
		}

		// Find sidewalk
		roadmanager::LaneSection* ls = r->GetLaneSectionByS(s);
		int laneId = side;
		int laneCount = side == 1 ? ls->GetNUmberOfLanesLeft() : ls->GetNUmberOfLanesRight();
		while (abs(laneId) <= laneCount) {
			roadmanager::Lane *l = ls->GetLaneById(laneId);
			if (l->GetLaneType() == roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK) {
				break;
			}
			laneId += side;
		}

		// Spawn sign on the sidewalk with given offsets
		roadmanager::Position tp(r->GetId(), laneId, s, UuToMeters(side * TOffset));
		double h_offset = FMath::DegreesToRadians(HOffset);
		if (SOffset != 0.) tp.MoveAlongS(UuToMeters(side * SOffset));
		tp.SetHeadingRelativeRoadDirection(tp.Side() > 0 ? -h_offset : M_PI - h_offset);
		FTransform t = CoordTranslate::OdrToUe::ToTransfrom(tp);
		AActor *junctionSign = GetWorld()->SpawnActor<AActor>(JunctionSignClass, t);
		if (!junctionSign) continue;
#if WITH_EDITOR
		// Renaming/moving
		FString path = FString::Printf(TEXT("Junction_%i/"), junctionId) + JunctionSignClass->GetFName().ToString();
		junctionSign->SetFolderPath(FName(*path));
		FString label = FString::Printf(TEXT("j%i_r%i"), junctionId, r->GetId());
		junctionSign->SetActorLabel(label);
#endif
		JunctionSigns.Add(junctionSign);
	}
}
