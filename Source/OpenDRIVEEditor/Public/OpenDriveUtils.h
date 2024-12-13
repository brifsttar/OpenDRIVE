#pragma once
#include "CoreMinimal.h"
#include "Engine/DecalActor.h"

enum EAlignmentMethod : int
{
	NoAlignment = 0,
	AlignButKeepDirection = 1,
	AlignToLane = 2,
};

class FOpenDriveUtils
{
public:

	/*
	* Used to correct Actor's rotation if they are Decals
	* @param ActorIn The Actor original Transform
	* @return The corrected Actor Transform 
	*/
	static FTransform UEToOdr(const AActor* ActorIn)
	{
		const FTransform ActorTransform = ActorIn->GetTransform();
		return IsValid(Cast<ADecalActor>(ActorIn)) ?
			FTransform(ActorTransform.TransformRotation(FRotator(180, -90.0, 90.0).Quaternion()), ActorTransform.GetLocation(), ActorTransform.GetScale3D()) :
			ActorTransform;
	}

	/**
 	* Used to convert an OpenDrivePosition's Transform before settings the related Actor's Transform (for decals). 
 	* @param ActorIn 
 	* @param OpenDriveTransformIn
 	* @param AlignmentIn
 	* @param bTakeOdrZHeight Sets this at true to get OpenDriveTransform Z Location and not Actor Z Location
 	* @return A World Transform 
	*/
	static FTransform OdrToUE(const AActor* ActorIn, FTransform& OpenDriveTransformIn, const EAlignmentMethod AlignmentIn, const bool bTakeOdrZHeight = false)
	{
		const FTransform ActorTransform = ActorIn->GetTransform();
		const float ZHeight = ActorTransform.GetLocation().Z;
		const FVector Location = bTakeOdrZHeight ? OpenDriveTransformIn.GetLocation() : FVector(OpenDriveTransformIn.GetLocation().X, OpenDriveTransformIn.GetLocation().Y , ZHeight);
		
		switch (AlignmentIn)
		{
			case NoAlignment:
				OpenDriveTransformIn.SetRotation(ActorTransform.GetRotation());
				break;
			case AlignButKeepDirection:
				if (CheckAngleBetweenActorAndOdr(ActorTransform, OpenDriveTransformIn))
				{
					const FQuat ModifiedRotation = OpenDriveTransformIn.GetRotation();
					const FQuat AdjustedRotation = FQuat(FVector::UpVector, PI) * ModifiedRotation;
					OpenDriveTransformIn.SetRotation(AdjustedRotation);
				}
				break;
			case AlignToLane:
				break;
		}
		
		return IsValid(Cast<ADecalActor>(ActorIn)) ?
			FTransform(OpenDriveTransformIn.TransformRotation(FRotator(90,180,-90).Quaternion()),Location, ActorTransform.GetScale3D()) :
			FTransform(OpenDriveTransformIn.GetRotation(), Location, ActorTransform.GetScale3D());
	}

	/**
	 * Used to convert an OpenDrivePosition's Transform before settings the related Actor's Transform.
	 * WONT HANDLE DECALS ROTATION
	 * @param ActorTransformIn 
	 * @param OpenDriveTransformIn 
	 * @param AlignmentIn
	 * @param bTakeOdrZHeight Sets this at true to get OpenDriveTransform Z Location and not Actor Z Location
	 * @return 
	 */
	static FTransform OdrToUE(const FTransform& ActorTransformIn, FTransform& OpenDriveTransformIn, const EAlignmentMethod AlignmentIn, const bool bTakeOdrZHeight = false)
	{
		const float ZHeight = ActorTransformIn.GetLocation().Z;
		const FVector Location = bTakeOdrZHeight ? OpenDriveTransformIn.GetLocation() : FVector(OpenDriveTransformIn.GetLocation().X, OpenDriveTransformIn.GetLocation().Y , ZHeight);
		
		switch (AlignmentIn)
		{
		case NoAlignment:
			OpenDriveTransformIn.SetRotation(ActorTransformIn.GetRotation());
			break;
		case AlignButKeepDirection:
			if (CheckAngleBetweenActorAndOdr(ActorTransformIn, OpenDriveTransformIn))
			{
				const FQuat ModifiedRotation = OpenDriveTransformIn.GetRotation();
				const FQuat AdjustedRotation = FQuat(FVector::UpVector, PI) * ModifiedRotation;
				OpenDriveTransformIn.SetRotation(AdjustedRotation);
			}
			break;
		case AlignToLane:
			break;
		}
		
		return FTransform(OpenDriveTransformIn.GetRotation(), Location, ActorTransformIn.GetScale3D());
	}

	static bool CheckAngleBetweenActorAndOdr(const FTransform& ActorTransformIn, const FTransform& OpenDriveTransformIn, float AngleThresholdIn = 45.0f)
	{
		const FVector OpenDriveForwardVector = OpenDriveTransformIn.GetRotation().GetForwardVector();
		const FVector ActorTransformForwardVector = ActorTransformIn.GetRotation().GetForwardVector();
		const float DotProduct = FVector::DotProduct(ActorTransformForwardVector, OpenDriveForwardVector);
		const float DotThreshold = FMath::Cos(AngleThresholdIn);
		return DotProduct < -DotThreshold;
	}

};
