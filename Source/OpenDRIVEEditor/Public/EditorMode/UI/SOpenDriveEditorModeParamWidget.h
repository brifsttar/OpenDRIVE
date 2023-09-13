#pragma once
#include "PropertyCustomizationHelpers.h"
#include "Framework/Application/SlateApplication.h"

class SOpenDRIVEEditorParamWidget : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SOpenDRIVEEditorParamWidget) {}
	SLATE_END_ARGS();

	/**
	* Constructs the lane info boxes
	*/
	void Construct(const FArguments& InArgs);

protected :

	/**
	*  Constructs the generate and reset buttons
	*/
	TSharedRef<SHorizontalBox> ConstructButtons(const FArguments& InArgs);

	/**
	* Constructs the road generation parameters box (offset / step)
	*/
	TSharedRef<SBorder> ConstructRoadGenerationParameters(const FArguments& InArgs);
	
	/**
	* Gets if roads are loaded or not
	* @return True if loaded, False if not
	*/
	bool IsLoaded() const;

	/**
	* Link the Generate() function in the OpenDRIVEEditorMode.cpp file
	*/
	FReply Generate();

	/**
	* Link the Reset() function in the OpenDRIVEEditorMode.cpp file
	*/
	FReply Reset();

	/**
	 * Gets if editor is in simulation mode or not
	 * @return True if simulating, False if not
	 */
	bool CheckIfInEditorMode() const;

	/**
	 * Sets the road offset
	 * @param newOffset The new offset
	 */
	void SetOffset(const FText& newOffset);

	/**
	* Called when arrows' visibility chekbox state is changed
	* @param state the new checkbox state
	*/
	void OnCheckStateChanged(ECheckBoxState state);

	/**
	* Called when the offset value is changed with the slider
	* @param value The new value
	*/
	void OnOffsetValueChanged(float value);

	/**
	* Called when the step value is changed with the slider
	* @param value The new value
	*/
	void OnStepValueChanged(float value);

	 /**
	 * Returns the Editor mode
	 * @return The OpenDRIVE Editor Mode
	 */
	 class FOpenDRIVEEditorMode* GetEdMode() const;

private:

	//Text font
	TSharedPtr<FSlateFontInfo> _fontInfoPtr;

	//Tickbox 
	TSharedPtr<SCheckBox> _showArrowsCheckBox;

	//Slider bars's texts 
	TSharedPtr<STextBlock> _stepTextPtr;
	TSharedPtr<STextBlock> _offsetTextPtr;
};