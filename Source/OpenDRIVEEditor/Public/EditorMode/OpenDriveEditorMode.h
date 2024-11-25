#pragma once 
#include "CoreMinimal.h"
#include "Tools/UEdMode.h"
#include "OpenDriveEditorMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOpenDriveEditorMode, Log, All);

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
	virtual ~UOpenDriveEditorMode() override {}

	// UEdMode interface implementation start
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void CreateToolkit() override;
	virtual TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetModeCommands() const override;
	virtual void ActorSelectionChangeNotify() override;
	// UEdMode interface implementation end
	
	FORCEINLINE UInteractiveToolManager* GetToolkitManager() const { return GetToolManager(); }
};
