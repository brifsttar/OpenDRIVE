#pragma once 

#include "CoreMinimal.h"
#include "Tools/UEdMode.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "EditorModeManager.h"
#include "OpenDrivePosition.h"
#include "OpenDriveEditorMode.generated.h"

UCLASS()
class UOpenDriveEditorMode : public UEdMode
{
	GENERATED_BODY()

public :

	const static FEditorModeID EM_OpenDriveEditorModeId;

	/* Tools names */
	static FString OpenDriveVisualizerToolName;
	static FString OpenDriveUtilsToolName;

	UOpenDriveEditorMode();
	virtual ~UOpenDriveEditorMode();

	/** UEdMode interface */
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void CreateToolkit() override;
	virtual TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetModeCommands() const override;
	virtual void ActorSelectionChangeNotify() override;	

	void CreateGizmo(FTransform InitialTransform, USceneComponent* AttachedComponent);
	
	void DestroyGizmo();

	void OnActorSelected(UObject* selectedObject);
	
	inline UInteractiveToolManager* GetToolkitManager() { return GetToolManager(); }

	static FString GizmoIdentifier;
	static FString GizmoBuilderIdentifier;
	static FString GizmoAxisBuilderIdentifier;

private :

	void RegisterGizmoBuilder();
	void UnregisterGizmoBuilder();
};
