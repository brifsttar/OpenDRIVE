#pragma once 
#include "../TabToolBase.h"

class OpenDRIVEEdModeTool : public FTabToolBase
{
	virtual void OnStartupModule() override;
	virtual void OnShutdownModule() override;

private :

	static TSharedPtr<class FSlateStyleSet> StyleSet;

	void RegisterStyleSet();
	void UnregisterStyleSet();

	void RegisterEditorMode();
	void UnregisterEditorMode();
};