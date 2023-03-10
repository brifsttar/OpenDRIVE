// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorActionUtility.h"
#include "Components/SplineComponent.h"
#include "OpenDrive2Landscape.generated.h"

class ALandscapeProxy;

/**
 * 
 */
UCLASS(Abstract, hideCategories = (Object), Blueprintable)
class OPENDRIVEEDITOR_API UOpenDrive2Landscape : public UActorActionUtility
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UOpenDrive2Landscape();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "OpenDRIVE2Landscape", meta = (AllowPrivateAccess = "true"))
	USplineComponent* Spline;

	/**
	* Sculpts the current OpenDRIVE roads into the selected landscapes
	* @param RoadZOffset Additional Z-offset (cm) to apply to sculpted roads (to dig deeper than surface)
	* @param Falloff Lateral distance (cm) to transition from road to existing landscape
	* @param PaintLayer Layer to paint on the landscape where roads are present
	*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Assets")
	void SculptLandscape(
		float RoadZOffset = -10.f,
		float Falloff = 100.f,
		ULandscapeLayerInfoObject *PaintLayer = 0,
		FName LayerName = FName(TEXT("Layer"))
	);

	/**
	* Creates landscape splines for all OpenDRIVE roads
	* @param RoadZOffset Additional Z-offset (cm) to apply to sculpted roads (to dig deeper than surface)
	* @param Falloff Lateral distance (cm) to transition from road to existing landscape
	* @param PaintLayer Layer to paint on the landscape where roads are present
	*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Assets")
	void CreateRoadSplines(
		float RoadZOffset = -10.f,
		float Falloff = 100.f,
		ULandscapeLayerInfoObject *PaintLayer = 0,
		FName LayerName = FName(TEXT("Layer"))
	);

	UFUNCTION(BlueprintImplementableEvent, Category = "Assets")
	void ApplySpline(
		ALandscapeProxy *Landscape,
		USplineComponent *SplineComponent,
		float WidthStart,
		float WidthEnd,
		float Falloff,
		ULandscapeLayerInfoObject *PaintLayer,
		FName LayerName
	);
};
