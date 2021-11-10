// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveTrajectory.h"
#include "RoadManager.hpp"
#include "CoordTranslate.h"


// Sets default values
AOpenDriveTrajectory::AOpenDriveTrajectory() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	RootComponent = Spline;

}

void AOpenDriveTrajectory::AlignPointsWithLanes() {
	int count = Spline->GetNumberOfSplinePoints();
	FVector oldPos, newPos;
	for (int i = 0; i < count; i++) {
		oldPos = Spline->GetWorldLocationAtSplinePoint(i);
		roadmanager::Position trackPos = CoordTranslate::UeToOdr::FromLocation(oldPos);
		trackPos.SetOffset(0);
		// You want to hear a boring story? Here's one:
		// Position::SetOffset doesn't update inertial coord. Neither does pretty much any of the
		// Position::Set<some track coord> by the way.
		// I could have fixed that. There's probably a dozen valid fixes for that.
		// Otherwise, I could also have made Position::Track2XYZ public and called it, that would
		// have worked too.
		// But you know what I did instead? Well, you can just look at the code below and see for
		// yourself.
		trackPos.MoveAlongS(0.);
		// That's right, I called Position::MoveAlongS with a null ds, which doesn't move me, but
		// it has the side effect of updating inertial coord, and hey, that's what I want.
		// Why on earth did I do that knowingly, then wrote a comment that's longer than this whole
		// frigging class to explain my wrongdoing?
		// That's a very good question you're asking here, for which I sadly have no answer.
		// Have a nice day!
		newPos = CoordTranslate::OdrToUe::ToLocation(trackPos);
		Spline->SetWorldLocationAtSplinePoint(i, newPos);
	}
}

// Called when the game starts or when spawned
void AOpenDriveTrajectory::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOpenDriveTrajectory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

