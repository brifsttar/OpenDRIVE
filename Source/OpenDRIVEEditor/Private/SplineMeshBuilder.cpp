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

USplineMeshComponent* USplineMeshBuilder::CreateMeshComponent(SplinePointData startPoint, SplinePointData endPoint)
{
	FName Name = MakeUniqueObjectName(GetOwner(), USplineMeshComponent::StaticClass());
	USplineMeshComponent* MeshComponent = NewObject<USplineMeshComponent>(this, Name);
	MeshComponent->SetStaticMesh(Mesh);
	MeshComponent->SetStartAndEnd(startPoint.Position, startPoint.tangent, endPoint.Position, endPoint.tangent);
	MeshComponent->SetStartScale(FVector2D(startPoint.Scale.Y, startPoint.Scale.X));
	MeshComponent->SetEndScale(FVector2D(endPoint.Scale.Y, endPoint.Scale.X));
	MeshComponent->SetCollisionProfileName(CollisionProfile.Name);
	//MeshComponent->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;

	MeshComponent->SetMobility(EComponentMobility::Movable);
	MeshComponent->CreationMethod = EComponentCreationMethod::Instance;
	//MeshComponent->SetPhysMaterialOverride(PhysicalMaterial);

	MeshComponent->SetVisibility(IsShown);
	GetOwner()->AddOwnedComponent(MeshComponent);
	MeshComponent->RegisterComponent();


	return MeshComponent;
}



void USplineMeshBuilder::AddMeshesToOwner(){
	if (!Mesh || !Spline) return;

	SplinePointData startPoint;
	SplinePointData endPoint;
	float Distance;

	float SectionLength = Mesh->GetBounds().BoxExtent.X * 2.0f;
	float SectionHalfWidth = Mesh->GetBounds().BoxExtent.Y * 0.75f;
	float SplineLength = Spline->GetSplineLength();
	int32 NumSegments = SplineLength / SectionLength;

	Bounds.Init();

	EvaluateAtPoint(startPoint, 0, SectionLength);

	for (int32 Index = 0; Index < NumSegments; ++Index)
	{
		EvaluateAtPoint(endPoint, Distance + SectionLength, SectionLength);

		SplineMeshComponents.Add(CreateMeshComponent(startPoint, endPoint));

		if (AddNavAreas){
			TArray<FVector> Points = EvaluateBound(startPoint,endPoint);
			NavAreas.Add(Points);
			for (auto Point : Points){
				Bounds += Point;
			}
		}
		Distance += SectionLength;
		startPoint = endPoint;
	}

	bBoundsInitialized = true;
	RefreshNavigationModifiers();
}






TArray<FVector> USplineMeshBuilder::EvaluateBound(SplinePointData startPoint, SplinePointData endPoint) {
	TArray<FVector> points;
	points.Add(startPoint.Position + startPoint.Scale.Y * startPoint.faceTangent * 50); // (*) 100 m->cm | (/) 2 from middle of scale
	points.Add(startPoint.Position - startPoint.Scale.Y * startPoint.faceTangent * 50);
	points.Add(points[0] + ZOffset);
	points.Add(points[1] + ZOffset);
	points.Add(endPoint.Position + endPoint.Scale.Y * endPoint.faceTangent * 50);
	points.Add(endPoint.Position - endPoint.Scale.Y * endPoint.faceTangent * 50);
	points.Add(points[4] + ZOffset * 50);
	points.Add(points[5] + ZOffset * 50);
	return points;
}


void USplineMeshBuilder::GetNavigationData(FNavigationRelevantData& Data) const
{
	for (int32 Idx = 0; Idx < NavAreas.Num(); Idx++)
	{
		FAreaNavModifier Area = FAreaNavModifier(NavAreas[Idx], ENavigationCoordSystem::Unreal, FTransform::Identity, AreaClass);
		Data.Modifiers.Add(Area);
	}
}
FVector USplineMeshBuilder::FastCW(double x, double y){
	return FVector (y,-x,0);
}
FVector USplineMeshBuilder::FastCW(FVector dir)
{
	return FVector(dir.Y,-dir.X,0);
}
FVector USplineMeshBuilder::FastCCW(double x, double y)
{
	return FVector(-y, x, 0);
}
FVector USplineMeshBuilder::FastCCW(FVector dir)
{
	return FVector(-dir.Y,dir.X,0);
}

void USplineMeshBuilder::EvaluateAtPoint(SplinePointData& pointData, float dist, float length){
	pointData.Position = Spline->GetLocationAtDistanceAlongSpline(dist, ESplineCoordinateSpace::World);
	pointData.tangent = Spline->GetTangentAtDistanceAlongSpline(dist, ESplineCoordinateSpace::World);
	pointData.Scale = Spline->GetScaleAtDistanceAlongSpline(dist);
	pointData.tangent = pointData.tangent.GetClampedToMaxSize(length);
	pointData.faceTangent = FastCW(pointData.tangent.X, pointData.tangent.Y);
	pointData.faceTangent.Normalize();
}
