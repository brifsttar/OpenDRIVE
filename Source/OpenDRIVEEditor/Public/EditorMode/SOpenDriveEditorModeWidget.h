#pragma once 
#include "../../../OpenDRIVE/Public/OpenDriveAsset.h"
#include "PropertyCustomizationHelpers.h"
#include "Framework/Application/SlateApplication.h"

class SOpenDRIVEEditorModeWidget : public SCompoundWidget
{
public :

	SLATE_BEGIN_ARGS(SOpenDRIVEEditorModeWidget) {}
	SLATE_END_ARGS();

	/**
	 * Constructs the window
	 */
	void Construct(const FArguments& InArgs);
	
	/**
	 * Constructs the lane info box
	 */
	TSharedRef<SBorder> ConstructLaneInfoBox(const FArguments& InArgs);

	/**
	 *  Constructs the generate and reset buttons
	 */
	TSharedRef<SHorizontalBox> ConstructButtons(const FArguments& InArgs);

	/**
	 * Returns the Editor mode 
	 * @return The OpenDRIVE Editor Mode
	 */
	class FOpenDRIVEEditorMode* GetEdMode() const;

	//Link between the buttons and the editor mode functions
	FReply Generate();
	FReply Reset();

	/**
	 * Gets if roads are loaded or not
	 * @return True if loaded, False if not
	 */
	bool IsLoaded() const;

	/**
	 * Gets if editor is in simulation mode or not
	 * @return True if simulating, False if not
	 */
	bool CheckIfSimulating() const;

	/**
	 * Displays the last selected lane's informations
	 * @param lane The lane
	 */
	void UpdateLaneInfo(AOpenDriveRoadEd* lane_);

	/**
	 * Sets the road offset 
	 * @param newOffset The new offset
	 */
	void SetOffset(const FText &newOffset);

	/**
	 * Called when an openDRIVE asset is selected, and sends it to the editor mode.
	 * @param assetData_ The new object's data 
	 */
	void OnObjectChanged(const FAssetData& assetData_);

	/**
	* Sets the corresponding FAssetData to the selected object in the PropertyEntryBox
	* @param assetData_ the FAssetData 
	*/
	inline void SetAssetData(const FAssetData& assetData_) { _assetData = assetData_; };

	/**
	* Called when arrows' visibility chekbox state is changed 
	* @param state the new checkbox state
	*/
	void OnCheckStateChanged(ECheckBoxState state);

private : 
	//Lane informations text blocks
	TSharedPtr<STextBlock> RoadIdTextPtr;
	TSharedPtr<STextBlock> JunctionIdTextPtr;
	TSharedPtr<STextBlock> LaneIdTextPtr;
	TSharedPtr<STextBlock> LaneTypeTextPtr;

	//openDRIVE asset box property
	TSharedPtr<SObjectPropertyEntryBox> OpenDRIVEAssetProBoxPtr;
	FAssetData _assetData;
	TSharedPtr<FAssetThumbnailPool> AssetThumbnailPoolPtr;

	/**
	 * @return The current asset data path.
	 */
	inline FString GetAssetDataPath() const { return _assetData.IsValid() ? _assetData.ObjectPath.ToString() : ""; };
};
