#pragma once 

#include "CoreMinimal.h"
#include "Tools/UEdMode.h"
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
	
	inline UInteractiveToolManager* GetToolkitManager() { return GetToolManager(); };
};
