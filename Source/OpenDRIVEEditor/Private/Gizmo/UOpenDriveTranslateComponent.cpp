#include "Gizmo/UOpenDriveTranslateComponent.h"

// Sets default values for this component's properties
UOpenDriveTranslateComponent::UOpenDriveTranslateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bIsEditorOnly = true;
	Position = NewObject<UOpenDrivePosition>();

	CalculatePivotPosition();
}

void UOpenDriveTranslateComponent::CalculatePivotPosition()
{
	AActor* owner = GetOwner();

	if (IsValid(owner))
	{
		FVector origin, boundsExtent;
		owner->GetActorBounds(false, origin, boundsExtent);
		PivotPosition = boundsExtent.Z / 2;
	}
}



