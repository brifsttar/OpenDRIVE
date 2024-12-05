// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrafficLight.generated.h"

UENUM(BlueprintType)
enum class ETrafficLightState : uint8 {
	GREEN = 0,
	AMBER,
	RED,
	AMBER_BLINK,
};

UENUM(BlueprintType)
enum class EPedestrianLightState : uint8 {
	GREEN = 0,
	RED,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrafficLightStateChanged, ETrafficLightState, State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPedestrianLightStateChanged, EPedestrianLightState, State);

UCLASS()
class OPENDRIVE_API ATrafficLight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrafficLight();

	UPROPERTY(VisibleDefaultsOnly, Category = "TrafficLight")
	class USceneComponent* Scene;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "TrafficLight", meta = (AllowPrivateAccess = "true"))
	class UOpenDriveComponent* OpenDrive;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TrafficLight")
	TObjectPtr<class ATrafficLightController> Controller;

	UPROPERTY(BlueprintAssignable);
	FOnTrafficLightStateChanged OnTrafficLightStateChanged;

	UPROPERTY(BlueprintAssignable);
	FOnPedestrianLightStateChanged OnPedestrianLightStateChanged;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TrafficLight")
	EPedestrianLightState PedestrianState;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TrafficLight")
	ETrafficLightState LightState;

public:
	UFUNCTION(BlueprintCallable, Category = "TrafficLight")
	ETrafficLightState GetTrafficLightState();
	
	UFUNCTION(BlueprintCallable, Category = "TrafficLight")
	void SetTrafficLightState(ETrafficLightState State);

	UFUNCTION(BlueprintCallable, Category = "TrafficLight")
	EPedestrianLightState GetPedestrianLightState();
	
	UFUNCTION(BlueprintCallable, Category = "TrafficLight")
	void SetPedestrianLightState(EPedestrianLightState State);

};
