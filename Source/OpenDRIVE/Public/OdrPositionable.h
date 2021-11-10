#ifndef _ODR_POSITIONABLE_H_
#define _ODR_POSITIONABLE_H_

#include <array>

#include "RoadManager.hpp"

// An object that can have a position in the OpenDRIVE reference frame (track coordinate)
class OPENDRIVE_API odrPositionable {

public:
	typedef std::array<double, 6> XyzPos;

private:
	// Cache for position
	mutable roadmanager::Position _TrackPosCache;
	mutable XyzPos _XyzPosCache;

public:
	odrPositionable();

	virtual ~odrPositionable();

	// Returns the position as x, y, z, h, p, r (see RoadManager)
	virtual XyzPos Position() const = 0;

	//! If possible, returns the S distance between this and the given positionable.
	double SDistanceTo(const odrPositionable* Other) const;

	// Returns the position in Virtual Driver environment, i.e. track and inertial coordinates
	roadmanager::Position OdrPosition() const;

	// Sets the VdPosition as the given track position
	// This method is used to initialize the position within the internal representation, avoiding
	// potential errors in XYZ2Track conversion. Therefore, this method doesn't move the car to the
	// given XYZ position.
	void SetOdrPosition(const roadmanager::Position &Pos);
};

#endif
