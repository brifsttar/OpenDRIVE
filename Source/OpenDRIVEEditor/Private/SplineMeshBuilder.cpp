// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineMeshBuilder.h"


void USplineMeshBuilder::Rebuild()
{
	ClearCurrentMeshes();
	AddMeshesToOwner();
}

void USplineMeshBuilder::CalcAndCacheBounds() const
{
}

void USplineMeshBuilder::ClearCurrentMeshes()
{
	for (auto Component : SplineMeshComponents)
	{
		FNavigationSystem::OnComponentUnregistered(*Component);
		GetOwner()->RemoveOwnedComponent(Component);
		Component->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		Component->UnregisterComponent();
		Component->DestroyComponent();
	}
	SplineMeshComponents.Empty();
	NavAreas.Empty();
}

USplineMeshComponent* USplineMeshBuilder::CreateMeshComponent(FVector StartPos, FVector StartTangent, FVector EndPos, FVector EndTangent)
{
	FName Name = MakeUniqueObjectName(GetOwner(), USplineMeshComponent::StaticClass());
	USplineMeshComponent* MeshComponent = NewObject<USplineMeshComponent>(this, Name);
	MeshComponent->SetStaticMesh(Mesh);
	MeshComponent->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
	MeshComponent->SetCollisionProfileName(CollisionProfile.Name);
	MeshComponent->SetMobility(EComponentMobility::Movable);
	MeshComponent->CreationMethod = EComponentCreationMethod::Instance;
	//MeshComponent->SetPhysMaterialOverride(PhysicalMaterial);

	return MeshComponent;
}

void USplineMeshBuilder::AddMeshesToOwner()
{
	if (!Mesh || !Spline)
	{
		return;
	}
	float SectionLength = Mesh->GetBounds().BoxExtent.X * 2.0f;
	float SectionHalfWidth = Mesh->GetBounds().BoxExtent.Y * 0.75f;
	float SplineLength = Spline->GetSplineLength();
	int32 NumSegments = SplineLength / SectionLength;

	Bounds.Init();

	for (int32 Index = 0; Index < NumSegments; ++Index)
	{
		float Distance = SectionLength * Index;

		FVector StartPos = Spline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		FVector StartTangent = Spline->GetTangentAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		StartTangent = StartTangent.GetClampedToMaxSize(SectionLength);
		FVector EndPos = Spline->GetLocationAtDistanceAlongSpline(Distance + SectionLength, ESplineCoordinateSpace::World);
		FVector EndTangent = Spline->GetTangentAtDistanceAlongSpline(Distance + SectionLength, ESplineCoordinateSpace::World);
		EndTangent = EndTangent.GetClampedToMaxSize(SectionLength);

		USplineMeshComponent* MeshComponent = CreateMeshComponent(StartPos, StartTangent, EndPos, EndTangent);

		GetOwner()->AddOwnedComponent(MeshComponent);
		MeshComponent->RegisterComponent();

		SplineMeshComponents.Add(MeshComponent);

		if (AddNavAreas)
		{

			FVector StartRight = Spline->GetRightVectorAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
			FVector EndRight = Spline->GetRightVectorAtDistanceAlongSpline(Distance + SectionLength, ESplineCoordinateSpace::World);

			FVector ZOffset(0.0f, 0.0f, 200.0f);

			TArray<FVector> Points;
			Points.Add(StartPos + SectionHalfWidth * StartRight);
			Points.Add(StartPos - SectionHalfWidth * StartRight);
			Points.Add(StartPos + SectionHalfWidth * StartRight + ZOffset);
			Points.Add(StartPos - SectionHalfWidth * StartRight + ZOffset);
			Points.Add(EndPos + SectionHalfWidth * EndRight);
			Points.Add(EndPos - SectionHalfWidth * EndRight);
			Points.Add(EndPos + SectionHalfWidth * EndRight + ZOffset);
			Points.Add(EndPos - SectionHalfWidth * EndRight + ZOffset);

			NavAreas.Add(Points);

			for (auto Point : Points)
			{
				Bounds += Point;
			}
		}
	}

	bBoundsInitialized = true;
	RefreshNavigationModifiers();
}

void USplineMeshBuilder::GetNavigationData(FNavigationRelevantData& Data) const
{
	for (int32 Idx = 0; Idx < NavAreas.Num(); Idx++)
	{
		FAreaNavModifier Area = FAreaNavModifier(NavAreas[Idx], ENavigationCoordSystem::Unreal, FTransform::Identity, AreaClass);
		Data.Modifiers.Add(Area);
	}
}
