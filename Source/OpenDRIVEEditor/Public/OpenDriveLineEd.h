// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "RoadManager.hpp"
#include "Components/SplineComponent.h"
#include "CoordTranslate.h"
#include "OpenDriveLineEd.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENDRIVEEDITOR_API UOpenDriveLineEd : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDriveLineEd();

	void Init(roadmanager::Lane* lane_, double length_);

protected:

	roadmanager::Lane* lane;

	USplineComponent* LaneSpline;
};
