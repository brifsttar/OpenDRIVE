
#include "RoadDecalSpawner.h"
#include "Engine/DecalActor.h"
#include "Components/BoxComponent.h"
#include "CoordTranslate.h"
#include "RoadManager.hpp"

ARoadDecalSpawner::ARoadDecalSpawner() {
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
}

void ARoadDecalSpawner::BeginPlay() {
	Super::BeginPlay();
	
}

void ARoadDecalSpawner::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ARoadDecalSpawner::SpawnDecals() {
	ClearDecals();
	roadmanager::OpenDrive* odr = roadmanager::Position::GetOpenDrive();
	roadmanager::Road* road = nullptr;
	int matCount = Materials.Num();
	ADecalActor* decal = nullptr;
	int roadCount = odr->GetNumOfRoads();
	double roadLen = 0.;
	float sRandMax = CoordTranslate::UuToMeters(SAverageSpacing);
	float tOffset = CoordTranslate::UuToMeters(TMaxOffset);

	FBox box = BoxComponent->CalcBounds(GetTransform()).GetBox();

	for (int i = 0; i < roadCount; i++) {
		road = odr->GetRoadByIdx(i);
		roadLen = road->GetLength();
		for (float j = 0; j < roadLen; j += FMath::RandRange(0.f, sRandMax)) {
			float widthRight = road->GetWidth(j, -1, roadmanager::Lane::LaneType::LANE_TYPE_ANY_DRIVING);
			float widthLeft  = road->GetWidth(j,  1, roadmanager::Lane::LaneType::LANE_TYPE_ANY_DRIVING);
			float t = FMath::RandRange(-widthRight + tOffset, widthLeft - tOffset);

			roadmanager::Position p;
			p.SetSnapLaneTypes(roadmanager::Lane::LaneType::LANE_TYPE_ANY);
			p.SetTrackPos(road->GetId(), j, t);
			p.SetH(FMath::RandRange(-PI, PI));

			// For roundabouts with inner sidewalks
			roadmanager::Lane* lane = p.GetLane();
			if (!lane) continue;
			if (!(lane->GetLaneType() & roadmanager::Lane::LaneType::LANE_TYPE_ANY_DRIVING)) continue;
			FTransform tf = CoordTranslate::OdrToUe::ToTransfrom(p);
			if (!box.IsInside(tf.GetLocation())) continue;
			tf.SetScale3D(FVector(FMath::RandRange(ScaleRangeMin, ScaleRangeMax)));
			decal = GetWorld()->SpawnActor<ADecalActor>(ADecalActor::StaticClass(), tf);
#if WITH_EDITOR
			// Renaming/moving
			FString path = FString::Printf(TEXT("RoadDecals/road_%i"), road->GetId());
			decal->SetFolderPath(FName(*path));
			FString label = FString::Printf(TEXT("s%i_t%i"), (int)j, (int)t);
			decal->SetActorLabel(label);
#endif
			SpawnedDecals.Add(decal);
			if (matCount != 0) {
				int matIdx = FMath::RandHelper(matCount);
				decal->SetDecalMaterial(Materials[matIdx]);
			}
		}
	}
}

void ARoadDecalSpawner::ClearDecals() {
	for (auto decal : SpawnedDecals) {
		if (!IsValid(decal)) continue;
		decal->Destroy();
	}
	SpawnedDecals.Empty();
}

