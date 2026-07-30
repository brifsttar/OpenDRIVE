// Fill out your copyright notice in the Description page of Project Settings.


#include "SideWalkPCGSpawner.h"
#include "Components/BoxComponent.h"
#include "CoordTranslate.h"
#include "PCGComponent.h"
#include "PCGSubsystem.h"
#include "Data/PCGPointData.h"
#include "DrawDebugHelpers.h"
#include "PCGPoint.h"
#include "RoadManager.hpp"

ASideWalkPCGSpawner::ASideWalkPCGSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;

	PCGComponent = CreateDefaultSubobject<UPCGComponent>(TEXT("PCGComponent"));
	PCGComponent->bActivated = false;
}

void ASideWalkPCGSpawner::SpawnPropsFromOpenDrive()
{
	ClearProps();
	UE_LOG(LogTemp, Warning, TEXT("=== Début du Spawner ==="));

	if (!PCGComponent || !BoxComponent) return;

	roadmanager::OpenDrive* odr = roadmanager::Position::GetOpenDrive();
	if (!odr)
	{
		UE_LOG(LogTemp, Error, TEXT("OpenDrive Instance Not Found"));
		return;
	}

	roadmanager::Road* road = nullptr;
	int roadCount = odr->GetNumOfRoads();
	if (roadCount == 0) return;

	double roadLen = 0.;
	float FixedStep = CoordTranslate::UuToMeters(AverageSpacing);
	if (FixedStep <= 0.1f) FixedStep = 1.0f;

	GeneratedPCGPoints.Empty();

	FBox BoxMonde = BoxComponent->CalcBounds(BoxComponent->GetComponentTransform()).GetBox();

	FBox SafeBoxMonde = BoxMonde.ExpandBy(-MarginBoxOffset);

	FVector BoxExtent = BoxComponent->GetUnscaledBoxExtent();

	float StartOffsetMeters = 5.0f;
	float EndOffsetMeters = 5.0f;

	for (int i = 0; i < roadCount; i++)
	{
		road = odr->GetRoadByIdx(i);
		roadLen = road->GetLength();

		float NextTreeDistance = 0.0f;
		float NextLampDistance = 0.0f;
		float NextBenchDistance = 0.0f;

		for (double j = StartOffsetMeters; j < (roadLen - EndOffsetMeters); j += FixedStep)
		{
			roadmanager::LaneSection* laneSection = road->GetLaneSectionByS(j);
			if (!laneSection) continue;

			for (unsigned int laneIdx = 0; laneIdx < laneSection->GetNumberOfLanes(); laneIdx++)
			{
				roadmanager::Lane* lane = laneSection->GetLaneByIdx(laneIdx);
				if (!lane) continue;

				if (lane->GetLaneType() != roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK) continue;

				int laneId = lane->GetId();
				float sidewalkWidth = road->GetWidth(j, laneId);

				roadmanager::Position p;
				p.SetLanePos(road->GetId(), laneId, j, 0.0);
				FTransform tf = CoordTranslate::OdrToUe::ToTransfrom(p);

				if (!SafeBoxMonde.IsInside(tf.GetLocation())) continue;

				float Heading = p.GetH();
				float NormalizedHeading = FMath::Fmod(Heading + PI, 2 * PI) / (2 * PI);
				float NormalizedWidth = FMath::Clamp(sidewalkWidth / 10.0f, 0.0f, 1.0f);
				if (j >= NextTreeDistance && TreeSpacing > 0.0f)
				{
					FPCGPoint TreePoint;
					TreePoint.Transform = tf;
					TreePoint.Transform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
					TreePoint.Density = 1.0f;
					TreePoint.Seed = FMath::Rand();
					TreePoint.Color = FVector4(NormalizedWidth, NormalizedHeading, 0.0f, 1.0f);
					TreePoint.BoundsMin = FVector(-50.0f, -50.0f, -50.0f);
					TreePoint.BoundsMax = FVector(50.0f, 50.0f, 50.0f);

					FVector LocalPos = BoxComponent->GetComponentTransform().InverseTransformPosition(TreePoint.Transform.GetLocation());

					if (FMath::Abs(LocalPos.X) <= BoxExtent.X &&
						FMath::Abs(LocalPos.Y) <= BoxExtent.Y &&
						FMath::Abs(LocalPos.Z) <= BoxExtent.Z)
					{
						GeneratedPCGPoints.Add(TreePoint);
					}
					NextTreeDistance = j + CoordTranslate::UuToMeters(TreeSpacing);
				}
				else if (j >= NextLampDistance && LampSpacing > 0.0f)
				{
					roadmanager::Position LampPos;
					LampPos.SetLanePos(road->GetId(), laneId, j, CoordTranslate::UuToMeters(LampLateralOffset));
					FTransform LampTransform = CoordTranslate::OdrToUe::ToTransfrom(LampPos);

					FRotator LampRot = LampTransform.GetRotation().Rotator();
					if (laneId < 0)
					{
						LampRot.Yaw -= 90.0f;
					}
					LampTransform.SetRotation(FQuat(LampRot));

					FPCGPoint LampPoint;
					LampPoint.Transform = LampTransform;
					LampPoint.Transform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
					LampPoint.Density = 1.0f;
					LampPoint.Seed = FMath::Rand();
					LampPoint.Color = FVector4(NormalizedWidth, NormalizedHeading, 0.5f, 1.0f);
					LampPoint.BoundsMin = FVector(-50.0f, -50.0f, -50.0f);
					LampPoint.BoundsMax = FVector(50.0f, 50.0f, 50.0f);

					FVector LocalPos = BoxComponent->GetComponentTransform().InverseTransformPosition(LampPoint.Transform.GetLocation());

					if (FMath::Abs(LocalPos.X) <= BoxExtent.X &&
						FMath::Abs(LocalPos.Y) <= BoxExtent.Y &&
						FMath::Abs(LocalPos.Z) <= BoxExtent.Z)
					{
						GeneratedPCGPoints.Add(LampPoint);
					}
					NextLampDistance = j + CoordTranslate::UuToMeters(LampSpacing);
				}
				else if (j >= NextBenchDistance && BenchSpacing > 0.0f)
				{
					FPCGPoint BenchPoint;
					BenchPoint.Transform = tf;
					BenchPoint.Transform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
					BenchPoint.Density = 1.0f;
					BenchPoint.Seed = FMath::Rand();
					BenchPoint.Color = FVector4(NormalizedWidth, NormalizedHeading, 1.0f, 1.0f);
					BenchPoint.BoundsMin = FVector(-50.0f, -50.0f, -50.0f);
					BenchPoint.BoundsMax = FVector(50.0f, 50.0f, 50.0f);

					FVector LocalPos = BoxComponent->GetComponentTransform().InverseTransformPosition(BenchPoint.Transform.GetLocation());

					if (FMath::Abs(LocalPos.X) <= BoxExtent.X &&
						FMath::Abs(LocalPos.Y) <= BoxExtent.Y &&
						FMath::Abs(LocalPos.Z) <= BoxExtent.Z)
					{
						GeneratedPCGPoints.Add(BenchPoint);
					}
					NextBenchDistance = j + CoordTranslate::UuToMeters(BenchSpacing);
				}
			}
		}
	}

	if (GeneratedPCGPoints.Num() > 0)
	{
		this->Modify();
		PCGComponent->bActivated = true;
		PCGComponent->Generate(true);
		UE_LOG(LogTemp, Warning, TEXT("=== FIN SUCCESS === PCG mis a jour sur les trottoirs avec %d points !"), GeneratedPCGPoints.Num());
	}
}

void ASideWalkPCGSpawner::ClearProps()
{
	if (PCGComponent && PCGComponent->bActivated)
	{
		PCGComponent->Cleanup();
	}
}


void ASideWalkPCGSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void ASideWalkPCGSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

