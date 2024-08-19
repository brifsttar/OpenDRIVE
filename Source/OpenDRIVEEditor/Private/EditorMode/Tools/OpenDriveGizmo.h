#pragma once 
#include "CoreMinimal.h"
#include "BaseGizmos/CombinedTransformGizmo.h"
#include "BaseGizmos/GizmoViewContext.h"
#include "ContextObjectStore.h"
#include "OpenDriveAxisPositionGizmo.h"
#include "OpenDriveGizmo.generated.h"

UCLASS()
class UOpenDriveGizmoBuilder : public UCombinedTransformGizmoBuilder
{
	GENERATED_BODY()

public:

	virtual UInteractiveGizmo* BuildGizmo(const FToolBuilderState& SceneState) const override;

	static void RegisterGizmoBuilder(UInteractiveGizmoManager* Manager, UGizmoViewContext* GizmoViewContext, FString GizmoAxisBuilderIdentifier, FString GizmoBuilderIdentifier);
};

UCLASS()
class UOpenDriveGizmo : public UCombinedTransformGizmo
{
	GENERATED_BODY()

public : 

	static UOpenDriveGizmo* CreateGizmo(UInteractiveGizmoManager* Manager, const FString& GizmoBuilderIdentifier, const FString& GizmoIdentifier);

protected:

	virtual UInteractiveGizmo* AddAxisTranslationGizmo(UPrimitiveComponent* AxisComponent, USceneComponent* RootComponent,
		IGizmoAxisSource* AxisSource,
		IGizmoTransformSource* TransformSource,
		IGizmoStateTarget* StateTargetIn,
		int AxisIndex) override;

};
