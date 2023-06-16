#pragma once 
#include "Framework/Application/SlateApplication.h"

class SOpenDRIVEEditorModeWidget : public SCompoundWidget
{
public :

	SLATE_BEGIN_ARGS(SOpenDRIVEEditorModeWidget) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs);

	class FOpenDRIVEEditorMode* GetEdMode() const;
};