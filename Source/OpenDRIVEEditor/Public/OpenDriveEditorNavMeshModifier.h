// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpenDriveEditorNavMeshModifier.generated.h"

UCLASS()
class OPENDRIVEEDITOR_API AOpenDriveEditorNavMeshModifier : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOpenDriveEditorNavMeshModifier();

	void Initialize(roadmanager::Road* road_, roadmanager::LaneSection* laneSection_, roadmanager::Lane* lane_, float offset_, float step_);

	TArray<FTransform> makeTransformArray(double offset);

	FTransform MakeTransform(double offset);

	void findPoint(double offset);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private: 
	roadmanager::Road* _road;
	roadmanager::LaneSection* _laneSection;
	roadmanager::Lane* _lane;

	double _laneLength;
	int _blockCount;
	float _blockSize;
	//Used to convert to world pos
	roadmanager::Position _position;
};
