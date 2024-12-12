#include "OpenDriveAlignToLaneGizmo.h"
#include "OpenDrivePosition.h"
#include "BaseBehaviors/MouseHoverBehavior.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "BaseGizmos/TransformSubGizmoUtil.h"
#include "EditorMode/Tools/Gizmo/OpenDriveGizmo.h"

UInteractiveGizmo* UOpenDriveAlignToLaneGizmoBuilder::BuildGizmo(const FToolBuilderState& SceneState) const
{
	return NewObject<UOpenDriveAlignToLaneGizmo>(SceneState.GizmoManager);
}

bool UOpenDriveAlignToLaneGizmo::Initialize(UPrimitiveComponent* ComponentIn,UTransformProxy* TransformProxyIn ,IToolContextTransactionProvider* TransactionProvider,  FGizmoSharedState* SharedStateIn)
{
	if (!ComponentIn)
	{
		return false;
	}

	AActor* OwnerActor = ComponentIn->GetOwner();
	
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
	
	return true;
}

void UOpenDriveAlignToLaneGizmo::Setup()
{
	Super::Setup();

	ClickBehavior = NewObject<USingleClickInputBehavior>(this);
	ClickBehavior->Initialize(this);
	ClickBehavior->SetDefaultPriority(FInputCapturePriority(FInputCapturePriority::DEFAULT_GIZMO_PRIORITY));
	AddInputBehavior(ClickBehavior);

	UMouseHoverBehavior* MouseHoverBehavior = NewObject<UMouseHoverBehavior>(this);
	MouseHoverBehavior->Initialize(this);
	MouseHoverBehavior->SetDefaultPriority(FInputCapturePriority(FInputCapturePriority::DEFAULT_GIZMO_PRIORITY));
	AddInputBehavior(MouseHoverBehavior);

	HitTarget = NewObject<UGizmoNilStateTarget>(this);
	OpenDrivePosition = NewObject<UOpenDrivePosition>(this);
}

FInputRayHit UOpenDriveAlignToLaneGizmo::BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos)
{
	FInputRayHit GizmoHit;
	if (HitTarget)
	{
		GizmoHit = HitTarget->IsHit(PressPos);
	}
	return GizmoHit;
}

void UOpenDriveAlignToLaneGizmo::OnBeginHover(const FInputDeviceRay& DevicePos)
{
	HitTarget->UpdateHoverState(true);
}

bool UOpenDriveAlignToLaneGizmo::OnUpdateHover(const FInputDeviceRay& DevicePos)
{
	return true;
}

void UOpenDriveAlignToLaneGizmo::OnEndHover()
{
	HitTarget->UpdateHoverState(false);
}

FInputRayHit UOpenDriveAlignToLaneGizmo::IsHitByClick(const FInputDeviceRay& ClickPos)
{
	FInputRayHit GizmoHit;
	if (HitTarget)
	{
		GizmoHit = HitTarget->IsHit(ClickPos);
	}
	return GizmoHit;
}

void UOpenDriveAlignToLaneGizmo::OnClicked(const FInputDeviceRay& ClickPos)
{
	StateTarget->BeginUpdate();
	OpenDrivePosition->SetTransform(TransformSource->GetTransform());
	OpenDrivePosition->AlignWithLaneCenter();
	FTransform OdrTransform = OpenDrivePosition->GetTransform();
	FVector OdrLocation = OdrTransform.GetLocation();
	FTransform TargetT  = TransformSource->GetTransform();
	FVector TargetLocation = TargetT.GetLocation();
	FVector LocationWithInitialHeight = FVector(OdrLocation.X, OdrLocation.Y, TargetLocation.Z);
	FTransform FinalTransform = FTransform(LocationWithInitialHeight);
	FinalTransform.SetRotation(OdrTransform.GetRotation());
	FinalTransform.SetScale3D(TargetT.GetScale3D());
	TransformSource->SetTransform(FinalTransform);
	OnClick.Broadcast(*this, ClickPos);
	StateTarget->EndUpdate();
}
