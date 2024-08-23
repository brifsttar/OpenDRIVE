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

	/* InteractiveToolBuilder interface */
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};

UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveUtilsToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public :

	UOpenDriveUtilsToolProperties() {}

	/* Methods */
	UPROPERTY(EditAnywhere, Category="Selected Actor")
	AActor* SelectedActor;

	UPROPERTY(EditAnywhere, Category = "Selected Actor")
	float S = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Selected Actor")
	float T = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Selected Actor | Lane")
	int32 LaneId = 0;

	UPROPERTY(EditAnywhere, Category = "Selected Actor | Repeat Along Road", meta=(Units = "cm"))
	float Step = 200.0f;

	/* Functions */
	UFUNCTION(CallInEditor, Category = "Selected Actor | Lane")
	void AlignWithLane() { if (IsValid(SelectedActor)) OnAlignActorWithLane.Execute(); }

	UFUNCTION(CallInEditor, Category = "Selected Actor | Repeat Along Road")
	void RepeatAlongRoad() { if (IsValid(SelectedActor)) OnRepeatAlongRoad.Execute(Step); }

	void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	void UpdateActorInfo(USceneComponent* SceneComponent, EUpdateTransformFlags UpdateTransformFlag, ETeleportType Teleport);
	void UpdateLaneInfo(USceneComponent* SceneComponent);

	/* Delegates */
	DECLARE_DELEGATE(FOnAlignActorWithLane)
	FOnAlignActorWithLane OnAlignActorWithLane;
	DECLARE_DELEGATE(FOnUpdateActorTransform)
	FOnUpdateActorTransform OnUpdateActorTransform;
	DECLARE_DELEGATE_OneParam(FOnLaneChange, int32 /*Direction*/)
	FOnLaneChange OnLaneChange;
	DECLARE_DELEGATE_OneParam(FOnRepeatAlongRoad, float /*Step*/)
	FOnRepeatAlongRoad OnRepeatAlongRoad;

	FDelegateHandle ActorTransformInfoHandle;
};

UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveUtilsTool : public UInteractiveTool
{
	GENERATED_BODY()

public:

	UOpenDriveUtilsTool() {}

	/* Interactive tool interface */
	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;

	virtual void SetWorld(UWorld* World);

protected:

	/* Properties and world */
	TObjectPtr<UOpenDriveUtilsToolProperties> Properties;
	UWorld* TargetWorld;

	/* Actor selection */
	void OnActorSelected(UObject* selectedObject);
	FDelegateHandle OnActorSelectedHandle;

	/* OpenDRIVE functions */
	void AlignActorWithLane();
	void UpdateActorTransform();
	void ChangeActorLane(int32 Direction);
	void RepeatAlongRoad(float step);

	UOpenDriveEditorMode* GetEditorMode() const;

private : 

	void ResetGizmoTransform();
};
