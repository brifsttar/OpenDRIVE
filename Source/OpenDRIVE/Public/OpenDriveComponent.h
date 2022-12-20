#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CoordTranslate.h"
#include "OpenDriveComponent.generated.h"

using CoordTranslate::UuToMeters;
using CoordTranslate::MetersToUu;

UCLASS( ClassGroup=(OpenDRIVE), meta=(BlueprintSpawnableComponent, ShortTooltip = "Manipulate OpenDRIVE coordinate (e.g. road, S, T)") )
class OPENDRIVE_API UOpenDriveComponent : public USceneComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class UOpenDrivePosition* _TrackPosition;

	void MovePositionToActor() const;
	void MoveActorToPosition();

	class UOpenDrivePosition* GetTrackPosition() const;

public:
	UOpenDriveComponent();

	virtual roadmanager::Position OdrPosition() const;

	void SetTrackPosition(const roadmanager::Position &p);

	/**
	* Computes the current OpenDRIVE position and updates the public position variables accordingly
	*/
	UFUNCTION(BlueprintCallable, CallInEditor, meta = (Category = "OpenDRIVE", DeprecatedFunction))
	void GetPosition();

	/**
	* Sets the OpenDRIVE position based on the public position variables values
	*/
	UFUNCTION(BlueprintCallable, CallInEditor, meta = (Category = "OpenDRIVE", DeprecatedFunction))
	void SetPosition();

	/**
	* Sets the OpenDRIVE position
	* @param TrackId Track (road) ID
	* @param LaneId  Lane ID
	* @param S       S-coordinate (cm) along the road
	* @param Offset  Lateral offset (cm) from the lane center
	* @param H       Heading offset (°) from the lane orientation
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	void SetTrackPosition(int TrackId, int LaneId, float S, float Offset, float H);

	/**
	* Moves the object along its S coordinate
	* @param  S       S-coordinate offset (cm)
	* @param Strategy Strategy to follow in case of junctions (-1=left, 0=forward, 1=right)
	* @return Success
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	bool MoveAlongS(float S, int Strategy=0);

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

	/**
	* Returns the current S-coordinate (cm)
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	float GetS() const;

	/**
	* Returns the current lateral offset (cm) from the lane center
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	float GetT() const;

	/**
	* Returns the current heading offset (°) from the lane orientation
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	float GetH() const;

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
	float SDistanceTo(const UOpenDriveComponent* Other) const;

	/**
	* Returns the delta to the other object
	* @param Other Other object
	* @param Ds Delta in S coordinate
	* @param Dt Delta in T coordinate
	* @param DLaneId Delta in lane ID
	* @return Whether the delta was resolved
	*/
	UFUNCTION(BlueprintCallable, meta = (Category = "OpenDRIVE"))
	bool Delta(const UOpenDriveComponent* Other, float& Ds, float& Dt, int& DLaneId) const;
};
