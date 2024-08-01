#include "EditorMode/OpenDriveEditorModeStyle.h"

#define IMAGE_BRUSH(RelativePath, ...)FSlateImageBrush(OpenDriveStyleInstance->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

TSharedPtr<FSlateStyleSet> FOpenDriveEditorModeStyleSet::OpenDriveStyleInstance = NULL;

void FOpenDriveEditorModeStyleSet::OnStartupModule()
{
	Initialize();
}

void FOpenDriveEditorModeStyleSet::OnShutdownModule()
{
	Shutdown();
}

void FOpenDriveEditorModeStyleSet::Initialize()
{
	if (!OpenDriveStyleInstance.IsValid())
	{
		Create();
		FSlateStyleRegistry::RegisterSlateStyle(*OpenDriveStyleInstance);
	}
}

void FOpenDriveEditorModeStyleSet::Shutdown()
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
	CreateButtonsStyle();
}

void FOpenDriveEditorModeStyleSet::CreateIcons()
{
	const FVector2D icon32x32(32.f, 32.f);
	const FVector2D icon64x64(64.f, 64.f);

	OpenDriveStyleInstance->Set("OpenDriveVisualizerToolIcon", new IMAGE_BRUSH(TEXT("RoadVisualizerIcon"), icon64x64));
	OpenDriveStyleInstance->Set("OpenDriveVisualizerToolIcon.Small", new IMAGE_BRUSH(TEXT("RoadVisualizerIcon"), icon32x32));

	OpenDriveStyleInstance->Set("OpenDriveEditorModeIcon", new IMAGE_BRUSH(TEXT("ASAMIcon"), icon64x64));
	OpenDriveStyleInstance->Set("OpenDriveEditorModeIcon.Small", new IMAGE_BRUSH(TEXT("ASAMIcon"), icon32x32));

	OpenDriveStyleInstance->Set("OpenDriveUtilsToolIcon", new IMAGE_BRUSH(TEXT("ASAMIcon"), icon64x64));
	OpenDriveStyleInstance->Set("OpenDriveUtilsToolIcon.Small", new IMAGE_BRUSH(TEXT("ASAMIcon"), icon32x32));
}

void FOpenDriveEditorModeStyleSet::CreateButtonsStyle()
{
	const FButtonStyle ButtonStyle = FAppStyle::Get().GetWidgetStyle<FButtonStyle>("ToolbarButton");

	FButtonStyle OpenDriveEditorModeButtonStyle = ButtonStyle;	

	OpenDriveStyleInstance->Set("OpenDriveEditor.ToolbarButton", OpenDriveEditorModeButtonStyle);
}

