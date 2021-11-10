// Fill out your copyright notice in the Description page of Project Settings.


#include "odrUePositionable.h"

#include "GameFramework/Actor.h"
#include "CoordTranslate.h"



odrUePositionable::odrUePositionable(AActor* Actor) : _Actor(Actor)
{

}

odrUePositionable::~odrUePositionable()
{

}

std::array<double, 6> odrUePositionable::Position() const {
	FVector p = CoordTranslate::UeToOdr::Location(_Actor->GetActorLocation());
	FVector r = CoordTranslate::UeToOdr::Rotation(_Actor->GetActorRotation().Euler());
	return std::array<double, 6>({ p.X, p.Y, p.Z, r.X, r.Y, r.Z });
}
