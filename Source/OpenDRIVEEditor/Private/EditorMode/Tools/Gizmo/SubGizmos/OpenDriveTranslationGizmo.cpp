#include "OpenDriveTranslationGizmo.h"
#include "InteractiveGizmoManager.h"
#include "BaseBehaviors/MouseHoverBehavior.h"
#include "BaseGizmos/AxisSources.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "BaseGizmos/GizmoMath.h"
#include "BaseGizmos/HitTargets.h"
#include "BaseGizmos/ParameterSourcesFloat.h"
#include "BaseGizmos/StateTargets.h"
#include "BaseGizmos/TransformSources.h"
#include "BaseGizmos/TransformSubGizmoUtil.h"
#include "DrawDebugHelpers.h"
#include "OpenDrivePosition.h"
#include "EditorMode/Tools/Gizmo/OpenDriveGizmo.h"
#include "EditorMode/Tools/Gizmo/Sources/OpenDriveFloatParameterSource.h"
#include "EditorMode/Tools/Gizmo/Sources/OpenDriveGizmoAxisSource.h"

UInteractiveGizmo* UOpenDriveTranslationGizmoBuilder::BuildGizmo(const FToolBuilderState& SceneState) const
{
	return NewObject<UOpenDriveTranslationGizmo>(SceneState.GizmoManager);
}

bool UOpenDriveTranslationGizmo::Initialize(UPrimitiveComponent* ComponentIn, UTransformProxy* TransformProxyIn,
                                          IToolContextTransactionProvider* TransactionProvider, FGizmoSharedState* SharedStateIn,
                                          const int AxisIndex, const EOpenDriveSourceType SourceType)
{
	const AActor* OwnerActor = ComponentIn->GetOwner();
	
	// Shared Axis Source
	UOpenDriveGizmoAxisSource* CastAxisSource = UOpenDriveGizmoAxisSource::Construct(OwnerActor->GetRootComponent(), AxisIndex, GetTransientPackage());
	AxisSource = CastAxisSource;
	if (SharedStateIn)
	{
		switch (AxisIndex)
		{
		case 0:
			SharedStateIn->AxisSSource = CastAxisSource;
			break;
		case 1:
			SharedStateIn->AxisTSource = CastAxisSource;
			break;
		case 2 :
			SharedStateIn->AxisChangeLaneSource = CastAxisSource;
			break;
		default:
			break;
		}
	}

	// Transform Source
	UGizmoScaledAndUnscaledTransformSources* CastTransformSource;
	if (SharedStateIn && SharedStateIn->TransformSource)
	{
		CastTransformSource = SharedStateIn->TransformSource;
	}
	else
	{
		CastTransformSource = UGizmoScaledAndUnscaledTransformSources::Construct(UGizmoTransformProxyTransformSource::Construct(
			TransformProxyIn, GetTransientPackage()), OwnerActor->GetRootComponent(), GetTransientPackage());

		if (SharedStateIn)
		{
			SharedStateIn->TransformSource = CastTransformSource;
		}
	}
	TransformSource = CastTransformSource;

	// HitTarget
	UGizmoComponentHitTarget* CastHitTarget = UGizmoComponentHitTarget::Construct(ComponentIn, this);
	CastHitTarget->UpdateHoverFunction = [ComponentIn](bool bHovering)
	{
		if (IGizmoBaseComponentInterface* Hoverable = Cast<IGizmoBaseComponentInterface>(ComponentIn))
		{
			Hoverable->UpdateHoverState(bHovering);
		}
	};
	CastHitTarget->UpdateInteractingFunction = [ComponentIn](bool bInteracting)
	{
		if (IGizmoBaseComponentInterface* Hoverable = Cast<IGizmoBaseComponentInterface>(ComponentIn))
		{
			Hoverable->UpdateInteractingState(bInteracting);
		}
	};
	HitTarget = CastHitTarget;

	// Shared StateTarget
	if (SharedStateIn && SharedStateIn->StateTarget)
	{
		StateTarget = SharedStateIn->StateTarget;
	}
	else
	{
		const FText DefaultTransactionName(NSLOCTEXT("UOpenDriveGizmo", "UOpenDriveGizmoTransaction", "Transform"));
		
		UGizmoTransformChangeStateTarget* CastStateTarget =  UGizmoTransformChangeStateTarget::Construct(OwnerActor->GetRootComponent(), DefaultTransactionName, TransactionProvider, GetTransientPackage() );
		CastStateTarget->DependentChangeSources.Add(MakeUnique<FTransformProxyChangeSource>(TransformProxyIn));
		StateTarget = CastStateTarget;

		if (SharedStateIn)
		{
			SharedStateIn->StateTarget = CastStateTarget;
		}
	}

	// Parameter source
	UOpenDriveFloatParameterSource* CastParameterSource = UOpenDriveFloatParameterSource::Construct(AxisSource.GetInterface(), CastTransformSource, GetTransientPackage());
	CastParameterSource->SourceType = SourceType;
	CastParameterSource->bAlignToLane = false;
	ParameterSource = CastParameterSource;
	
	return true;
}

void UOpenDriveTranslationGizmo::Setup()
{
	Super::Setup();

	ClickDragBehavior = NewObject<UClickDragInputBehavior>();
	ClickDragBehavior->Initialize(this);
	ClickDragBehavior->SetDefaultPriority(FInputCapturePriority(FInputCapturePriority::DEFAULT_GIZMO_PRIORITY));
	AddInputBehavior(ClickDragBehavior);
	
	UMouseHoverBehavior* HoverBehavior = NewObject<UMouseHoverBehavior>();
	HoverBehavior->Initialize(this);
	HoverBehavior->SetDefaultPriority(FInputCapturePriority(FInputCapturePriority::DEFAULT_GIZMO_PRIORITY));
	AddInputBehavior(HoverBehavior);

	AxisSource = NewObject<UGizmoComponentAxisSource>(this);
	ParameterSource = NewObject<UGizmoLocalFloatParameterSource>(this);
	HitTarget = NewObject<UGizmoNilStateTarget>(this);
	OpenDrivePosition = NewObject<UOpenDrivePosition>(this);

	bInInteraction = false;
}

FInputRayHit UOpenDriveTranslationGizmo::BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos)
{
	FInputRayHit GizmoHit;
	if (HitTarget)
	{
		GizmoHit = HitTarget->IsHit(PressPos);
	}
	return GizmoHit;
}

void UOpenDriveTranslationGizmo::OnBeginHover(const FInputDeviceRay& DevicePos)
{
	HitTarget->UpdateHoverState(true);
}

bool UOpenDriveTranslationGizmo::OnUpdateHover(const FInputDeviceRay& DevicePos)
{
	return true;
}

void UOpenDriveTranslationGizmo::OnEndHover()
{
	HitTarget->UpdateHoverState(false);
}

FInputRayHit UOpenDriveTranslationGizmo::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
{
	FInputRayHit GizmoHit;

	if (HitTarget && AxisSource && ParameterSource)
	{
		GizmoHit = HitTarget->IsHit(PressPos);

		if (GizmoHit.bHit)
		{
			LastHitLocation = PressPos.WorldRay.PointAt(GizmoHit.HitDepth);
		}
	}
	return GizmoHit;
}

void UOpenDriveTranslationGizmo::OnClickPress(const FInputDeviceRay& PressPos)
{
	InteractionOrigin = LastHitLocation;
	InteractionAxis = AxisSource->GetDirection();

	FVector NearestPt;
	float RayNearestParam;

	GizmoMath::NearestPointOnLineToRay(InteractionOrigin, InteractionAxis, PressPos.WorldRay.Origin,PressPos.WorldRay.Direction, InteractionStartPoint, InteractionStartParameter,NearestPt, RayNearestParam);
	FVector AxisOrigin = AxisSource->GetOrigin();
	double DirectionSign = FVector::DotProduct(InteractionStartPoint - AxisOrigin, InteractionAxis);
	ParameterSign = DirectionSign < 0 ? -1 : 1;
	float AxisOriginParamValue;
	GizmoMath::NearestPointOnLine(InteractionOrigin, InteractionAxis,AxisOrigin, NearestPt,AxisOriginParamValue);
	InteractionStartAxisOriginParameterOffset = InteractionStartParameter - AxisOriginParamValue;
	
	InteractionCurPoint = InteractionStartPoint;
	InteractionStartParameter *= ParameterSign;
	InteractionCurParameter = InteractionStartParameter;

	InitialTargetParameter = ParameterSource->GetParameter();
	ParameterSource->BeginModify();

	bInInteraction = true;

	if (StateTarget)
	{
		StateTarget->BeginUpdate();
	}
	if (ensure(HitTarget))
	{
		HitTarget->UpdateInteractingState(bInInteraction);
	}
}

void UOpenDriveTranslationGizmo::OnClickDrag(const FInputDeviceRay& DragPos)
{
	float RayNearestParam;
	float AxisNearestParam;
	FVector RayNearestPoint;

	GizmoMath::NearestPointOnLineToRay(InteractionOrigin, InteractionAxis, DragPos.WorldRay.Origin, DragPos.WorldRay.Direction,InteractionCurPoint, AxisNearestParam, RayNearestPoint, RayNearestParam);
	InteractionCurParameter = ParameterSign * AxisNearestParam;

	float DeltaParam = InteractionCurParameter - InteractionStartParameter;
	float NewParamValue = InitialTargetParameter + DeltaParam;
	
	ParameterSource->SetParameter(NewParamValue);
}

void UOpenDriveTranslationGizmo::OnClickRelease(const FInputDeviceRay& ReleasePos)
{
	check(bInInteraction);

	ParameterSource->EndModify();
	if (StateTarget)
	{
		StateTarget->EndUpdate();
	}
	bInInteraction = false;

	if (ensure(HitTarget))
	{
		HitTarget->UpdateInteractingState(bInInteraction);
	}
}

void UOpenDriveTranslationGizmo::OnTerminateDragSequence()
{
	check(bInInteraction);

	ParameterSource->EndModify();
	if (StateTarget)
	{
		StateTarget->EndUpdate();
	}
	bInInteraction = false;
	if (ensure(HitTarget))
	{
		HitTarget->UpdateInteractingState(bInInteraction);
	}
}

void UOpenDriveTranslationGizmo::AutoAlignToLane(const bool bShouldAlign) const
{
	if (UOpenDriveFloatParameterSource* CastParameterSource = Cast<UOpenDriveFloatParameterSource>(ParameterSource.GetObject()))
	{
		CastParameterSource->AutoAlignToLane(bShouldAlign);
	}
}

void UOpenDriveTranslationGizmo::OverrideHeight(bool bOverrideHeight)
{
	if (UOpenDriveFloatParameterSource* CastParameterSource = Cast<UOpenDriveFloatParameterSource>(ParameterSource.GetObject()))
	{
		CastParameterSource->OverrideHeight(bOverrideHeight);
	}
}
