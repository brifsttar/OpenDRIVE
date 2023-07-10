#pragma once 
#include "EditorModes.h"
#include "EdMode.h"
#include "../OpenDriveRoadEd.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLaneSelected, AOpenDriveRoadEd* road)

class FOpenDRIVEEditorMode : public FEdMode
{
public :

	const static FEditorModeID EM_RoadMode;

	FOpenDRIVEEditorMode();

	// FEdMode base classes
	virtual void Enter() override;
	virtual void Exit() override;

	/**
	 * Gets if roads are drawn or not. 
	 * @return True if loaded, False if not
	 */
	inline bool GetHasBeenLoaded() const { return bHasBeenLoaded; };

	/**
	 * Deletes drawn roads.
	 */
	void Reset();

	/**
	 * Generates roads.
	 * It will call Reset() if there's already a generation done.
	 */
	void Generate();

	/**
	 * @return The drawn roads array
	 */
	inline TArray<AOpenDriveRoadEd*> GetRoads() const { return FRoadsArray; };

	~FOpenDRIVEEditorMode();

	/**
	 * Sets the road offset 
	 * @param newOffset_ The new offset
	 */
	inline void SetRoadOffset(float newOffset_) { _roadOffset = newOffset_;};

	/**
	 * @return The road offset
	 */
	inline float GetRoadOffset() { return _roadOffset; };

protected :

	/**
	 * Loads roads 
	 */
	void LoadRoads();

	/**
	 * Sets the roads visibility in editor only
	 * @param bIsVisible True for visible, False for hidden
	 */
	void SetRoadsVisibilityInEditor(bool bIsVisible);

	TArray<AOpenDriveRoadEd*> FRoadsArray;

private :

	float _roadOffset = 1.0f;

	bool bHasBeenLoaded = false;

	// Delegate used to detect when we load an existing map from the content browser 
	// Note : doesn't seem to work if you create a new level without saving it.
	FDelegateHandle MapOpenedDelegateHandle;
	void OnMapOpenedCallback(const FString& mapName, bool bLoadAsTemplate);
	bool bIsMapOpening;

	// When an actor is selected. Used to send and display useful road info in the editor mode widget.
	FDelegateHandle OnActorSelectedHandle;
	void OnActorSelected(UObject* _selectedObject);
};