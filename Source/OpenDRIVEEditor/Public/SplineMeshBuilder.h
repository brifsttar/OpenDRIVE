// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavModifierVolume.h"
#include "NavRelevantComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include <NavigationSystem.h>
#include "Engine/Polys.h"
#include "Components/BrushComponent.h"
#include "Components/SplineMeshComponent.h"
#include "NavRelevantComponent.h"
#include "SplineMeshBuilder.generated.h"

/**
 * 
 */
UCLASS()
class OPENDRIVEEDITOR_API USplineMeshBuilder : public UNavRelevantComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USplineComponent* Spline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCollisionProfileName CollisionProfile = FName("BlockAll");
	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPhysicalMaterial* PhysicalMaterial;
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//TSubclassOf<UNavArea> AreaClass;
	TSubclassOf<UNavArea> AreaClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AddNavAreas = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsShown = false;

	UFUNCTION(BlueprintCallable)
	void Rebuild();

	virtual void CalcAndCacheBounds() const override;

protected:

	struct SplinePointData {
		FVector Position;
		FVector Scale;
		FVector tangent;
		FVector faceTangent;
	};

	UPROPERTY()
	TArray<USplineMeshComponent*> SplineMeshComponents;

	UPROPERTY()
	TArray<ANavModifierVolume*> NavModifierVolumes;

	TArray<TArray<FVector>> NavAreas;

	void ClearCurrentMeshes();
	USplineMeshComponent* CreateMeshComponent(SplinePointData startPoint, SplinePointData endPoint);
	void AddMeshesToOwner();


	TArray<FVector> EvaluateBound(SplinePointData startPoint, SplinePointData endPoint);

	virtual void GetNavigationData(FNavigationRelevantData& Data) const override;

	FVector FastCW(double x, double y);
	FVector FastCW(FVector dir);
	FVector FastCCW(double x, double y);
	FVector FastCCW(FVector dir);

	void EvaluateAtPoint(SplinePointData& pointData, float dist, float length);


	const FVector ZOffset = FVector(0.0f, 0.0f, 200.0f);


};
