#pragma once
#include "CoreMinimal.h"
#include "CoordTranslate.h"
#include "OpenDrivePosition.generated.h"

using CoordTranslate::UuToMeters;
using CoordTranslate::MetersToUu;

UENUM(BlueprintType)
enum LaneType : int64
{
	NONE            = roadmanager::Lane::LaneType::LANE_TYPE_NONE,
	DRIVING         = roadmanager::Lane::LaneType::LANE_TYPE_DRIVING,
	STOP            = roadmanager::Lane::LaneType::LANE_TYPE_STOP,
	SHOULDER        = roadmanager::Lane::LaneType::LANE_TYPE_SHOULDER,
	BIKING          = roadmanager::Lane::LaneType::LANE_TYPE_BIKING,
	SIDEWALK        = roadmanager::Lane::LaneType::LANE_TYPE_SIDEWALK,
	BORDER          = roadmanager::Lane::LaneType::LANE_TYPE_BORDER,
	RESTRICTED      = roadmanager::Lane::LaneType::LANE_TYPE_RESTRICTED,
	PARKING         = roadmanager::Lane::LaneType::LANE_TYPE_PARKING,
	BIDIRECTIONAL   = roadmanager::Lane::LaneType::LANE_TYPE_BIDIRECTIONAL,
	MEDIAN          = roadmanager::Lane::LaneType::LANE_TYPE_MEDIAN,
	SPECIAL1        = roadmanager::Lane::LaneType::LANE_TYPE_SPECIAL1,
	SPECIAL2        = roadmanager::Lane::LaneType::LANE_TYPE_SPECIAL2,
	SPECIAL3        = roadmanager::Lane::LaneType::LANE_TYPE_SPECIAL3,
	ROADMARKS       = roadmanager::Lane::LaneType::LANE_TYPE_ROADMARKS,
	TRAM            = roadmanager::Lane::LaneType::LANE_TYPE_TRAM,
	RAIL            = roadmanager::Lane::LaneType::LANE_TYPE_RAIL,
	ENTRY           = roadmanager::Lane::LaneType::LANE_TYPE_ENTRY,
	EXIT            = roadmanager::Lane::LaneType::LANE_TYPE_EXIT,
	OFF_RAMP        = roadmanager::Lane::LaneType::LANE_TYPE_OFF_RAMP,
	ON_RAMP         = roadmanager::Lane::LaneType::LANE_TYPE_ON_RAMP,
	CURB            = roadmanager::Lane::LaneType::LANE_TYPE_CURB,
	CONNECTING_RAMP = roadmanager::Lane::LaneType::LANE_TYPE_CONNECTING_RAMP,
	REFERENCE_LINE  = roadmanager::Lane::LaneType::LANE_TYPE_REFERENCE_LINE,
	ANY_DRIVING     = roadmanager::Lane::LaneType::LANE_TYPE_ANY_DRIVING,
	ANY_ROAD        = roadmanager::Lane::LaneType::LANE_TYPE_ANY_ROAD,
	ANY             = roadmanager::Lane::LaneType::LANE_TYPE_ANY,
	INVALID         = 0
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OPENDRIVE_API UOpenDrivePosition : public UObject {
	GENERATED_BODY()

protected:
	roadmanager::Position _TrackPos;

	UPROPERTY()
	FTransform _InertialPosCache;

	UPROPERTY()
	int _HintRoad = -1;

	void InvalidateCache();

public:
	UOpenDrivePosition();

	/**
	* Returns a copy of the wrapped position object
	*/
	virtual roadmanager::Position OdrPosition() const;

	void SetTrackPosition(const roadmanager::Position& p);

	UFUNCTION(meta = (Category = "OpenDRIVE"))
	void SetHintRoad(int HintRoad) { _HintRoad = HintRoad; }

	/**
	* Computes the current OpenDRIVE position based on the input transform
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	void SetTransform(const FTransform &T);

	/**
	* Get the transform based on the current OpenDRIVE position
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	FTransform GetTransform() const;

	/**
	* Sets the OpenDRIVE position
	* @param TrackId Track (road) ID
	* @param LaneId  Lane ID
	* @param S       S-coordinate (cm) along the road
	* @param Offset  Lateral offset (cm) from the lane center
	* @param H       Heading offset (�) from the lane orientation
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	void SetTrackPosition(int TrackId, int LaneId, float S, float Offset, float H);

	/**
	* Sets the OpenDRIVE position
	* @param TrackId Track (road) ID
	* @param S       S-coordinate (cm) along the road
	* @param T       T-coordinate (cm) along the road
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	void UpdateTrackPosition(int TrackId, float S, float T);

	/**
	* Moves the object along its S coordinate
	* @param  S       S-coordinate offset (cm)
	* @param Strategy Strategy to follow in case of junctions (-1=left, 0=forward, 1=right)
	* @return Success
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	bool MoveAlongS(float S, int Strategy=0);

	/**
	* Moves the object along the lanes on the road
	* @param LaneOffset Lane Offset
	* @param LaneFilter Which type of lane to include in the move
	* @param bClamp     If true, offset will be clamped at max possible value
	* TODO: Set LaneFilter default to ANY, which isn't currently possible due to UnrealPython not
	*       handling signed (or non-uint8) enums. It causes a crash at startup.
	* @return Success
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	bool MoveAlongLanes(
		int LaneOffset,
		LaneType LaneFilter/*=ANY*/,
		bool bClamp=false
	);

	/**
	* Resets the OpenDRIVE position buffer, in case the object was moved away from its previous position
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	void ResetPosition();

	/**
	* Returns the current Road ID
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	int GetRoadId() const;

	/**
	* Returns the current Lane ID
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	int GetLaneId() const;

	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	void SetLaneById(int NewLaneId);

	/**
	* Returns the current S-coordinate (cm)
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	float GetS() const;

	/**
	 * @param S New S value (cm)
	 */
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	void SetS(float S);

	/**
	* Returns the current lateral offset (cm) from the lane center
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE", DeprecatedFunction, DeprecationMessage = "This method is deprecated, use GetOffset() instead."))
	float GetT() const;

	/**
	 * @param T New offset from lane value (cm)
	 */
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE", DeprecatedFunction, DeprecationMessage = "This method is deprecated, use SetOffset() instead."))
	void SetT(float T);

	/**
	 * @return The lateral offset for reference lane id=0 in centimeters
	 * todo: change function name to GetT()
	 */
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	float GetRealT() const {return MetersToUu(OdrPosition().GetT());}

	/**
	 * Adds a lateral offset from reference lane id=0
	 * @param T The lateral offset (cm) from reference lane id=0
	 * todo: change function name to SetT()
	 */
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	void SetRealT(float T);

	/**
	* @return The lateral offset from current lane center in centimeters 
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	float GetOffset() const {return MetersToUu(OdrPosition().GetOffset());}

	/**
	 * Adds a lateral from current lane center
	 * @param  Offset The desired Offset from current lane (cm)
	 */
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	void SetOffset(float Offset);

	/**
	* Returns the current heading offset (�) from the lane orientation
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	float GetH() const;

	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	void SetH(float H);

	/**
	* Returns the width of the lane at the current position
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	float GetLaneWidth() const;

	/**
	* Returns the current Junction ID, or -1 if N/A
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	int GetJunctionId() const;

	/**
	* Returns the distance (cm) to the next junction
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	float GetNextJunctionDistance() const;

	/**
	* Returns the next junction's ID
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	int GetNextJunctionId() const;

	/**
	* Returns whether the distance to a junction is less than a given distance
	* @param  Dist       Distance (cm) threshold
	* @param  JunctionId Junction Id to measure distance to, or -1 to use the next junction
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	bool IsJunctionDistanceLessThan(float Dist, int JunctionId = -1) const;

	/**
	* Returns the longitudinal distance (cm, also known as "pk" or "S" distance) to another object
	* @param Other Other object
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	float SDistanceTo(const UOpenDrivePosition* Other) const;

	/**
	* Returns the delta to the other object
	* @param Other Other object
	* @param Ds Delta in S coordinate
	* @param Dt Delta in T coordinate
	* @param DLaneId Delta in lane ID
	* @return Whether the delta was resolved
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	bool Delta(const UOpenDrivePosition* Other, float& Ds, float& Dt, int& DLaneId) const;

	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	void AlignWithLaneCenter();

	/**
	 * @return The lane type 
	 */
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	LaneType GetLaneType() const;

	/**
	* @param LookaheadDist Distance in front to use as reference for computation
	* @return Road curvature angle in degrees
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	float GetRoadCurvatureAngle(float LookaheadDist) const;

};
