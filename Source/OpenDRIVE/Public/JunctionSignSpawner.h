// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JunctionSignSpawner.generated.h"

UCLASS()
class OPENDRIVE_API AJunctionSignSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJunctionSignSpawner();

	UPROPERTY(EditDefaultsOnly)
	class UOpenDriveComponent* OpenDrive;

	UPROPERTY(EditDefaultsOnly)
	class UBillboardComponent* BillboardComponent;

	UPROPERTY(EditAnywhere, Category = "Junction Spawner|Generation")
	TSubclassOf<AActor> JunctionSignClass;

	/**
	* Whether to clear previously generated signs on re-generation
	*/
	UPROPERTY(EditAnywhere, Category = "Junction Spawner|Generation")
	bool bClearExisting = false;

	/**
	* S-coordinate offset (cm) from the junction
	*/
	UPROPERTY(EditAnywhere, Category = "Junction Spawner|Generation")
	float SOffset = 100.f;

	/**
	* T-coordinate offset (cm) from the sidewalk center
	*/
	UPROPERTY(EditAnywhere, Category = "Junction Spawner|Generation")
	float TOffset = 0.f;

	/**
	* Heading offset (°) from the road orientation
	*/
	UPROPERTY(EditAnywhere, Category = "Junction Spawner|Generation")
	float HOffset = -90.f;

protected:
	class UTexture2D* SpriteTexture;

	TArray<AActor*> JunctionSigns;

public:
	UFUNCTION(BlueprintCallable, CallInEditor, meta = (Category = "Junction Spawner"))
	virtual void SpawnJunctionSigns();
};
