#pragma once 
#include "CoreMinimal.h"
#include "Tools/UEdMode.h"
#include "OpenDriveEditorMode.generated.h"

class UOpenDriveGizmo;

DECLARE_LOG_CATEGORY_EXTERN(LogOpenDriveEditorMode, Log, All);

UCLASS()
class UOpenDriveEditorMode : public UEdMode
{
	GENERATED_BODY()

public :

	const static FEditorModeID EM_OpenDriveEditorModeId;

	/* Tools names */
	const static FString OpenDriveVisualizerToolName;
	const static FString OpenDriveUtilsToolName;

	UOpenDriveEditorMode();

	virtual ~UOpenDriveEditorMode() override {}

	// UEdMode interface implementation start
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void CreateToolkit() override;
	virtual TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetModeCommands() const override;
	virtual void ActorSelectionChangeNotify() override;
	// UEdMode interface implementation end
	
	UInteractiveToolManager* GetToolkitManager() const { return GetToolManager(); }

	// Gizmos' builders identifiers
	const static FString OpenDriveChangeLaneBuilderIdentifier;
	const static FString OpenDriveMoveAlongLaneBuilderIdentifier;
	const static FString OpenDriveGizmoBuilderIdentifier;

	// Gizmo
	const static FString OpenDriveGizmoIdentifier;
	UPROPERTY()
	bool bAutoAlignWithLane;
	UPROPERTY()
	bool bOverrideHeight;
	
	// Utilities methods
	void AlignActorWithLane();
	void ChangeActorLane(int NewLaneId);
	void RepeatAlongRoad(float Step, bool bAlignWithLaneDirection);
	void ToggleAutoAlignWithLane();
	void ToggleOverrideHeight();
	AActor* GetSelectedActor() { return SelectedActor; }
	void ResetGizmo() const;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOdrActorSelectionChanged, AActor*)
	FOdrActorSelectionChanged OnActorSelectionChanged;
	
protected :

	UPROPERTY()
	TObjectPtr<AActor> SelectedActor;

	UPROPERTY()
	TObjectPtr<UOpenDriveGizmo> OpenDriveGizmo;

private :

	void InitializeOpenDriveGizmo();
	void DeInitializeOpenDriveGizmo();
};

