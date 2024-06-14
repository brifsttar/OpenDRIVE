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

USplineMeshComponent* USplineMeshBuilder::CreateMeshComponent(FVector StartPos, FVector StartTangent, FVector EndPos, FVector EndTangent, FVector2D startScale, FVector2D endScale)
{
	FName Name = MakeUniqueObjectName(GetOwner(), USplineMeshComponent::StaticClass());
	USplineMeshComponent* MeshComponent = NewObject<USplineMeshComponent>(this, Name);
	MeshComponent->SetStaticMesh(Mesh);
	MeshComponent->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
	MeshComponent->SetStartScale(startScale);
	MeshComponent->SetEndScale(endScale);
	MeshComponent->SetCollisionProfileName(CollisionProfile.Name);
	//MeshComponent->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;

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
		FVector StartScale = Spline->GetScaleAtDistanceAlongSpline(Distance);
		StartTangent = StartTangent.GetClampedToMaxSize(SectionLength);
		FVector EndPos = Spline->GetLocationAtDistanceAlongSpline(Distance + SectionLength, ESplineCoordinateSpace::World);
		FVector EndTangent = Spline->GetTangentAtDistanceAlongSpline(Distance + SectionLength, ESplineCoordinateSpace::World);
		FVector EndScale = Spline->GetScaleAtDistanceAlongSpline(Distance);
		EndTangent = EndTangent.GetClampedToMaxSize(SectionLength);

		USplineMeshComponent* MeshComponent = CreateMeshComponent(StartPos, StartTangent, EndPos, EndTangent, FVector2D(StartScale.Y, StartScale.X), FVector2D(EndScale.Y, EndScale.X));

		MeshComponent->SetVisibility(IsShown);
		GetOwner()->AddOwnedComponent(MeshComponent);
		MeshComponent->RegisterComponent();

		SplineMeshComponents.Add(MeshComponent);

		if (AddNavAreas)
		{

			FVector StartFace = perpCW(StartTangent.X, StartTangent.Y);
			FVector EndFace = perpCW(StartTangent.X, StartTangent.Y);
			StartFace.Normalize();
			EndFace.Normalize();

			//FVector StartRight = Spline->GetRightVectorAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
			//FVector EndRight = Spline->GetRightVectorAtDistanceAlongSpline(Distance + SectionLength, ESplineCoordinateSpace::World);

			FVector ZOffset(0.0f, 0.0f, 200.0f);

			TArray<FVector> Points;

			Points.Add(StartPos + StartScale * StartFace * 50); // (*) 100 m->cm | (/) 2 from middle of scale
			Points.Add(StartPos - StartFace * StartScale * 50);
			Points.Add(Points[0] + ZOffset);
			Points.Add(Points[1] + ZOffset);
			Points.Add(EndPos + EndFace * EndScale * 50);
			Points.Add(EndPos - EndFace * EndScale * 50);
			Points.Add(Points[4] + ZOffset * 50);
			Points.Add(Points[5] + ZOffset * 50);
			/*Points.Add(StartPos + StartScale * 100 * StartRight / 2);
			Points.Add(StartPos - StartScale * 100 * StartRight / 2);
			Points.Add(StartPos + StartScale * 100 * StartRight / 2 + ZOffset);
			Points.Add(StartPos - StartScale * 100 * StartRight / 2 + ZOffset);
			Points.Add(EndPos + EndScale * 100 * EndRight / 2);
			Points.Add(EndPos - EndScale * 100 * EndRight / 2);
			Points.Add(EndPos + EndScale * 100 * EndRight / 2 + ZOffset);
			Points.Add(EndPos - EndScale * 100 * EndRight / 2 + ZOffset);
			*/
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
FVector USplineMeshBuilder::perpCW(double x, double y){
	return FVector (y,-x,0);
}
FVector USplineMeshBuilder::perpCW(FVector dir)
{
	return FVector(dir.Y,-dir.X,0);
}
FVector USplineMeshBuilder::perpCCW(double x, double y)
{
	return FVector(-y, x, 0);
}
FVector USplineMeshBuilder::perpCCW(FVector dir)
{
	return FVector(-dir.Y,dir.X,0);
}
