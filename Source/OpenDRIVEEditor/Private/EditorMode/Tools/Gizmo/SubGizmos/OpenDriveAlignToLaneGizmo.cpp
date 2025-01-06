#include "OpenDriveAlignToLaneGizmo.h"
#include "EditorModeManager.h"
#include "BaseBehaviors/MouseHoverBehavior.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "BaseGizmos/TransformSubGizmoUtil.h"
#include "EditorMode/OpenDriveEditorMode.h"
#include "EditorMode/Tools/Gizmo/OpenDriveGizmo.h"

UInteractiveGizmo* UOpenDriveAlignToLaneGizmoBuilder::BuildGizmo(const FToolBuilderState& SceneState) const
{
	return NewObject<UOpenDriveAlignToLaneGizmo>(SceneState.GizmoManager);
}

bool UOpenDriveAlignToLaneGizmo::Initialize(UPrimitiveComponent* ComponentIn)
{
	if (!ComponentIn)
	{
		return false;
	}
	
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
	
	return true;
}

void UOpenDriveAlignToLaneGizmo::Setup()
{
	Super::Setup();

	ClickBehavior = NewObject<USingleClickInputBehavior>(this);
	ClickBehavior->Initialize(this);
	ClickBehavior->SetDefaultPriority(FInputCapturePriority(FInputCapturePriority::DEFAULT_GIZMO_PRIORITY));
	AddInputBehavior(ClickBehavior);

	UMouseHoverBehavior* MouseHoverBehavior = NewObject<UMouseHoverBehavior>(this); //Mouse hover behavior
	MouseHoverBehavior->Initialize(this);
	MouseHoverBehavior->SetDefaultPriority(FInputCapturePriority(FInputCapturePriority::DEFAULT_GIZMO_PRIORITY));
	AddInputBehavior(MouseHoverBehavior);

	HitTarget = NewObject<UGizmoNilStateTarget>(this);
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
	if (UOpenDriveEditorMode* CastEditorMode = Cast<UOpenDriveEditorMode>(GLevelEditorModeTools().GetActiveScriptableMode((UOpenDriveEditorMode::EM_OpenDriveEditorModeId))))
	{
		CastEditorMode->AlignActorWithLane();
	}
	OnClick.Broadcast(*this, ClickPos);
}
