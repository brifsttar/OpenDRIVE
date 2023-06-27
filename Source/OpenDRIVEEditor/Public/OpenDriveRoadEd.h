// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "RoadManager.hpp"
#include "CoordTranslate.h"
#include "OpenDriveRoadEd.generated.h"

UCLASS()
class OPENDRIVEEDITOR_API AOpenDriveRoadEd : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOpenDriveRoadEd();

	void Initialize(roadmanager::Road* road_, float roadOffset_);

	void DrawLanes(roadmanager::Road* road);

protected:

	UPROPERTY(EditAnywhere)
	TArray<USplineMeshComponent*> LanesMeshesArray;

private : 

	TObjectPtr<UStaticMesh> LaneMesh;
	TObjectPtr<UMaterial> LaneMaterial;
	TObjectPtr<UMaterialInstance> LeftMaterial;
	TObjectPtr<UMaterialInstance> RightMaterial;
	TObjectPtr<UMaterialInstance> SidewalkMaterial;

	float roadOffset;

	float baseMeshSize;
};
