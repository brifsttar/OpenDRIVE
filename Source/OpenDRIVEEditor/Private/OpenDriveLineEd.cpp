// Fill out your copyright notice in the Description page of Project Settings.
#include "OpenDriveLineEd.h"

// Sets default values for this component's properties
UOpenDriveLineEd::UOpenDriveLineEd()
{
	PrimaryComponentTick.bCanEverTick = false;

	LaneSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RoadSpline"));
}

void UOpenDriveLineEd::Init(roadmanager::Lane* lane_, double length_)
{

}

