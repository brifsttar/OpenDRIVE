#pragma once 
#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "BaseGizmos/TransformGizmoUtil.h"
#include "OpenDriveUtilsTool.generated.h"

class UOpenDrivePosition;
class UOpenDriveEditorMode;

UENUM()
enum class EOpenDriveTranslateType : uint8 {
	TranslateOnS = 0,
	TranslateOnT = 1
};

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

	UOpenDriveUtilsToolProperties(): SelectedActor(nullptr), S(0), T(0), RoadId(0), LaneId(0)
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

	UPROPERTY(EditAnywhere, Category = "Repeat Along Road", meta=(ForceUnits="cm"))
	float Step = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Repeat Along Road")
	bool bAlignDuplicatesWithLaneDirection = false;

	UPROPERTY()
	TObjectPtr<UOpenDrivePosition> OpenDrivePosition = nullptr;

	UPROPERTY()
	FTransform LastKnownTransform;
	
	/* Functions */
	UFUNCTION(CallInEditor, Category = "Align with lane")
	FORCEINLINE void AlignWithLane() const { if (IsValid(SelectedActor)) OnAlignActorWithLane.Execute(); }

	UFUNCTION(CallInEditor, Category = "Repeat Along Road")
	FORCEINLINE void RepeatAlongRoad() const { if (IsValid(SelectedActor)) OnRepeatAlongRoad.Execute(Step, bAlignDuplicatesWithLaneDirection); }

	/* Update Actor info */
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	void UpdateLaneInfo();

	/* Delegates */
	DECLARE_DELEGATE(FOnAlignActorWithLane)
	FOnAlignActorWithLane OnAlignActorWithLane;
	DECLARE_DELEGATE_OneParam(FOnUpdateActorTransform, const EOpenDriveTranslateType /*TranslateType*/)
	FOnUpdateActorTransform OnUpdateActorTransform;
	DECLARE_DELEGATE_OneParam(FOnLaneChange, const int32 /*Direction*/)
	FOnLaneChange OnLaneChange;
	DECLARE_DELEGATE_TwoParams(FOnRepeatAlongRoad, const float /*Step*/, const bool /*AlignWithLaneDirection*/)
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

	void OnAlignActorWithLane();
	void OnLaneChange(int NewLaneId);
	void OnRepeatAlongRoad(float Step, bool bAlignWithLaneDirection);
	void ActorSelectionChanged(AActor* Actor);
	
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
	
	/* OpenDRIVE functions */
	void UpdateActorT() const;
	void UpdateActorS() const;
	void UpdateActorTransform(EOpenDriveTranslateType TranslateType) const;

	UOpenDriveEditorMode* GetEditorMode() const;
	FDelegateHandle SelectionChangedHandle;
};
