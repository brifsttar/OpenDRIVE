//#include "OpenDriveEdModeTool.h"
//#include "OpenDriveEditor.h"
//#include "OpenDriveEditorMode.h"
//
//#define IMAGE_BRUSH(RelativePath, ...)FSlateImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
//
//TSharedPtr < FSlateStyleSet > OpenDRIVEEdModeTool::StyleSet = nullptr;
//
//void OpenDRIVEEdModeTool::OnStartupModule()
//{
//	RegisterStyleSet();
//	RegisterEditorMode();
//}
//
//void OpenDRIVEEdModeTool::OnShutdownModule()
//{
//	UnregisterStyleSet();
//	UnregisterEditorMode();
//}
//
//void OpenDRIVEEdModeTool::RegisterStyleSet()
//{
//	const FVector2D Icon20x20(20.f, 20.f);
//	const FVector2D Icon40x40(40.f, 40.f);
//
//	if (StyleSet.IsValid())
//	{
//		return;
//	}
//
//	StyleSet = MakeShareable(new FSlateStyleSet("OpenDRIVEEdModeToolStyle"));
//	StyleSet->SetContentRoot(FPaths::ProjectPluginsDir() / TEXT("OpenDRIVE/Content/EditorRessources"));
//	StyleSet->SetCoreContentRoot(FPaths::ProjectPluginsDir() / TEXT("OpenDRIVE/Content/EditorRessources"));
//
//	StyleSet->Set("OpenDRIVEEditorMode", new IMAGE_BRUSH(TEXT("IconRoadRunnerEditorMode"), Icon40x40));
//	StyleSet->Set("OpenDRIVEEditorMode.Small", new IMAGE_BRUSH(TEXT("IconRoadRunnerEditorMode"), Icon20x20));
//	
//
//	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
//}
//
//void OpenDRIVEEdModeTool::UnregisterStyleSet()
//{
//	if (StyleSet.IsValid())
//	{
//		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
//		ensure(StyleSet.IsUnique());
//		StyleSet.Reset();
//	}
//}
//
//void OpenDRIVEEdModeTool::RegisterEditorMode()
//{
//	FEditorModeRegistry::Get().RegisterMode<FOpenDRIVEEditorMode>(
//		FOpenDRIVEEditorMode::EM_RoadMode,
//		FText::FromString("OpenDRIVE"),
//		FSlateIcon(StyleSet->GetStyleSetName(), "OpenDRIVEEditorMode", "OpenDRIVEEditorMode.Small"), true, 1
//	);
//}
//
//void OpenDRIVEEdModeTool::UnregisterEditorMode()
//{
//	FEditorModeRegistry::Get().UnregisterMode(FOpenDRIVEEditorMode::EM_RoadMode);
//}
//
//#undef IMAGE_BRUSH
