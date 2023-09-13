#pragma once 
#include "EditorModes.h"
#include "EdMode.h"
#include "../OpenDriveEditorLane.h"
#include "../../../OpenDRIVE/Public/OpenDRIVEActor.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLaneSelected, AOpenDriveEditorLane* road)

class FOpenDRIVEEditorMode : public FEdMode
{
public :

	const static FEditorModeID EM_RoadMode;

	FOpenDRIVEEditorMode();

	~FOpenDRIVEEditorMode();

	/**
	* Called everytime the editor mode is entered
	*/
	virtual void Enter() override;

	/**
	* Called everytime the editor mode is closed 
	*/
	virtual void Exit() override;

	/**
	 * Gets if the roads are drawn or not. 
	 * @return true if loaded, false if not
	 */
	inline bool GetHasBeenLoaded() const { return bHasBeenLoaded; };

	/**
	 * Deletes drawn roads.
	 */
	void ResetLanesArray();

	/**
	 * Generates roads.
	 * It will call Reset() if there's already a generation done.
	 */
	void Generate();

	/**
	 * Sets the road offset 
	 * @param newOffset_ The new offset
	 */
	inline void SetRoadOffset(float newOffset_) { _roadOffset = newOffset_;};

	/**
	 * @return The road offset
	 */
	inline float GetRoadOffset() { return _roadOffset; };

	/**
	* Sets the step for the roads' lanes drawing
	* @param The new step 
	*/
	inline void SetStep(float newStep_) { _step = newStep_; };

	/**
	* @return The step
	*/
	inline float GetStep() { return _step; };

	/**
	* Sets roads' arrows visibility
	* @param bIsVisible True for visible false for hidden
	*/
	void SetRoadsArrowsVisibilityInEditor(bool bIsVisible);

	/**
	* Sets the roads visibility in editor only
	* @param bIsVisible True for visible, False for hidden
	*/
	void SetRoadsVisibilityInEditor(bool bIsVisible);

protected :

	/**
	 * Loads roads 
	 */
	void LoadRoadsNetwork();

	TArray<AOpenDriveEditorLane*> roadsArray;

private :

	AOpenDRIVEActor* _openDriveActor;

	float _roadOffset = 20.0f;
	float _step = 5.f;
	bool bHasBeenLoaded = false;

	FDelegateHandle MapOpenedDelegateHandle;
	/**
	* Called when a new level is opened (or created)
	* @param type MapChangeEventFlags namespace flag
	*/
	void OnMapOpenedCallback(uint32 type);
	bool bIsMapOpening = false;

	FDelegateHandle OnActorSelectedHandle;
	/**
	* Called when an actor is selected in editor 
	* @param selectedObject The selected object 
	*/
	void OnActorSelected(UObject* _selectedObject);
};
