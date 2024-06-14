// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include <RoadManager.hpp>
#include <NavigationSystem.h>
#include <OpenDriveComponent.h>
#include <SplineMeshBuilder.h>
#include "OpenDriveEditorNavMeshModifier.generated.h"

UCLASS()
class OPENDRIVEEDITOR_API AOpenDriveEditorNavMeshModifier : public AActor
{
	GENERATED_BODY()
	
public:	

	virtual void OnConstruction(const FTransform& transform) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	USplineMeshBuilder* _splineMeshBuilder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	USplineComponent* _spline;



	// Sets default values for this actor's properties
	AOpenDriveEditorNavMeshModifier();

	void Initialize(roadmanager::Road* road_, roadmanager::LaneSection* laneSection_, roadmanager::Lane* lane_, float offset_, float step_);

	void CreateSpline();

	void SetLanePoint(double s);

	TArray<FTransform> makeTransformArray(double offset);

	FTransform MakeTransform(double offset);

	void findPoint(double offset);

	void SetNavType(TSubclassOf<UNavArea> navArea);

	void Rebuild();

private: 
	roadmanager::Road* _road;
	roadmanager::LaneSection* _laneSection;
	roadmanager::Lane* _lane;

	float _offset;
	double _step;
	float _baseMeshSize;
	double _laneLength;
	int _blockCount;
	float _blockSize;
	//Used to convert to world pos
	roadmanager::Position _position;
};
