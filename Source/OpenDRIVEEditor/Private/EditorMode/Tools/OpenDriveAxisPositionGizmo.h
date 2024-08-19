#pragma once 
#include "CoreMinimal.h"
#include "BaseGizmos/AxisPositionGizmo.h"
#include "OpenDriveAxisPositionGizmo.generated.h"

UCLASS()
class UOpenDriveAxisGizmoBuilder : public UInteractiveGizmoBuilder
{
	GENERATED_BODY()

public:
	virtual UInteractiveGizmo* BuildGizmo(const FToolBuilderState& SceneState) const override;
};

UCLASS()
class UOpenDriveAxisPositionGizmo : public UAxisPositionGizmo
{
	GENERATED_BODY()

public : 

	// IClickDragBehaviorTarget interface implementation
	virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
	virtual void OnClickPress(const FInputDeviceRay& PressPos) override;
	virtual void OnClickDrag(const FInputDeviceRay& DragPos) override;
	virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override;
	virtual void OnTerminateDragSequence() override;
};

UCLASS()
class UOpenDriveGizmoAxisTranslationParameterSource : public UGizmoAxisTranslationParameterSource
{
	GENERATED_BODY()

public :

	virtual void SetParameter(float NewValue) override;

	static UOpenDriveGizmoAxisTranslationParameterSource* Construct(
		IGizmoAxisSource* AxisSourceIn,
		IGizmoTransformSource* TransformSourceIn,
		UObject* Outer = (UObject*)GetTransientPackage())
	{
		UOpenDriveGizmoAxisTranslationParameterSource* NewSource = NewObject<UOpenDriveGizmoAxisTranslationParameterSource>(Outer);
		NewSource->AxisSource = Cast<UObject>(AxisSourceIn);
		NewSource->TransformSource = Cast<UObject>(TransformSourceIn);
		return NewSource;
	}
};
