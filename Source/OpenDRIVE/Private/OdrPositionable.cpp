#include "odrPositionable.h"

odrPositionable::odrPositionable() {
	_TrackPosCache.SetSnapLaneTypes(roadmanager::Lane::LaneType::LANE_TYPE_ANY);
}

odrPositionable::~odrPositionable() {

}

double odrPositionable::SDistanceTo(const odrPositionable* Other) const {
	roadmanager::PositionDiff diff;
	roadmanager::Position otherPos = Other->OdrPosition();
	if(OdrPosition().Delta(&otherPos, diff)) {
		return diff.ds;
	} else {
		return std::numeric_limits<double>::quiet_NaN();
	}
}

roadmanager::Position odrPositionable::OdrPosition() const {
	XyzPos pos = Position();
	if(pos != _XyzPosCache) {
		_XyzPosCache = pos;
		_TrackPosCache.SetInertiaPos(pos[0], pos[1], pos[2], pos[3], pos[4], pos[5]);
	}
	return _TrackPosCache;
}

void odrPositionable::SetOdrPosition(const roadmanager::Position &Pos) {
	_TrackPosCache = Pos;
	_TrackPosCache.SetSnapLaneTypes(roadmanager::Lane::LaneType::LANE_TYPE_ANY);
}
