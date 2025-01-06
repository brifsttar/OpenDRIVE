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
	_TrackPos.SetHintRoad(_HintRoad);
	_TrackPos.SetInertiaPosMode(p.X, p.Y, p.Z, r.X, r.Y, r.Z,
		roadmanager::Position::PosMode::Z_ABS |
		roadmanager::Position::PosMode::H_ABS |
		roadmanager::Position::PosMode::P_ABS |
		roadmanager::Position::PosMode::R_ABS
	);
}

FTransform UOpenDrivePosition::GetTransform() const {
	return CoordTranslate::OdrToUe::ToTransfrom(_TrackPos);
}

void UOpenDrivePosition::SetTrackPosition(int TrackId, int LaneId, float S, float Offset, float H) {
	roadmanager::Position p(TrackId, LaneId, UuToMeters(S), UuToMeters(Offset));
	p.SetHeading(FMath::DegreesToRadians(H) + p.GetDrivingDirection());
	SetTrackPosition(p);
}

void UOpenDrivePosition::UpdateTrackPosition(int TrackId, float S, float T) {
	_TrackPos.SetTrackPos(TrackId, UuToMeters(S), UuToMeters(T));
}

bool UOpenDrivePosition::MoveAlongS(float S, int Strategy) {
	roadmanager::Position p = OdrPosition();
	p.SetMode(
		roadmanager::Position::PosModeType::UPDATE,
		roadmanager::Position::PosMode::Z_REL |
		roadmanager::Position::PosMode::H_REL |
		roadmanager::Position::PosMode::P_REL |
		roadmanager::Position::PosMode::R_REL
	);
	roadmanager::Position::ReturnCode ret;
	ret = p.MoveAlongS(
		UuToMeters(S),
		0.,
		roadmanager::Junction::JunctionStrategyType(Strategy),
		false,
		roadmanager::Position::MoveDirectionMode::HEADING_DIRECTION,
		true
	);
	SetTrackPosition(p);
	return ret >= roadmanager::Position::ReturnCode::OK;
}

bool UOpenDrivePosition::MoveAlongLanes(int LaneOffset, LaneType LaneFilter) {
	if (LaneOffset == 0) return false;
	roadmanager::Position p = OdrPosition();
	roadmanager::LaneSection* ls = p.GetRoad()->GetLaneSectionByS(p.GetS());
	roadmanager::Lane* targetLane = nullptr;
	int laneId = p.GetLaneId();
	int currentLaneOffset = 0;
	int laneOffsetUnit = LaneOffset / abs(LaneOffset);

	roadmanager::Lane::LaneType laneTypeFilter = (roadmanager::Lane::LaneType)LaneFilter;
	for (
		int i = laneId + laneOffsetUnit;
		currentLaneOffset != LaneOffset;
		i += laneOffsetUnit
	) {
		targetLane = ls->GetLaneById(i);
		if (targetLane == nullptr) break;
		if (targetLane->GetLaneType() == laneTypeFilter) {
			currentLaneOffset += laneOffsetUnit;
		}
	}
	if (targetLane == nullptr) return false;
	p.SetLanePos(p.GetTrackId(), targetLane->GetId(), p.GetS(), p.GetOffset());
	p.SetHeading(p.GetDrivingDirection());
	SetTrackPosition(p);
	return true;
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

void UOpenDrivePosition::SetLaneById(int NewLaneId)
{
	roadmanager::Position p = OdrPosition();
	p.SetLaneId(NewLaneId);
	p.MoveAlongS(0.);
	SetTrackPosition(p);
}

float UOpenDrivePosition::GetS() const {
	return MetersToUu(OdrPosition().GetS());
}

void UOpenDrivePosition::SetS(float S) {
	_TrackPos.SetTrackPos(GetRoadId(), UuToMeters(S), _TrackPos.GetT()); //Used to force Coordinates update
}

float UOpenDrivePosition::GetT() const {
	return MetersToUu(OdrPosition().GetOffset());
}

void UOpenDrivePosition::SetT(float T) {
	roadmanager::Position p = OdrPosition();
	p.SetOffset(UuToMeters(T));
	// Ugly trick to force refresh of internal T value
	p.MoveAlongS(0.);
	SetTrackPosition(p);
}

void UOpenDrivePosition::SetRealT(float T)
{
	_TrackPos.SetTrackPos(GetRoadId(), _TrackPos.GetS(), UuToMeters(T));  //Used to force Coordinates update
}

void UOpenDrivePosition::SetOffset(const float Offset)
{
	roadmanager::Position p = OdrPosition();
	p.SetOffset(UuToMeters(Offset));
	// Ugly trick to force refresh of internal T value
	p.MoveAlongS(0.);
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

LaneType UOpenDrivePosition::GetLaneType() const {
	return (LaneType)OdrPosition().GetLane()->GetLaneType();
}

float UOpenDrivePosition::GetRoadCurvatureAngle(float LookaheadDist) const {
	float h1 = OdrPosition().GetH();
	roadmanager::Position p2 = OdrPosition();
	p2.MoveAlongS(CoordTranslate::UuToMeters(LookaheadDist));
	float h2 = p2.GetH();
	return FMath::RadiansToDegrees(h2 - h1);
}
