#pragma once
#include "CoreMinimal.h"
#include "InteractiveToolObjects.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "RoadManager.hpp"
#include "OpenDriveEditorLane.generated.h"

class UProceduralMeshComponent;

UCLASS(Transient, Hidden, NotBlueprintable, NotBlueprintType, NotPlaceable)
class OPENDRIVEEDITOR_API AOpenDriveEditorLane : public AInternalToolFrameworkActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOpenDriveEditorLane();

	/**
	 * Initializes the lane parameters and draw it 
	 * @param RoadIn The road Id
	 * @param LaneSectionIn The Roadmanager's lane section 
	 * @param LaneIn The Roadmanager's lane
	 * @param Offset The road offset
	 * @param Step The step (the lower it is, the more precise it will be)  
	 */
	void Initialize(roadmanager::Road* RoadIn, roadmanager::LaneSection* LaneSectionIn, roadmanager::Lane* LaneIn, float Offset, float Step);

	/**
	* Gets the road Id
	* @return The Road Id
	*/
	int GetRoadId() const { return Road->GetId(); };

	/**
	* Gets the junction Id
	* @return The Junction Id
	*/
	int GetJunctionId() const { return Road->GetJunction(); };

	/**
	* Gets the lane type
	* @return The lane type 
	*/
	FString GetLaneType() const;

	/**
	* Gets the lane Id
	* @return The lane Id
	*/
	int GetLaneId() const { return Lane->GetId(); };

	/**
	* @return The successor's road Id
	*/
	int GetSuccessorId() const;

	/**
	* @return The predecessor's road Id
	*/
	int GetPredecessorId() const;

protected : 

	/**
	 * Builds the lane mesh
	 * @param SampleStepMeters The step used to sample the lane (meters)
	 * @param ZOffset The Z offset applied to the lane's mesh (centimeters)
	 */
	void BuildLaneMesh(float SampleStepMeters = 0.01f, float ZOffset = 50.0f);

private : 

	roadmanager::Road* Road;
	roadmanager::LaneSection* LaneSection;
	roadmanager::Lane* Lane;
};
