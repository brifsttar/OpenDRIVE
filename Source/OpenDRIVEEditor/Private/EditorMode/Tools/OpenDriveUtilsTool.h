#pragma once 

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "BaseGizmos/CombinedTransformGizmo.h"
#include "BaseGizmos/TransformGizmoUtil.h"
#include "OpenDriveUtilsTool.generated.h"

UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveUtilsToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};

DECLARE_DELEGATE(FOnAlignActorWithLane)

UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveUtilsToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public :

	UOpenDriveUtilsToolProperties();

	UPROPERTY(VisibleAnywhere)
	AActor* SelectedActor;

	UFUNCTION(CallInEditor)
	void AlignWithLane();

	FOnAlignActorWithLane OnAlignActorWithLane;
};

UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveUtilsTool : public UInteractiveTool
{
	GENERATED_BODY()

public:
	UOpenDriveUtilsTool();

	virtual void SetWorld(UWorld* World);

	virtual void Setup() override;

	virtual void Shutdown(EToolShutdownType ShutdownType) override;

	void OnGizmoTransformStateChanged(FTransform CurrentTransform);

protected:

	UPROPERTY()
	TObjectPtr<UOpenDriveUtilsToolProperties> Properties;

	UWorld* TargetWorld;

	TObjectPtr<UCombinedTransformGizmo> Gizmo;
	FString GizmoIdentifier = "OpenDriveGizmo";

	void CreateGizmo(FTransform InitialTransform, USceneComponent* AttachedComponent);

	void DestroyGizmo();
	
	void OnActorSelected(UObject* selectedObject);
	FDelegateHandle OnActorSelectedHandle;
};
