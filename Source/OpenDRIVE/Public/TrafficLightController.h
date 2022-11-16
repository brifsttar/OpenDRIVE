// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JunctionSignSpawner.h"
#include "TrafficLightController.generated.h"

USTRUCT(BlueprintType)
struct FTrafficLightGroup {
	GENERATED_USTRUCT_BODY()

	// For some reason this doesn't work. It can be set via BP, but not in the editor window.
	//UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<TScriptInterface<ITrafficLightInterface>> TrafficLights;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TrafficLightGroup")
	TArray<class ATrafficLight*> TrafficLights;

	/**
	* Green light start time (s)
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TrafficLightGroup")
	float GreenStart = 0.f;

	/**
	* Amber light start time (s)
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TrafficLightGroup")
	float AmberStart = 0.f;

	/**
	* Red light start time (s)
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TrafficLightGroup")
	float RedStart = 0.f;

	/**
	* Pedestrian green light start time (s)
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TrafficLightGroup")
	float PedestrianGreenStart = 0.f;

	/**
	* Pedestrian red light start time (s)
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TrafficLightGroup")
	float PedestrianRedStart = 0.f;
};

UCLASS()
class OPENDRIVE_API ATrafficLightController : public AJunctionSignSpawner
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrafficLightController();

	/**
	* Traffic lights groups
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Traffic Light|Controller")
	TArray<FTrafficLightGroup> TrafficLightGroups;

	/**
	* Total sequence duration (s)
	*/
	UPROPERTY(EditAnywhere, Category = "Traffic Light|Controller")
	float SequenceDuration = 0.f;

	/**
	* Current sequence time (s)
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Traffic Light|Controller")
	float SequenceTime = 0.f;

	/**
	* Whether the sequence is paused or not
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Traffic Light|Controller")
	bool PauseSequence = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdateLights();

	// If possible, creates and initiliazes groups
	void CreateDefaultGroups();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SpawnJunctionSigns() override;
};
