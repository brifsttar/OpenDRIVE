// Fill out your copyright notice in the Description page of Project Settings.


#include "TrafficLightController.h"

#include "RoadManager.hpp"
//#include "OpenDriveComponent.h"
#include "Components/BillboardComponent.h"
#include "EngineUtils.h"
#include "CoordTranslate.h"

#include <set>
#include <algorithm>

using CoordTranslate::UuToMeters;

// Sets default values
ATrafficLightController::ATrafficLightController()
{
	bClearExisting = true;

	// Setting default sequence times
	FTrafficLightGroup g1, g2;
	SequenceDuration = 30.;

	g1.GreenStart = 0.;
	g1.AmberStart = 12.;
	g1.RedStart = 15.;
	g1.PedestrianGreenStart = 17.;
	g1.PedestrianRedStart = 28.;

	g2.GreenStart = 15.;
	g2.AmberStart = 27.;
	g2.RedStart = 30.;
	g2.PedestrianGreenStart = 2.;
	g2.PedestrianRedStart = 13.;

	TrafficLightGroups.Add(g1);
	TrafficLightGroups.Add(g2);
}

// Called when the game starts or when spawned
void ATrafficLightController::BeginPlay()
{
	Super::BeginPlay();
	UpdateLights();
}

// Called every frame
void ATrafficLightController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!PauseSequence) {
		SequenceTime += DeltaTime;
		SequenceTime = fmod(SequenceTime, SequenceDuration);
	}
	UpdateLights();
}

void ATrafficLightController::UpdateLights()
{
	for (auto& g : TrafficLightGroups) {
		// Naughty magic to get the current state based on timer

		// Traffic light
		ETrafficLightState state = ETrafficLightState::RED;
		std::set<float> times = { g.GreenStart, g.AmberStart, g.RedStart, SequenceDuration };
		auto upper = std::upper_bound(times.begin(), times.end(), SequenceTime);
		if (upper == times.cend()) continue;
		if (*upper == g.GreenStart) state = ETrafficLightState::RED;
		else if (*upper == g.AmberStart) state = ETrafficLightState::GREEN;
		else if (*upper == g.RedStart)  state = ETrafficLightState::AMBER;
		else if (*upper == SequenceDuration) {
			std::vector<float> t = { g.GreenStart, g.AmberStart, g.RedStart };
			state = (ETrafficLightState)std::distance(t.begin(), std::max_element(t.begin(), t.end()));
		}

		// Pedestrian light
		EPedestrianLightState pedState = EPedestrianLightState::RED;
		times = { g.PedestrianGreenStart, g.PedestrianRedStart, SequenceDuration };
		upper = std::upper_bound(times.begin(), times.end(), SequenceTime);
		if (upper == times.cend()) continue;
		if (*upper == g.PedestrianGreenStart) pedState = EPedestrianLightState::RED;
		else if (*upper == g.PedestrianRedStart) pedState = EPedestrianLightState::GREEN;
		else if (*upper == SequenceDuration) {
			pedState = g.PedestrianGreenStart > g.PedestrianRedStart ?
				EPedestrianLightState::GREEN :
				EPedestrianLightState::RED;
		}

		// Applying state
		for (auto& tl : g.TrafficLights) {
			ITrafficLightInterface::Execute_SetTrafficLightState(tl, state);
			ITrafficLightInterface::Execute_SetPedestrianLightState(tl, pedState);
		}
	}
}

void ATrafficLightController::SpawnJunctionSigns()
{
	Super::SpawnJunctionSigns();
	CreateDefaultGroups();
}


void ATrafficLightController::CreateDefaultGroups()
{
	// Only supporting 4-way junctions for now
	if (JunctionSigns.Num() != 4) return;
	// We expect only the 2 default groups
	if (TrafficLightGroups.Num() != 2) return;

	// Maximum angle offset to group traffic lights
	const float MAX_DH = 30.;

	// Finding pairs
	std::set<AActor*> tlset;
	tlset.insert(JunctionSigns.begin(), JunctionSigns.end());
	AActor* refTl = JunctionSigns[0];
	TrafficLightGroups[0].TrafficLights.Add(refTl);
	tlset.erase(refTl);
	FRotator refRot = refTl->GetActorTransform().GetRotation().Rotator();
	for (auto& tl : tlset) {
		FRotator tlRot = tl->GetActorTransform().GetRotation().Rotator();
		float da = std::abs((tlRot - refRot).Euler().Z);
		if ((180 - MAX_DH) < da && da < (180 + MAX_DH)) {
			TrafficLightGroups[0].TrafficLights.Add(tl);
			tlset.erase(tl);
			break;
		}
	}
	for (auto& tl : tlset) {
		TrafficLightGroups[1].TrafficLights.Add(tl);
	}
}
