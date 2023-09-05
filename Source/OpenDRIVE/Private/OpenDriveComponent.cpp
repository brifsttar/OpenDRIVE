#include "OpenDriveComponent.h"

UOpenDriveComponent::UOpenDriveComponent() {
	_TrackPosition = CreateDefaultSubobject<UOpenDrivePosition>(FName("TrackPosition"));
	PrimaryComponentTick.bCanEverTick = true;
}

void UOpenDriveComponent::MovePositionToActor() const {
	if (!GetOwner()) return;
	_TrackPosition->SetTransform(GetOwner()->GetActorTransform());
}

void UOpenDriveComponent::MoveActorToPosition() {
	if (!GetOwner()) return;
	GetOwner()->SetActorTransform(_TrackPosition->GetTransform());
}

UOpenDrivePosition* UOpenDriveComponent::GetTrackPosition() const {
	MovePositionToActor();
	return _TrackPosition;
}

void UOpenDriveComponent::GetPosition() {

}

void UOpenDriveComponent::SetPosition() {

}

roadmanager::Position UOpenDriveComponent::OdrPosition() const {
	return GetTrackPosition()->OdrPosition();
}

void UOpenDriveComponent::SetTrackPosition(const roadmanager::Position &p) {
	GetTrackPosition()->SetTrackPosition(p);
	MoveActorToPosition();
}

void UOpenDriveComponent::SetTrackPosition(int TrackId, int LaneId, float S, float Offset, float H) {
	GetTrackPosition()->SetTrackPosition(TrackId, LaneId, S, Offset, H);
	MoveActorToPosition();
}

bool UOpenDriveComponent::MoveAlongS(float S, int Strategy) {
	bool r = GetTrackPosition()->MoveAlongS(S, Strategy);
	if (r) MoveActorToPosition();
	return r;
}

void UOpenDriveComponent::ResetPosition() {
	GetTrackPosition()->ResetPosition();
}

int UOpenDriveComponent::GetRoadId() const {
	return GetTrackPosition()->GetRoadId();
}

int UOpenDriveComponent::GetLaneId() const {
	return GetTrackPosition()->GetLaneId();
}

float UOpenDriveComponent::GetS() const {
	return GetTrackPosition()->GetS();
}

float UOpenDriveComponent::GetT() const {
	return GetTrackPosition()->GetT();
}

float UOpenDriveComponent::GetH() const {
	return GetTrackPosition()->GetH();
}

float UOpenDriveComponent::GetLaneWidth() const {
	return GetTrackPosition()->GetLaneWidth();
}

int UOpenDriveComponent::GetJunctionId() const {
	return GetTrackPosition()->GetJunctionId();
}

float UOpenDriveComponent::GetNextJunctionDistance() const {
	return GetTrackPosition()->GetNextJunctionDistance();
}

int UOpenDriveComponent::GetNextJunctionId() const {
	return GetTrackPosition()->GetNextJunctionId();
}

bool UOpenDriveComponent::IsJunctionDistanceLessThan(float Dist, int JunctionId) const {
	return GetTrackPosition()->IsJunctionDistanceLessThan(Dist, JunctionId);
}

float UOpenDriveComponent::SDistanceTo(const UOpenDriveComponent* Other) const {
	if (!IsValid(Other)) return NAN;
	return GetTrackPosition()->SDistanceTo(Other->GetTrackPosition());
}

bool UOpenDriveComponent::Delta(const UOpenDriveComponent* Other, float& Ds, float& Dt, int& DLaneId) const {
	if (!IsValid(Other)) return false;
	return GetTrackPosition()->Delta(Other->GetTrackPosition(), Ds, Dt, DLaneId);
}
