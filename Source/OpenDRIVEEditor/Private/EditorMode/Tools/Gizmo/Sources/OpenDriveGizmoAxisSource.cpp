#include "OpenDriveGizmoAxisSource.h"

void UOpenDriveGizmoAxisSource::Initialize()
{
	OnDirectionChanged.BindUObject(this, &UOpenDriveGizmoAxisSource::SetDirection);
}

FVector UOpenDriveGizmoAxisSource::GetOrigin() const
{
	return Component->GetComponentLocation();
}

FVector UOpenDriveGizmoAxisSource::GetDirection() const
{
	if (bFreezeDirection)
	{
		return Direction;
	}
	/*
	auto CheckRotation = [&](const FTransform& WorldTransform, const FTransform& OpenDriveTransform)-> bool
	{
		const FVector OpenDriveForwardVector = OpenDriveTransform.GetRotation().GetForwardVector();
		const FVector WorldTransformForwardVector = WorldTransform.GetRotation().GetForwardVector();
	
		const float DotProduct = FVector::DotProduct(WorldTransformForwardVector, OpenDriveForwardVector);
	
		const float DotThreshold = FMath::Cos(45.0f);
		
		return DotProduct < -DotThreshold;
	};
	*/
	
	const FTransform WorldTransform = Component->GetComponentTransform();
	OpenDrivePosition->SetTransform(WorldTransform);
	OpenDrivePosition->SetH(0.);
	FTransform OpenDriveTransform = OpenDrivePosition->GetTransform();

	/*
	if (CheckRotation(WorldTransform, OpenDriveTransform))
	{
		const FQuat AdjustedRotation = FQuat(FVector::UpVector, PI) * OpenDriveTransform.GetRotation();
		OpenDriveTransform.SetRotation(AdjustedRotation);
	}
	*/

	FVector NewDirection = Direction;
	
	switch (AxisIndex)
	{
		case 0:
			NewDirection = OpenDriveTransform.GetRotation().GetForwardVector();
			OnDirectionChanged.Execute(NewDirection);
			return NewDirection;
		case 1:
			NewDirection = OpenDriveTransform.GetRotation().GetRightVector();
			OnDirectionChanged.Execute(NewDirection);
        	return NewDirection;
		case 2:
			NewDirection = OpenDriveTransform.GetRotation().GetRightVector();
			OnDirectionChanged.Execute(NewDirection);
			return NewDirection;
		default:
			return NewDirection;
	}
}

bool UOpenDriveGizmoAxisSource::HasTangentVectors() const
{
	return false;
}

void UOpenDriveGizmoAxisSource::GetTangentVectors(FVector& TangentXOut, FVector& TangentYOut) const
{
	IGizmoAxisSource::GetTangentVectors(TangentXOut, TangentYOut);
}
