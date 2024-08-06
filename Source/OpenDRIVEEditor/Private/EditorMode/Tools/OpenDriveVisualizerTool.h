#pragma once 

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "OpenDriveVisualizerTool.generated.h"

UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveVisualizerToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:

	/* InteractiveToolBuilder interface */
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};

/* Buttons' events */
DECLARE_DELEGATE_TwoParams(FOnGenerateVisualization, float /* Offset */, float/* Step */)
DECLARE_DELEGATE(FOnChangeLaneVisibility)

UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveVisualizerToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:

	UOpenDriveVisualizerToolProperties() {}

	/* OpenDriveVisualizer properties */
	UPROPERTY(EditAnywhere, Category="Parameters", meta = (Tooltip = "Road's height above ground (useful if you have road meshes)", UIMin="0.0", UIMax="50.0"))
	float RoadOffset = 20.0f;
	UPROPERTY(EditAnywhere, Category="Parameters", meta = (Tooltip = "road's drawn precision (don't go to low with that value...)", UIMin = "1.0", UIMax = "20.0"))
	float Step = 5.0f;

	/* OpenDrive lane informations */
	UPROPERTY(VisibleAnywhere, Category = "Info")
	int RoadId = 0;
	UPROPERTY(VisibleAnywhere, Category = "Info")
	int JunctionId = 0;
	UPROPERTY(VisibleAnywhere, Category = "Info")
	int LaneId = 0;
	UPROPERTY(VisibleAnywhere, Category = "Info")
	FString LaneType = "None";
	UPROPERTY(VisibleAnywhere, Category = "Info")
	int SuccessorId = 0;
	UPROPERTY(VisibleAnywhere, Category = "Info")
	int PredecessorId = 0;

	/* Methods */
	UFUNCTION(CallInEditor, Category="Functions")
	void Generate(){ OnGenerateVisualization.Execute(RoadOffset, Step); }
	UFUNCTION(CallInEditor, Category = "Functions")
	void ChangeLaneVisibility(){ OnChangeLaneVisibility.Execute(); }

	/* Delegates */
	FOnGenerateVisualization OnGenerateVisualization;
	FOnChangeLaneVisibility OnChangeLaneVisibility;
};

UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveVisualizerTool : public UInteractiveTool
{
	GENERATED_BODY()

public:

	UOpenDriveVisualizerTool() {}

	/* InteractiveTool interface */
	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;

	virtual void SetWorld(UWorld* World);

protected:

	/* Properties and world */
	TObjectPtr<UOpenDriveVisualizerToolProperties> Properties;
	UWorld* TargetWorld;

	/* OpenDrive's visualizer base methods */
	void Generate(float offset, float step);
	void HideShowLanes();
	void DeleteAllLanes();

	/* Actor selection */
	void OnActorSelected(UObject* selectedActor);
	FDelegateHandle OnActorSelectedHandle;
};
