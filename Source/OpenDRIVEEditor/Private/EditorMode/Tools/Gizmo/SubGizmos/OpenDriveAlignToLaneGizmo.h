﻿#pragma once
#include "CoreMinimal.h"
#include "BaseBehaviors/SingleClickBehavior.h"
#include "BaseGizmos/AxisAngleGizmo.h"
#include "OpenDriveAlignToLaneGizmo.generated.h"

class UOpenDrivePosition;

UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveAlignToLaneGizmoBuilder : public UInteractiveGizmoBuilder
{
	GENERATED_BODY()
	public:
	virtual UInteractiveGizmo* BuildGizmo(const FToolBuilderState& SceneState) const override;
};

/**
 * 
 */
UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveAlignToLaneGizmo : public UInteractiveGizmo, public IClickBehaviorTarget, public IHoverBehaviorTarget
{
	GENERATED_BODY()

public :

	bool Initialize(UPrimitiveComponent* ComponentIn);
	
	// UInteractiveGizmo implementation
	virtual void Setup() override;

	// IHoverBehaviorTarget implementation
	virtual FInputRayHit BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos) override;
	virtual void OnBeginHover(const FInputDeviceRay& DevicePos) override;
	virtual bool OnUpdateHover(const FInputDeviceRay& DevicePos) override;
	virtual void OnEndHover() override;

	// IClickBehaviourTarget implementation
	virtual FInputRayHit IsHitByClick(const FInputDeviceRay& ClickPos) override;
	virtual void OnClicked(const FInputDeviceRay& ClickPos) override;

public:
	
	UPROPERTY()
	TScriptInterface<IGizmoClickTarget> HitTarget; //Hit test with UPrimitiveComponent
	
	UPROPERTY()
	TObjectPtr<USingleClickInputBehavior> ClickBehavior; //Button click input behavior

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnClick, UOpenDriveAlignToLaneGizmo&, const FInputDeviceRay&);
	FOnClick OnClick;
};
