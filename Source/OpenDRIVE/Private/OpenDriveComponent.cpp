// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveComponent.h"
#include "CoordTranslate.h"


// Sets default values for this component's properties
UOpenDriveComponent::UOpenDriveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDriveComponent::BeginPlay()
{
	Super::BeginPlay();
	ResetOdrComp();
}


// Called every frame
void UOpenDriveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UOpenDriveComponent::ResetOdrComp() {
	_Odr = MakeUnique<odrUePositionable>(dynamic_cast<AActor*>(GetOwner()));
}

void UOpenDriveComponent::GetPosition() {
	if (!GetOwner()) return;
	if (!_Odr) ResetOdrComp();
	roadmanager::Position p = _Odr->OdrPosition();
	RoadId_ = p.GetTrackId();
	LaneId_ = p.GetLaneId();
	S_ = p.GetS() * MetersToUu();
	T_ = p.GetOffset() * MetersToUu();
	H_ = FMath::RadiansToDegrees(p.GetHRelativeDrivingDirection());
}

void UOpenDriveComponent::SetPosition() {
	if (!GetOwner()) return;
	if (!_Odr) ResetOdrComp();
	SetTrackPosition(RoadId_, LaneId_, S_, T_, H_);
}

void UOpenDriveComponent::SetTrackPosition(const roadmanager::Position &p) {
	AActor* owner = dynamic_cast<AActor*>(GetOwner());
	owner->SetActorTransform(CoordTranslate::OdrToUe::ToTransfrom(p));
	_Odr->SetOdrPosition(p);
}

void UOpenDriveComponent::SetTrackPosition(int TrackId, int LaneId, float S, float Offset, float H) {
	roadmanager::Position p(TrackId, LaneId, S * UuToMeters(), Offset * UuToMeters());
	p.SetHeadingRelative(FMath::DegreesToRadians(H));
	SetTrackPosition(p);
}

bool UOpenDriveComponent::MoveAlongS(float S, int Strategy) {
	if (!_Odr) ResetOdrComp();
	roadmanager::Position p = _Odr->OdrPosition();
	roadmanager::Position::ErrorCode ret;
	ret = p.MoveAlongS(S * UuToMeters(), 0., roadmanager::Junction::JunctionStrategyType(Strategy));
	SetTrackPosition(p);
	return ret == roadmanager::Position::ErrorCode::ERROR_NO_ERROR;
}

void UOpenDriveComponent::ResetPosition() {
	ResetOdrComp();
}

int UOpenDriveComponent::GetRoadId() const {
	if (!_Odr) return -1;
	return _Odr->OdrPosition().GetTrackId();
}

int UOpenDriveComponent::GetLaneId() const {
	if (!_Odr) return -1;
	return _Odr->OdrPosition().GetLaneId();
}

float UOpenDriveComponent::GetS() const {
	if (!_Odr) return NAN;
	return _Odr->OdrPosition().GetS() * MetersToUu();
}

float UOpenDriveComponent::GetT() const {
	if (!_Odr) return NAN;
	return _Odr->OdrPosition().GetOffset() * MetersToUu();
}

float UOpenDriveComponent::GetH() const {
	if (!_Odr) return NAN;
	return FMath::RadiansToDegrees(_Odr->OdrPosition().GetHRelativeDrivingDirection());
}

int UOpenDriveComponent::GetJunctionId() const {
	if (!_Odr) return -1;
	roadmanager::Road* r = _Odr->OdrPosition().GetRoad();
	return r ? r->GetJunction() : -1;
}

float UOpenDriveComponent::GetNextJunctionDistance() const {
	if (!_Odr) return NAN;
	return _Odr->OdrPosition().GetNextJunction().distance * MetersToUu();
}

int UOpenDriveComponent::GetNextJunctionId() const {
	if (!_Odr) return -1;
	return _Odr->OdrPosition().GetNextJunction().junction->GetId();
}

bool UOpenDriveComponent::IsJunctionDistanceLessThan(float Dist, int JunctionId) const {
	Dist *= UuToMeters();
	roadmanager::Position::NextJunction next = _Odr->OdrPosition().GetNextJunction();
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
	return _Odr->SDistanceTo(Other->GetOdr()) * MetersToUu();
}
