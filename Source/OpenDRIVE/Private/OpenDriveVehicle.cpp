// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveVehicle.h"
#include "WheeledVehiclePawn.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "ChaosWheeledVehicleMovementComponent.h"

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
	_Car = dynamic_cast<AWheeledVehiclePawn*>(GetOwner());
	if (_Car) {
		_MovComp = Cast<UChaosWheeledVehicleMovementComponent>(_Car->GetVehicleMovementComponent());
	}
}

FBoxSphereBounds UOpenDriveVehicle::GetBounds() const {
	return FBoxSphereBounds(
		_Car->GetMesh()->GetSkeletalMeshAsset()->GetPhysicsAsset()->CalcAABB(
			_Car->GetMesh(),
			FTransform::Identity
		)
	);
}

double UOpenDriveVehicle::LengthFront() const
{
	FBoxSphereBounds b = GetBounds();
	float x = b.BoxExtent.X;
	float o = b.Origin.X;
	return o + x;
}

double UOpenDriveVehicle::LengthBack() const
{
	FBoxSphereBounds b = GetBounds();
	float x = b.BoxExtent.X;
	float o = b.Origin.X;
	return o - x;
}

double UOpenDriveVehicle::OdrSpeed() const {
	return CoordTranslate::UuToMeters(_MovComp->GetForwardSpeed());
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
	return -FMath::DegreesToRadians(_MovComp->Wheels[0]->GetSteerAngle());
}

double UOpenDriveVehicle::OdrSteerAngleMax() const {
	return FMath::DegreesToRadians(_MovComp->Wheels[0]->MaxSteerAngle);
}

double UOpenDriveVehicle::OdrWheelbase() const {
	if (_Wheelbase == 0.) {
		int frontIdx = 0;
		int backIdx = _MovComp->WheelSetups.Num() > 2 ? 3 : 1;
		FName wFrontName = _MovComp->WheelSetups[frontIdx].BoneName;
		FName wBackName = _MovComp->WheelSetups[backIdx].BoneName;
		float wFrontPos = _Car->GetMesh()->GetBoneLocation(wFrontName, EBoneSpaces::ComponentSpace).X;
		float wBackPos  = _Car->GetMesh()->GetBoneLocation(wBackName , EBoneSpaces::ComponentSpace).X;
		_Wheelbase = CoordTranslate::UuToMeters(std::abs(wFrontPos - wBackPos));
	}
	return _Wheelbase;
}

float UOpenDriveVehicle::RoadDistanceTo(const UOpenDriveVehicle *Other) const {
	double ds = SDistanceTo(Other);
	if (!Other) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("UOpenDriveVehicle::RoadDistanceTo(): No other vehicle"));
		return NAN;
	}
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
	if (!Other) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("UOpenDriveVehicle::RoadDistanceTo(): No other vehicle"));
		return NAN;
	}
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
	if (!Other) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("UOpenDriveVehicle::RoadDistanceTo(): No other vehicle"));
		return NAN;
	}
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
