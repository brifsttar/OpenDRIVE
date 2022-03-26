// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RoadManager.hpp"

namespace CoordTranslate {
	namespace OdrToUe {
		FTransform OPENDRIVE_API ToTransfrom(const roadmanager::Position &P);
		FVector    OPENDRIVE_API ToLocation (const roadmanager::Position &P);
		FRotator   OPENDRIVE_API ToRotation (const roadmanager::Position &P);
		FVector    OPENDRIVE_API Location   (const FVector &P);
		FVector    OPENDRIVE_API Rotation   (const FVector &P);
	}

	namespace UeToOdr {
		roadmanager::Position OPENDRIVE_API FromTransfrom(const FTransform &P);
		roadmanager::Position OPENDRIVE_API FromLocation (const FVector &P);
		FVector               OPENDRIVE_API Location     (const FVector &P);
		FVector               OPENDRIVE_API Rotation     (const FVector &P);
	}

	constexpr float OPENDRIVE_API MetersToUu(double Meters) { return Meters * 100.f; };
	constexpr double OPENDRIVE_API UuToMeters(float Uu) { return Uu / 100.; };
};
