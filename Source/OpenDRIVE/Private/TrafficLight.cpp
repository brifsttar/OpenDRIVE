// Fill out your copyright notice in the Description page of Project Settings.


#include "TrafficLight.h"
#include "OpenDriveComponent.h"

ATrafficLight::ATrafficLight() {
	PrimaryActorTick.bCanEverTick = true;


	Scene = CreateDefaultSubobject<USceneComponent>("Root");
	Scene->SetupAttachment(RootComponent);

	OpenDrive = CreateDefaultSubobject<UOpenDriveComponent>(TEXT("OpenDrive"));
	OpenDrive->SetupAttachment(Scene);

}

ETrafficLightState ATrafficLight::GetTrafficLightState() {
	return LightState;
}

void ATrafficLight::SetTrafficLightState(ETrafficLightState State) {
	if (State == LightState) return;
	LightState = State;
	OnTrafficLightStateChanged.Broadcast(LightState);
}

EPedestrianLightState ATrafficLight::GetPedestrianLightState() {
	return PedestrianState;
}

void ATrafficLight::SetPedestrianLightState(EPedestrianLightState State) {
	if (State == PedestrianState) return;
	PedestrianState = State;
	OnPedestrianLightStateChanged.Broadcast(PedestrianState);
}

