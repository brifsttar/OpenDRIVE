#pragma once
#include "CoreMinimal.h"
#include "InteractiveGizmo.h"
#include "InteractiveGizmoBuilder.h"
#include "InteractiveToolChange.h"
#include "OpenDrivePosition.h"
#include "BaseBehaviors/BehaviorTargetInterfaces.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "BaseGizmos/GizmoInterfaces.h"
#include "BaseGizmos/ParameterSourcesFloat.h"
#include "BaseGizmos/TransformSubGizmoUtil.h"
#include "EditorMode/Tools/Gizmo/Sources/OpenDriveGizmoAxisSource.h"
#include "OpenDriveTranslationGizmo.generated.h"

class UTransformProxy;

UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveTranslationGizmoBuilder : public UInteractiveGizmoBuilder
{
	GENERATED_BODY()
public:
	virtual UInteractiveGizmo* BuildGizmo(const FToolBuilderState& SceneState) const override;
};

/**
 * 
 */
UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveTranslationGizmo : public UInteractiveGizmo, public IClickDragBehaviorTarget, public IHoverBehaviorTarget
{
	GENERATED_BODY()

	public :

	bool Initialize(UPrimitiveComponent* ComponentIn, UTransformProxy* TransformProxyIn, IToolContextTransactionProvider*
	                TransactionProvider, FGizmoSharedState* SharedStateIn, int AxisIndex, EOpenDriveSourceType
	                SourceType);

	// UInteractiveGizmo implementation
	virtual void Setup() override;

	// IHoverBehaviorTarget implementation
	virtual FInputRayHit BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos) override;
	virtual void OnBeginHover(const FInputDeviceRay& DevicePos) override;
	virtual bool OnUpdateHover(const FInputDeviceRay& DevicePos) override;
	virtual void OnEndHover() override;

	// IClickDragBehaviorTarget
	virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
	virtual void OnClickPress(const FInputDeviceRay& PressPos) override;
	virtual void OnClickDrag(const FInputDeviceRay& DragPos) override;
	virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override;
	virtual void OnTerminateDragSequence() override;
	
	void AutoAlignToLane(const bool bShouldAlign) const;

public :

	UPROPERTY()
	TScriptInterface<IGizmoAxisSource> AxisSource;

	UPROPERTY()
	TScriptInterface<IGizmoFloatParameterSource> ParameterSource;

	UPROPERTY()
	TScriptInterface<IGizmoTransformSource> TransformSource;
	
	UPROPERTY()
	TScriptInterface<IGizmoClickTarget> HitTarget;

	UPROPERTY()
	TScriptInterface<IGizmoStateTarget> StateTarget;
	
	UPROPERTY()
	TObjectPtr<UClickDragInputBehavior> ClickDragBehavior;

	UPROPERTY()
	TObjectPtr<UOpenDrivePosition> OpenDrivePosition;

	UPROPERTY()
	bool bInInteraction;

	UPROPERTY()
	FVector LastHitLocation;

	UPROPERTY()
	FVector InteractionOrigin;

	UPROPERTY()
	FVector InteractionAxis;

	UPROPERTY()
	FVector InteractionStartPoint;

	UPROPERTY()
	FVector InteractionCurPoint;

	UPROPERTY()
	float InteractionStartParameter;

	UPROPERTY()
	int ParameterSign;

	UPROPERTY()
	float InteractionStartAxisOriginParameterOffset;

	UPROPERTY()
	float InteractionCurParameter;

	UPROPERTY()
	float InitialTargetParameter;
};
