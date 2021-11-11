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


// Called when the game starts
void UOpenDriveComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UOpenDriveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UOpenDriveComponent::GetPosition() {
	if (!GetOwner()) return;
	roadmanager::Position p = OdrPosition();
	RoadId_ = p.GetTrackId();
	LaneId_ = p.GetLaneId();
	S_ = p.GetS() * MetersToUu();
	T_ = p.GetOffset() * MetersToUu();
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
	roadmanager::Position p(TrackId, LaneId, S * UuToMeters(), Offset * UuToMeters());
	p.SetHeadingRelative(FMath::DegreesToRadians(H));
	SetTrackPosition(p);
}

bool UOpenDriveComponent::MoveAlongS(float S, int Strategy) {
	roadmanager::Position p = OdrPosition();
	roadmanager::Position::ErrorCode ret;
	ret = p.MoveAlongS(S * UuToMeters(), 0., roadmanager::Junction::JunctionStrategyType(Strategy));
	SetTrackPosition(p);
	return ret == roadmanager::Position::ErrorCode::ERROR_NO_ERROR;
}

void UOpenDriveComponent::ResetPosition() {
	SetTrackPosition(roadmanager::Position());
}

int UOpenDriveComponent::GetRoadId() const {
	return OdrPosition().GetTrackId();
}

int UOpenDriveComponent::GetLaneId() const {
	return OdrPosition().GetLaneId();
}

float UOpenDriveComponent::GetS() const {
	return OdrPosition().GetS() * MetersToUu();
}

float UOpenDriveComponent::GetT() const {
	return OdrPosition().GetOffset() * MetersToUu();
}

float UOpenDriveComponent::GetH() const {
	return FMath::RadiansToDegrees(OdrPosition().GetHRelativeDrivingDirection());
}

int UOpenDriveComponent::GetJunctionId() const {
	roadmanager::Road* r = OdrPosition().GetRoad();
	return r ? r->GetJunction() : -1;
}

float UOpenDriveComponent::GetNextJunctionDistance() const {
	return OdrPosition().GetNextJunction().distance * MetersToUu();
}

int UOpenDriveComponent::GetNextJunctionId() const {
	return OdrPosition().GetNextJunction().junction->GetId();
}

bool UOpenDriveComponent::IsJunctionDistanceLessThan(float Dist, int JunctionId) const {
	Dist *= UuToMeters();
	roadmanager::Position::NextJunction next = OdrPosition().GetNextJunction();
	bool isLess = next.distance < Dist;
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
		return diff.ds * MetersToUu();
	} else {
		return std::numeric_limits<double>::quiet_NaN();
	}
}
