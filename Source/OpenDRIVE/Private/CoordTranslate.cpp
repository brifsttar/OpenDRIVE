// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoordTranslate.h"

FTransform CoordTranslate::OdrToUe::ToTransfrom(const roadmanager::Position &P) {
	return FTransform(
		ToRotation(P),
		ToLocation(P)
	);
}

FVector CoordTranslate::OdrToUe::ToLocation(const roadmanager::Position &P) {
	return Location(FVector(P.GetX(), P.GetY(), P.GetZ()));
}

FRotator CoordTranslate::OdrToUe::ToRotation(const roadmanager::Position &P) {
	return FRotator(
		FMath::RadiansToDegrees( P.GetP()),
		FMath::RadiansToDegrees(-P.GetH()),
		FMath::RadiansToDegrees( P.GetR())
	);
}

FVector CoordTranslate::OdrToUe::Location(const FVector &P) {
	return FVector(
		MetersToUu( P.X),
		MetersToUu(-P.Y),
		MetersToUu( P.Z)
	);
}

FVector CoordTranslate::OdrToUe::Rotation(const FVector &P) {
	throw std::exception("CoordTranslate::OdrToUe::Rotation is not implemented yet.");
}

roadmanager::Position CoordTranslate::UeToOdr::FromTransfrom(const FTransform &P) {
	return roadmanager::Position(
		UuToMeters( P.GetLocation().X),
		UuToMeters(-P.GetLocation().Y),
		UuToMeters( P.GetLocation().Z),
		FMath::DegreesToRadians(-P.GetRotation().Rotator().Yaw  ),
		FMath::DegreesToRadians( P.GetRotation().Rotator().Pitch),
		FMath::DegreesToRadians( P.GetRotation().Rotator().Roll)
	);
}

roadmanager::Position CoordTranslate::UeToOdr::FromLocation(const FVector &P) {
	return FromTransfrom(FTransform(FRotator(), P));
}

FVector CoordTranslate::UeToOdr::Location(const FVector &P) {
	return FVector(
		UuToMeters( P.X),
		UuToMeters(-P.Y),
		UuToMeters( P.Z)
	);
}

FVector CoordTranslate::UeToOdr::Rotation(const FVector &P) {
	return FVector(
		FMath::DegreesToRadians(-P.Z),
		FMath::DegreesToRadians( P.Y),
		FMath::DegreesToRadians( P.X)
	);
}
