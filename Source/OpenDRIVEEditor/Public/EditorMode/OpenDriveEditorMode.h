#pragma once 
#include "CoreMinimal.h"
#include "Tools/UEdMode.h"
#include "EditorMode/Tools/Gizmo/OpenDriveGizmo.h"
#include "OpenDriveEditorMode.generated.h"

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
	
	FORCEINLINE UInteractiveToolManager* GetToolkitManager() const { return GetToolManager(); }
	
	// Gizmos' builders identifiers
	const static FString OpenDriveChangeLaneBuilderIdentifier;
	const static FString OpenDriveMoveAlongLaneBuilderIdentifier;
	const static FString OpenDriveGizmoBuilderIdentifier;

	// Gizmo name
	const static FString OpenDriveGizmoIdentifier;
	
protected :
	
	UPROPERTY()
	TObjectPtr<UOpenDriveGizmo> OpenDriveGizmo;
	
	UPROPERTY()
	TObjectPtr<UOpenDrivePosition> OpenDrivePosition;

private :

	void InitializeOpenDriveGizmo();
	void DeInitializeOpenDriveGizmo() const;
};
