#include "OpenDriveAxisPositionGizmo.h"
#include "BaseGizmos/GizmoMath.h"
#include "OpenDrivePosition.h"

UInteractiveGizmo* UOpenDriveAxisGizmoBuilder::BuildGizmo(const FToolBuilderState& SceneState) const
{
	UOpenDriveAxisPositionGizmo* NewGizmo = NewObject<UOpenDriveAxisPositionGizmo>(SceneState.GizmoManager);
	return NewGizmo;
}

FInputRayHit UOpenDriveAxisPositionGizmo::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
{
	FInputRayHit GizmoHit;
	if (HitTarget && AxisSource && ParameterSource)
	{
		GizmoHit = HitTarget->IsHit(PressPos);
		if (GizmoHit.bHit)
		{
			LastHitPosition = PressPos.WorldRay.PointAt(GizmoHit.HitDepth);
		}
	}
	return GizmoHit;
}

//starts
void UOpenDriveAxisPositionGizmo::OnClickPress(const FInputDeviceRay& PressPos)
{
	InteractionOrigin = LastHitPosition;
	InteractionAxis = AxisSource->GetDirection();

	// Find interaction start point and parameter.
	FVector NearestPt; float RayNearestParam;
	GizmoMath::NearestPointOnLineToRay(InteractionOrigin, InteractionAxis,
		PressPos.WorldRay.Origin, PressPos.WorldRay.Direction,
		InteractionStartPoint, InteractionStartParameter,
		NearestPt, RayNearestParam);

	FVector AxisOrigin = AxisSource->GetOrigin();

	double DirectionSign = FVector::DotProduct(InteractionStartPoint - AxisOrigin, InteractionAxis);
	ParameterSign = (bEnableSignedAxis && DirectionSign < 0) ? -1.0f : 1.0f;

	// Figure out how the parameter would need to be adjusted to bring the axis origin to the
	// interaction start point. This is used when aligning the axis origin to a custom destination.
	float AxisOriginParamValue;
	GizmoMath::NearestPointOnLine(InteractionOrigin, InteractionAxis, AxisOrigin,
		NearestPt, AxisOriginParamValue);
	InteractionStartAxisOriginParameterOffset = InteractionStartParameter - AxisOriginParamValue;

	InteractionCurPoint = InteractionStartPoint;
	InteractionStartParameter *= ParameterSign;
	InteractionCurParameter = InteractionStartParameter;

	InitialTargetParameter = ParameterSource->GetParameter();
	ParameterSource->BeginModify();

	bInInteraction = true;

	if (HitTarget)
	{
		HitTarget->UpdateInteractingState(bInInteraction);
	}

	if (StateTarget)
	{
		StateTarget->BeginUpdate();
	}
	UE_LOG(LogTemp, Log, TEXT("OnClickPress: Interaction started at %s"), *InteractionStartPoint.ToString());
}

//drag 
void UOpenDriveAxisPositionGizmo::OnClickDrag(const FInputDeviceRay& DragPos)
{
	FVector HitPoint;

	FCustomDestinationParams Params;
	Params.WorldRay = &DragPos.WorldRay;
	if (ShouldUseCustomDestinationFunc() && CustomDestinationFunc(Params, HitPoint))
	{
		GizmoMath::NearestPointOnLine(InteractionOrigin, InteractionAxis, HitPoint,
			InteractionCurPoint, InteractionCurParameter);
		InteractionCurParameter += bCustomDestinationAlignsAxisOrigin ? InteractionStartAxisOriginParameterOffset : 0.0f;
	}
	else
	{
		float RayNearestParam; float AxisNearestParam;
		FVector RayNearestPt;
		GizmoMath::NearestPointOnLineToRay(InteractionOrigin, InteractionAxis,
			DragPos.WorldRay.Origin, DragPos.WorldRay.Direction,
			InteractionCurPoint, AxisNearestParam,
			RayNearestPt, RayNearestParam);

		InteractionCurParameter = ParameterSign * AxisNearestParam;
	}

	float DeltaParam = InteractionCurParameter - InteractionStartParameter;
	float NewParamValue = InitialTargetParameter + DeltaParam;

	ParameterSource->SetParameter(NewParamValue);

	UE_LOG(LogTemp, Log, TEXT("OnClickDrag: Current interaction point is %d"), NewParamValue);
}

//end
void UOpenDriveAxisPositionGizmo::OnClickRelease(const FInputDeviceRay& ReleasePos)
{
	check(bInInteraction);

	ParameterSource->EndModify();
	if (StateTarget)
	{
		StateTarget->EndUpdate();
	}
	bInInteraction = false;

	if (HitTarget)
	{
		HitTarget->UpdateInteractingState(bInInteraction);
	}
	UE_LOG(LogTemp, Log, TEXT("OnClickRelease: Interaction ended at %s"), *InteractionCurPoint.ToString());

}

//not called after drag 
void UOpenDriveAxisPositionGizmo::OnTerminateDragSequence()
{
	check(bInInteraction);

	ParameterSource->EndModify();
	if (StateTarget)
	{
		StateTarget->EndUpdate();
	}
	bInInteraction = false;

	if (HitTarget)
	{
		HitTarget->UpdateInteractingState(bInInteraction);
	}
	UE_LOG(LogTemp, Log, TEXT("OnTerminateDragSequence: Interaction terminated."));
}

void UOpenDriveGizmoAxisTranslationParameterSource::SetParameter(float NewValue)
{
	UGizmoComponentAxisSource* source = Cast<UGizmoComponentAxisSource>(AxisSource->_getUObject());
	if (source->bLocalAxes)
	{
		UOpenDrivePosition* OpenDrivePosition = NewObject<UOpenDrivePosition>();
		OpenDrivePosition->SetTransform(InitialTransform);
		OpenDrivePosition->MoveAlongS(NewValue, 0);

		TransformSource->SetTransform(OpenDrivePosition->GetTransform());
		OnParameterChanged.Broadcast(this, LastChange);
	}
	else
	{
		Parameter = NewValue;
		LastChange.CurrentValue = NewValue;

		double UseDelta = LastChange.GetChangeDelta();

		// check for any constraints on the delta value
		double SnappedDelta = 0;
		if (AxisDeltaConstraintFunction(UseDelta, SnappedDelta))
		{
			UseDelta = SnappedDelta;
		}

		// construct translation as delta from initial position
		FVector Translation = UseDelta * CurTranslationAxis;

		// translate the initial transform
		FTransform NewTransform = InitialTransform;
		NewTransform.AddToTranslation(Translation);

		// apply position constraint
		FVector SnappedPos;
		if (PositionConstraintFunction(NewTransform.GetTranslation(), SnappedPos))
		{
			FVector SnappedLinePos = GizmoMath::ProjectPointOntoLine(SnappedPos, CurTranslationOrigin, CurTranslationAxis);
			NewTransform.SetTranslation(SnappedLinePos);
		}

		TransformSource->SetTransform(NewTransform);

		OnParameterChanged.Broadcast(this, LastChange);
	}
}
