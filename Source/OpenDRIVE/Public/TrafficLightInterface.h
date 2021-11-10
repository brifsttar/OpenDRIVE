// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TrafficLightInterface.generated.h"

UENUM(BlueprintType)
enum class ETrafficLightState : uint8
{
	GREEN = 0,
	AMBER,
	RED,
	AMBER_BLINK,
};

UENUM(BlueprintType)
enum class EPedestrianLightState : uint8
{
	GREEN = 0,
	RED,
};

UINTERFACE(MinimalAPI, Blueprintable)
class UTrafficLightInterface : public UInterface {
	GENERATED_BODY()
};

class ITrafficLightInterface {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TrafficLight")
	ETrafficLightState GetTrafficLightState();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TrafficLight")
	void SetTrafficLightState(ETrafficLightState State);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TrafficLight")
	EPedestrianLightState GetPedestrianLightState();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "TrafficLight")
	void SetPedestrianLightState(EPedestrianLightState State);

};
