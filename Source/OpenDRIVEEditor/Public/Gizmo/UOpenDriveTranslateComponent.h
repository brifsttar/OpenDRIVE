#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OpenDrivePosition.h"
#include "Engine/Selection.h" 
#include "UOpenDriveTranslateComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Transient)
class OPENDRIVEEDITOR_API UOpenDriveTranslateComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOpenDriveTranslateComponent();
	UOpenDrivePosition* Position;
	float PivotPosition;

private :
	void CalculatePivotPosition();
};
