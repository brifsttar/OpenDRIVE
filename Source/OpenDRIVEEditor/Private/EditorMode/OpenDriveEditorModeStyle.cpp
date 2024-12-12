#include "EditorMode/OpenDriveEditorModeStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define IMAGE_BRUSH(RelativePath, ...)FSlateImageBrush(OpenDriveStyleInstance->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

TSharedPtr<FSlateStyleSet> FOpenDriveEditorModeStyleSet::OpenDriveStyleInstance = nullptr;

void FOpenDriveEditorModeStyleSet::OnStartupModule()
{
	if (!OpenDriveStyleInstance.IsValid())
	{
		Create();
		FSlateStyleRegistry::RegisterSlateStyle(*OpenDriveStyleInstance);
	}
}

void FOpenDriveEditorModeStyleSet::OnShutdownModule()
{
	if (OpenDriveStyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*OpenDriveStyleInstance);
		ensure(OpenDriveStyleInstance.IsUnique());
		OpenDriveStyleInstance.Reset();
	}
}

FName FOpenDriveEditorModeStyleSet::GetStyleSetName()
{
	static FName StyleSetName(TEXT("OpenDriveStyle"));
	return StyleSetName;
}

const ISlateStyle& FOpenDriveEditorModeStyleSet::Get()
{
	return *OpenDriveStyleInstance;
}

void FOpenDriveEditorModeStyleSet::Create()
{
	OpenDriveStyleInstance = MakeShareable(new FSlateStyleSet("OpenDriveStyle"));
	OpenDriveStyleInstance->SetContentRoot(FPaths::ProjectPluginsDir() / TEXT("OpenDRIVE/Content/EditorRessources"));
	OpenDriveStyleInstance->SetCoreContentRoot(FPaths::ProjectPluginsDir() / TEXT("OpenDRIVE/Content/EditorRessources"));
	CreateIcons();
	//CreateButtonsStyle();
}

void FOpenDriveEditorModeStyleSet::CreateIcons()
{
	const FVector2D Icon16X16(16.f, 16.f);
	const FVector2D Icon32X32(32.f, 32.f);
	const FVector2D Icon64X64(64.f, 64.f);

	OpenDriveStyleInstance->Set("OpenDriveEditorModeCommands.OpenDriveSwitchToEditorMode", new IMAGE_BRUSH(TEXT("ASAMIcon"), Icon64X64));
	OpenDriveStyleInstance->Set("OpenDriveEditorModeCommands.OpenDriveSwitchToEditorMode.Small", new IMAGE_BRUSH(TEXT("ASAMIcon"), Icon32X32));

	OpenDriveStyleInstance->Set("OpenDriveEditorModeCommands.OpenDriveVisualizerTool", new IMAGE_BRUSH(TEXT("RoadVisualizerIcon"), Icon32X32));
	OpenDriveStyleInstance->Set("OpenDriveEditorModeCommands.OpenDriveVisualizerTool.Small", new IMAGE_BRUSH(TEXT("RoadVisualizerIcon"), Icon16X16));

	OpenDriveStyleInstance->Set("OpenDriveEditorModeCommands.OpenDriveUtilsTool", new IMAGE_BRUSH(TEXT("ASAMIcon"), Icon32X32));
	OpenDriveStyleInstance->Set("OpenDriveEditorModeCommands.OpenDriveUtilsTool.Small", new IMAGE_BRUSH(TEXT("ASAMIcon"), Icon16X16));
}

void FOpenDriveEditorModeStyleSet::CreateButtonsStyle()
{
	const FButtonStyle ButtonStyle = FAppStyle::Get().GetWidgetStyle<FButtonStyle>("ToolbarButton");
	const FButtonStyle OpenDriveEditorModeButtonStyle = ButtonStyle;	
	OpenDriveStyleInstance->Set("OpenDriveEditor.ToolbarButton", OpenDriveEditorModeButtonStyle);
}

