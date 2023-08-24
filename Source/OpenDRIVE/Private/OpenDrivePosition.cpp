#include "OpenDrivePosition.h"

UOpenDrivePosition::UOpenDrivePosition() {
	_TrackPos.SetSnapLaneTypes(roadmanager::Lane::LaneType::LANE_TYPE_ANY);
}

void UOpenDrivePosition::InvalidateCache() {
	_InertialPosCache = FTransform();
}

void UOpenDrivePosition::SetTrackPosition(const roadmanager::Position& p) {
	InvalidateCache();
	_TrackPos = p;
	_TrackPos.SetSnapLaneTypes(roadmanager::Lane::LaneType::LANE_TYPE_ANY);
}

roadmanager::Position UOpenDrivePosition::OdrPosition() const {
	return _TrackPos;
}

void UOpenDrivePosition::SetTransform(const FTransform& T) {
	if (T.Equals(_InertialPosCache)) return;
	_InertialPosCache = T;
	FVector p = CoordTranslate::UeToOdr::Location(T.GetLocation());
	FVector r = CoordTranslate::UeToOdr::Rotation(T.Rotator().Euler());
	_TrackPos.SetX(p.X);
	_TrackPos.SetY(p.Y);
	//_TrackPos.SetZ(p.Z);
	_TrackPos.XYZH2TrackPos(p.X, p.Y, p.Z, _TrackPos.GetH(), false, -1, false, _HintRoad);
	_TrackPos.SetHeading(r.X);
	_TrackPos.SetPitch(r.Y);
	_TrackPos.SetRoll(r.Z);
	_TrackPos.EvaluateOrientation();
}

FTransform UOpenDrivePosition::GetTransform() const {
	return CoordTranslate::OdrToUe::ToTransfrom(_TrackPos);
}

void UOpenDrivePosition::SetTrackPosition(int TrackId, int LaneId, float S, float Offset, float H) {
	roadmanager::Position p(TrackId, LaneId, UuToMeters(S), UuToMeters(Offset));
	p.SetHeading(FMath::DegreesToRadians(H) + p.GetDrivingDirection());
	SetTrackPosition(p);
}

bool UOpenDrivePosition::MoveAlongS(float S, int Strategy) {
	roadmanager::Position p = OdrPosition();
	roadmanager::Position::ReturnCode ret;
	ret = p.MoveAlongS(UuToMeters(S), 0., roadmanager::Junction::JunctionStrategyType(Strategy));
	SetTrackPosition(p);
	return ret >= roadmanager::Position::ReturnCode::OK;
}

void UOpenDrivePosition::ResetPosition() {
	SetTrackPosition(roadmanager::Position());
}

int UOpenDrivePosition::GetRoadId() const {
	return OdrPosition().GetTrackId();
}

int UOpenDrivePosition::GetLaneId() const {
	return OdrPosition().GetLaneId();
}

float UOpenDrivePosition::GetS() const {
	return MetersToUu(OdrPosition().GetS());
}

float UOpenDrivePosition::GetT() const {
	return MetersToUu(OdrPosition().GetOffset());
}

void UOpenDrivePosition::SetT(float T) {
	roadmanager::Position p = OdrPosition();
	p.SetOffset(UuToMeters(T));
	SetTrackPosition(p);
}

float UOpenDrivePosition::GetH() const {
	return FMath::RadiansToDegrees(OdrPosition().GetHRelativeDrivingDirection());
}

void UOpenDrivePosition::SetH(float H) {
	roadmanager::Position p = OdrPosition();
	p.SetHeading(FMath::DegreesToRadians(H) + p.GetDrivingDirection());
	SetTrackPosition(p);
}

float UOpenDrivePosition::GetLaneWidth() const {
	roadmanager::Position p = OdrPosition();
	double s = p.GetS();
	const roadmanager::Road* r = p.GetRoad();
	if (r == nullptr) return NAN;
	const roadmanager::LaneSection* ls = r->GetLaneSectionByS(s);
	if (ls == nullptr) return NAN;
	return MetersToUu(ls->GetWidth(s, p.GetLaneId()));
}

int UOpenDrivePosition::GetJunctionId() const {
	roadmanager::Road* r = OdrPosition().GetRoad();
	return r ? r->GetJunction() : -1;
}

float UOpenDrivePosition::GetNextJunctionDistance() const {
	return MetersToUu(OdrPosition().GetNextJunction().distance);
}

int UOpenDrivePosition::GetNextJunctionId() const {
	return OdrPosition().GetNextJunction().junction->GetId();
}

bool UOpenDrivePosition::IsJunctionDistanceLessThan(float Dist, int JunctionId) const {
	roadmanager::Position::NextJunction next = OdrPosition().GetNextJunction();
	bool isLess = next.distance < UuToMeters(Dist);
	if (JunctionId == -1) {
		return isLess;
	} else {
		return isLess && next.junction && next.junction->GetId() == JunctionId;
	}
}

float UOpenDrivePosition::SDistanceTo(const UOpenDrivePosition* Other) const {
	if (!IsValid(Other)) return NAN;
	roadmanager::PositionDiff diff;
	roadmanager::Position otherPos = Other->OdrPosition();
	if (OdrPosition().Delta(&otherPos, diff)) {
		return MetersToUu(diff.ds);
	} else {
		return NAN;
	}
}

bool UOpenDrivePosition::Delta(const UOpenDrivePosition* Other, float& Ds, float& Dt, int& DLaneId) const {
	if (!IsValid(Other)) return false;
	roadmanager::PositionDiff diff;
	roadmanager::Position otherPos = Other->OdrPosition();
	bool ret = OdrPosition().Delta(&otherPos, diff);
	Ds = MetersToUu(diff.ds);
	Dt = MetersToUu(diff.dt);
	DLaneId = diff.dLaneId;
	return ret;
}

void UOpenDrivePosition::AlignWithLaneCenter() {
	SetTrackPosition(GetRoadId(), GetLaneId(), GetS(), 0.f, 0.f);
}
