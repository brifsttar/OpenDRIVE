// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveComponent.h"
#include "CoordTranslate.h"


// Sets default values for this component's properties
UOpenDriveComponent::UOpenDriveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	_TrackPosCache.SetSnapLaneTypes(roadmanager::Lane::LaneType::LANE_TYPE_ANY);
	// ...
}

void UOpenDriveComponent::GetPosition() {
	if (!GetOwner()) return;
	roadmanager::Position p = OdrPosition();
	RoadId_ = p.GetTrackId();
	LaneId_ = p.GetLaneId();
	S_ = MetersToUu(p.GetS());
	T_ = MetersToUu(p.GetOffset());
	H_ = FMath::RadiansToDegrees(p.GetHRelativeDrivingDirection());
}

void UOpenDriveComponent::SetPosition() {
	if (!GetOwner()) return;
	SetTrackPosition(RoadId_, LaneId_, S_, T_, H_);
}

roadmanager::Position UOpenDriveComponent::OdrPosition() const {
	if (!GetOwner()) return _TrackPosCache;
	FVector p = CoordTranslate::UeToOdr::Location(GetOwner()->GetActorLocation());
	FVector r = CoordTranslate::UeToOdr::Rotation(GetOwner()->GetActorRotation().Euler());
	if (p != _XyzPosCache) {
		_XyzPosCache = p;
		_TrackPosCache.SetInertiaPos(p.X, p.Y, p.Z, r.X, r.Y, r.Z);
	}
	return _TrackPosCache;
}

void UOpenDriveComponent::SetTrackPosition(const roadmanager::Position &p) {
	if (!GetOwner()) return;
	GetOwner()->SetActorTransform(CoordTranslate::OdrToUe::ToTransfrom(p));
	_TrackPosCache = p;
	_TrackPosCache.SetSnapLaneTypes(roadmanager::Lane::LaneType::LANE_TYPE_ANY);
}

void UOpenDriveComponent::SetTrackPosition(int TrackId, int LaneId, float S, float Offset, float H) {
	roadmanager::Position p(TrackId, LaneId, UuToMeters(S), UuToMeters(Offset));
	p.SetHeadingRelative(FMath::DegreesToRadians(H));
	SetTrackPosition(p);
}

bool UOpenDriveComponent::MoveAlongS(float S, int Strategy) {
	roadmanager::Position p = OdrPosition();
	roadmanager::Position::ReturnCode ret;
	ret = p.MoveAlongS(UuToMeters(S), 0., roadmanager::Junction::JunctionStrategyType(Strategy));
	SetTrackPosition(p);
	return ret >= roadmanager::Position::ReturnCode::OK;
}

void UOpenDriveComponent::ResetPosition() {
	_TrackPosCache = roadmanager::Position();
}

int UOpenDriveComponent::GetRoadId() const {
	return OdrPosition().GetTrackId();
}

int UOpenDriveComponent::GetLaneId() const {
	return OdrPosition().GetLaneId();
}

float UOpenDriveComponent::GetS() const {
	return MetersToUu(OdrPosition().GetS());
}

float UOpenDriveComponent::GetT() const {
	return MetersToUu(OdrPosition().GetOffset());
}

float UOpenDriveComponent::GetH() const {
	return FMath::RadiansToDegrees(OdrPosition().GetHRelativeDrivingDirection());
}

float UOpenDriveComponent::GetLaneWidth() const {
	roadmanager::Position p = OdrPosition();
	double s = p.GetS();
	const roadmanager::Road* r = p.GetRoad();
	if (r == nullptr) return NAN;
	const roadmanager::LaneSection* ls = r->GetLaneSectionByS(s);
	if (ls == nullptr) return NAN;
	return MetersToUu(ls->GetWidth(s, p.GetLaneId()));
}

int UOpenDriveComponent::GetJunctionId() const {
	roadmanager::Road* r = OdrPosition().GetRoad();
	return r ? r->GetJunction() : -1;
}

float UOpenDriveComponent::GetNextJunctionDistance() const {
	return MetersToUu(OdrPosition().GetNextJunction().distance);
}

int UOpenDriveComponent::GetNextJunctionId() const {
	return OdrPosition().GetNextJunction().junction->GetId();
}

bool UOpenDriveComponent::IsJunctionDistanceLessThan(float Dist, int JunctionId) const {
	roadmanager::Position::NextJunction next = OdrPosition().GetNextJunction();
	bool isLess = next.distance < UuToMeters(Dist);
	if (JunctionId == -1) {
		return isLess;
	} else {
		return isLess && next.junction && next.junction->GetId() == JunctionId;
	}
}

float UOpenDriveComponent::SDistanceTo(const UOpenDriveComponent* Other) const {
	if (!Other) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("UOpenDriveComponent::SDistanceTo(): No other point"));
		return NAN;
	}
	roadmanager::PositionDiff diff;
	roadmanager::Position otherPos = Other->OdrPosition();
	if (OdrPosition().Delta(&otherPos, diff)) {
		return MetersToUu(diff.ds);
	} else {
		return std::numeric_limits<double>::quiet_NaN();
	}
}

bool UOpenDriveComponent::Delta(const UOpenDriveComponent* Other, float& Ds, float& Dt, int& DLaneId) const {
	if (!Other) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("UOpenDriveComponent::Delta(): No other point"));
		return false;
	}
	roadmanager::PositionDiff diff;
	roadmanager::Position otherPos = Other->OdrPosition();
	bool ret = OdrPosition().Delta(&otherPos, diff);
	Ds = MetersToUu(diff.ds);
	Dt = MetersToUu(diff.dt);
	DLaneId = diff.dLaneId;
	return ret;
}
