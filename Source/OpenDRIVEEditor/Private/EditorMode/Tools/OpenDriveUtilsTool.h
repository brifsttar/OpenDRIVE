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

/* Buttons' events */
DECLARE_DELEGATE(FOnAlignActorWithLane)

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

	/* Functions */
	UFUNCTION(CallInEditor, Category="Methods")
	void AlignWithLane() { OnAlignActorWithLane.Execute(); }

	void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	void UpdateActorInfo(USceneComponent* SceneComponent, EUpdateTransformFlags UpdateTransformFlag, ETeleportType Teleport);
	void UpdateActorTransform();

	/* Delegates */
	FOnAlignActorWithLane OnAlignActorWithLane;

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
};
