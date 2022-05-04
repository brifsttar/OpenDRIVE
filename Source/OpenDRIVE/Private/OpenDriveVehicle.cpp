// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveVehicle.h"
#include "WheeledVehicle.h"
#include "WheeledVehicleMovementComponent.h"

UOpenDriveVehicle::UOpenDriveVehicle() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UOpenDriveVehicle::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OnNewRoad.IsBound() && PrevRoadId != GetRoadId()) {
		PrevRoadId = GetRoadId();
		OnNewRoad.Broadcast(PrevRoadId);
	}
}

void UOpenDriveVehicle::PostLoad() {
	Super::PostLoad();
	if (!GetOwner()) return;
	_Car = dynamic_cast<AWheeledVehicle *>(GetOwner());
}

FBoxSphereBounds UOpenDriveVehicle::GetBounds() const {
	return FBoxSphereBounds(
		_Car->GetMesh()->SkeletalMesh->PhysicsAsset->CalcAABB(
			_Car->GetMesh(),
			FTransform::Identity
		)
	);
}

double UOpenDriveVehicle::LengthFront() const {
	FBoxSphereBounds b = GetBounds();
	float x = b.BoxExtent.X;
	float o = b.Origin.X;
	return o + x;
}

double UOpenDriveVehicle::LengthBack() const {
	FBoxSphereBounds b = GetBounds();
	float x = b.BoxExtent.X;
	float o = b.Origin.X;
	return o - x;
}

double UOpenDriveVehicle::OdrSpeed() const {
	if (_Car->GetMesh()->IsSimulatingPhysics()) {
		return CoordTranslate::UuToMeters(_Car->GetVehicleMovement()->GetForwardSpeed());
	} else {
		return _SpeedOverride;
	}
}

double UOpenDriveVehicle::OdrAcceleration() const {
	float t = _Car->GetGameTimeSinceCreation();
	if (_PrevTime == 0.0) {
		_PrevTime = t;
		_PrevSpeed = OdrSpeed();
	}
	if (_PrevTime != t) {
		_Acc = (OdrSpeed() - _PrevSpeed) / (t - _PrevTime);
		_PrevTime = t;
		_PrevSpeed = OdrSpeed();
	}
	return _Acc;
}

double UOpenDriveVehicle::OdrSteerAngle() const {
	return -FMath::DegreesToRadians(_Car->GetVehicleMovement()->Wheels[0]->GetSteerAngle());
}

double UOpenDriveVehicle::OdrSteerAngleMax() const {
	return FMath::DegreesToRadians(_Car->GetVehicleMovement()->Wheels[0]->SteerAngle);
}

double UOpenDriveVehicle::OdrWheelbase() const {
	FName wFrontName = _Car->GetVehicleMovement()->WheelSetups[0].BoneName;
	FName wBackName = _Car->GetVehicleMovement()->WheelSetups[3].BoneName;
	float wFrontPos = NAN, wBackPos = NAN;
	for (auto &c : _Car->GetMesh()->Constraints) {
		//todo That probably doesn't work for all cars...
		if (c->ConstraintBone1 == wFrontName) wFrontPos = c->Pos2.X;
		if (c->ConstraintBone2 == wFrontName) wFrontPos = c->Pos2.X;
		if (c->ConstraintBone1 == wBackName) wBackPos = c->Pos2.X;
		if (c->ConstraintBone2 == wBackName) wBackPos = c->Pos2.X;
	}
	return CoordTranslate::UuToMeters(std::abs(wFrontPos - wBackPos));
}

float UOpenDriveVehicle::RoadDistanceTo(const UOpenDriveVehicle *Other) const {
	double ds = SDistanceTo(Other);
	if (ds >= 0) {
		// Other is in front of us
		ds -= LengthFront() - Other->LengthBack();
		ds = std::max(0., ds);
	} else {
		// Other is behind us
		ds -= LengthBack() - Other->LengthFront();
		ds = std::min(0., ds);
	}
	return ds;
}

float UOpenDriveVehicle::RoadDistanceToOdr(const UOpenDriveComponent *Other) const {
	double ds = SDistanceTo(Other);
	if (ds >= 0) {
		// Other is in front of us
		ds -= LengthFront();
		ds = std::max(0., ds);
	} else {
		// Other is behind us
		ds -= LengthBack();
		ds = std::min(0., ds);
	}
	return ds;
}

float UOpenDriveVehicle::THW(const UOpenDriveVehicle *Other) const {
	double dist = RoadDistanceTo(Other);
	if (!std::isnan(dist) && dist >= 0) {
		if (Speed() == 0.) {
			return std::numeric_limits<double>::infinity();
		}
		// THW is only valid if car is in front of us
		return dist / Speed();
	} else {
		return NAN;
	}
}

float UOpenDriveVehicle::TimeTo(const UOpenDriveComponent *Other) const {
	return RoadDistanceToOdr(Other) / Speed();
}

float UOpenDriveVehicle::TimeToWithAcc(const UOpenDriveComponent *Other) const {
	double dist = RoadDistanceToOdr(Other);
	if (!std::isnan(dist) && (dist * Speed() >= 0)) {
		// ETTC is only valid if car is in front of us
		return ETTC(dist, Speed(), Acceleration());
	} else {
		return NAN;
	}
}

float UOpenDriveVehicle::TTC(const UOpenDriveVehicle *Other) const {
	if (Speed() == Other->Speed()) {
		return std::numeric_limits<double>::infinity();
	}
	double dist = RoadDistanceTo(Other);
	if (!std::isnan(dist) && (dist * Speed() >= 0)) {
		// TTC is only valid if car is in front of us
		return dist / (Speed() - Other->Speed());
	} else {
		return NAN;
	}
}

float UOpenDriveVehicle::ETTC(const UOpenDriveVehicle *Other) const {
	if (Acceleration() == Other->Acceleration()) {
		return TTC(Other);
	}
	double dist = RoadDistanceTo(Other);
	if (!std::isnan(dist) && (dist * Speed() >= 0)) {
		// ETTC is only valid if car is in front of us
		double deltaSpeed = Speed() - Other->Speed();
		double deltaAcc = Acceleration() - Other->Acceleration();
		return ETTC(dist, deltaSpeed, deltaAcc);
	} else {
		return NAN;
	}
}

double UOpenDriveVehicle::ETTC(double DeltaDist, double DeltaSpeed, double DeltaAcc) const {
	return -(DeltaSpeed - sqrt(pow(DeltaSpeed, 2) + 2 * DeltaAcc * DeltaDist)) / DeltaAcc;
}
