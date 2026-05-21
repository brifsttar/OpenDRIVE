#include "OpenDriveEditorLane.h"
#include "CoordTranslate.h"
#include "Components/SplineMeshComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "ProceduralMeshComponent.h"

namespace OpenDriveLaneDebugHelper
{
	UMaterialInterface* GetInstancedMaterialForMesh(const roadmanager::Lane* Lane)
	{
		switch (Lane->GetLaneType())
		{
		case(roadmanager::Lane::LaneType::LANE_TYPE_DRIVING):
			if (Lane->GetId() > 0)
			{
				return LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorResources/Materials/MI_LeftRoad"));
			}
			return LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorResources/Materials/MI_RightRoad"));
		case(roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK):
			return LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorResources/Materials/MI_Sidewalk"));
		case(roadmanager::Lane::LaneType::LANE_TYPE_SHOULDER):
			return LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorResources/Materials/MI_Shoulder"));
		case(roadmanager::Lane::LaneType::LANE_TYPE_PARKING):
			return LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorResources/Materials/MI_Parking"));
		case(roadmanager::Lane::LaneType::LANE_TYPE_BIKING):
			return LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorResources/Materials/MI_Biking"));
		case(roadmanager::Lane::LaneType::LANE_TYPE_RESTRICTED):
			return LoadObject<UMaterialInstance>(nullptr, TEXT("/OpenDRIVE/EditorResources/Materials/MI_RestrictedRoad"));
		default:
			return nullptr;
		}
	}
}

// Sets default values
AOpenDriveEditorLane::AOpenDriveEditorLane(): Road(nullptr), LaneSection(nullptr), Lane(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
	bEditable = true;
	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	RootComponent = RootComp;
	RootComponent->SetMobility(EComponentMobility::Static);
	bIsSelectableInEditor = true;
	bIsEditorOnlyActor = true;
}

void AOpenDriveEditorLane::Initialize(roadmanager::Road* RoadIn, roadmanager::LaneSection* LaneSectionIn, roadmanager::Lane* LaneIn, const float Offset, const float Step)
{
	Road = RoadIn;
	LaneSection = LaneSectionIn;
	Lane = LaneIn;

	roadmanager::Position Pos;
	Pos.SetLanePos(Road->GetId(), Lane->GetId(), LaneSection->GetS(), 0.f);
	FTransform LaneTransform = CoordTranslate::OdrToUe::ToTransfrom(Pos);
	SetActorTransform(LaneTransform);

	BuildLaneMesh(Step, Offset);
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

void AOpenDriveEditorLane::BuildLaneMesh(float SampleStepMeters, float ZOffset)
{
	if (!Lane || !LaneSection || !Road)
	{
		return;
	}

	UProceduralMeshComponent* Mesh = NewObject<UProceduralMeshComponent>(this);

	Mesh->RegisterComponent();
	Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;

	double sStart = LaneSection->GetS();
	double sEnd = LaneSection->GetS() + LaneSection->GetLength();

	int32 VertexIndex = 0;

	for (double s = sStart; s < sEnd; s += SampleStepMeters)
	{
		double Width = LaneSection->GetWidth(s, Lane->GetId());

		roadmanager::Position LeftPos;
		LeftPos.SetLanePos(GetRoadId(), GetLaneId(), s, -Width * 0.5f);

		roadmanager::Position RightPos;
		RightPos.SetLanePos(GetRoadId(), GetLaneId(), s, Width * 0.5f);

		// Add vertices for left and right edges of the lane

		FVector LeftVertex = Mesh->GetOwner()->GetActorTransform()
			.InverseTransformPosition(CoordTranslate::OdrToUe::ToLocation(LeftPos) + FVector(0.0f,0.0f,ZOffset));
		FVector RightVertex = Mesh->GetOwner()->GetActorTransform()
			.InverseTransformPosition(CoordTranslate::OdrToUe::ToLocation(RightPos) + FVector(0.0f,0.0f,ZOffset));

		Vertices.Add(LeftVertex);
		Vertices.Add(RightVertex);

		// Add UVs for texturing

		float V = (s - sStart) * 0.2f;

		UVs.Add(FVector2D(0.0f, V));
		UVs.Add(FVector2D(1.0f, V));

		// Add Normals and Tangents

		FVector Forward = (RightVertex - LeftVertex).GetSafeNormal();
		FVector Normal = FVector::UpVector;
		FVector Tangent = FVector::CrossProduct(Normal, Forward).GetSafeNormal();

		Normals.Add(Normal);
		Normals.Add(Normal);

		Tangents.Add(FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));
		Tangents.Add(FProcMeshTangent(Tangent.X, Tangent.Y, Tangent.Z));

		// Add triangles 

		if (VertexIndex > 0)
		{
			int32 i0 = VertexIndex - 2;
			int32 i1 = VertexIndex - 1;
			int32 i2 = VertexIndex + 0;
			int32 i3 = VertexIndex + 1;

			// Triangle 1
			Triangles.Add(i0);
			Triangles.Add(i2);
			Triangles.Add(i1);

			// Triangle 2
			Triangles.Add(i1);
			Triangles.Add(i2);
			Triangles.Add(i3);
		}

		VertexIndex += 2;
	}

	Mesh->CreateMeshSection(
		0,
		Vertices,
		Triangles,
		Normals,
		UVs,
		TArray<FColor>(),
		Tangents,
		true
	);

	Mesh->ContainsPhysicsTriMeshData(false);

	UMaterialInterface* Material = OpenDriveLaneDebugHelper::GetInstancedMaterialForMesh(Lane);
	if (!Material)
	{
		Destroy();
		return;
	}

	UMaterialInstanceConstant* MaterialConst = NewObject<UMaterialInstanceConstant>(this, UMaterialInstanceConstant::StaticClass(), NAME_None, RF_Transient);
	MaterialConst->SetParentEditorOnly(Material);
	Mesh->SetMaterial(0, MaterialConst);

	return;
}
