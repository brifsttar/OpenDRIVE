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
	
	const FTransform WorldTransform = Component->GetComponentTransform();
	const FTransform WorldTransformNoRot(FQuat::Identity, WorldTransform.GetLocation(), WorldTransform.GetScale3D());
	OpenDrivePosition->SetTransform(WorldTransformNoRot);
	OpenDrivePosition->SetH(0.);
	const FTransform OpenDriveTransform = OpenDrivePosition->GetTransform();
	FVector NewDirection = Direction;
	
	switch (AxisIndex)
	{
		case 0: //s
			NewDirection = OpenDriveTransform.GetRotation().GetForwardVector();
			OnDirectionChanged.Execute(NewDirection);
			return NewDirection;
		case 1: //t
			NewDirection = OpenDriveTransform.GetRotation().GetRightVector();
			OnDirectionChanged.Execute(NewDirection);
        	return NewDirection;
		case 2: //lane 
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
