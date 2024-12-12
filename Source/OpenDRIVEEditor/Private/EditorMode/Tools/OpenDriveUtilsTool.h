#pragma once 
#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "BaseGizmos/TransformGizmoUtil.h"
#include "OpenDriveUtilsTool.generated.h"

class UOpenDrivePosition;
class UOpenDriveEditorMode;

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

	UOpenDriveUtilsToolProperties(): SelectedActor(nullptr), S(0), T(0), LaneId(0)
	{
	}

	/* Methods */
	UPROPERTY(EditAnywhere, Category="Selected Actor")
	AActor* SelectedActor;

	UPROPERTY(EditAnywhere, Category="Selected Actor",  meta=(ForceUnits="m"))
	float S;

	UPROPERTY(EditAnywhere, Category = "Selected Actor", meta=(ForceUnits="m"))
	float T;

	UPROPERTY(VisibleAnywhere, Category = "Selected Actor")
	int32 RoadId;

	UPROPERTY(EditAnywhere, Category = "Selected Actor")
	int32 LaneId;

	UPROPERTY(EditAnywhere, Category = "Repeat Along Road")
	float Step = 200.0f;

	/* Functions */
	UFUNCTION(CallInEditor, Category = "Align with lane")
	FORCEINLINE void AlignWithLane() const { if (IsValid(SelectedActor)) OnAlignActorWithLane.Execute(); }

	UFUNCTION(CallInEditor, Category = "Repeat Along Road")
	FORCEINLINE void RepeatAlongRoad() const { if (IsValid(SelectedActor)) OnRepeatAlongRoad.Execute(Step); }

	/* Update Actor info */
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	void UpdateActorInfo(USceneComponent* SceneComponent, EUpdateTransformFlags UpdateTransformFlag, ETeleportType Teleport);
	void UpdateLaneInfo(const USceneComponent* SceneComponent);

	/* Delegates */
	DECLARE_DELEGATE(FOnAlignActorWithLane)
	FOnAlignActorWithLane OnAlignActorWithLane;
	DECLARE_DELEGATE(FOnUpdateActorTransform)
	FOnUpdateActorTransform OnUpdateActorTransform;
	DECLARE_DELEGATE_OneParam(FOnLaneChange, const int32 /*Direction*/)
	FOnLaneChange OnLaneChange;
	DECLARE_DELEGATE_OneParam(FOnRepeatAlongRoad, const float /*Step*/)
	FOnRepeatAlongRoad OnRepeatAlongRoad;

	FDelegateHandle ActorTransformInfoHandle;
};

UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveUtilsTool : public UInteractiveTool
{
	GENERATED_BODY()

public:

	UOpenDriveUtilsTool(): TargetWorld(nullptr)
	{
	}

	/* Interactive tool interface */
	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;

	virtual void SetWorld(UWorld* World);

protected:

	/* Properties and world */
	UPROPERTY()
	TObjectPtr<UOpenDriveUtilsToolProperties> Properties;

	UPROPERTY()
	UWorld* TargetWorld;

	/* Actor selection */
	void OnActorSelected(UObject* SelectedObject);
	FDelegateHandle OnActorSelectedHandle;

	/* OpenDRIVE functions */
	void AlignActorWithLane() const;
	void UpdateActorTransform() const;
	void ChangeActorLane(int32 Direction) const;
	void RepeatAlongRoad(float Step) const;

	UOpenDriveEditorMode* GetEditorMode() const;

	UPROPERTY()
	TObjectPtr<UOpenDrivePosition> OpenDrivePosition;
};
