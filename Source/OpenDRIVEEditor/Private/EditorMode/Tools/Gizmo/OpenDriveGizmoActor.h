#pragma once
#include "CoreMinimal.h"
#include "BaseGizmos/GizmoActor.h"
#include "OpenDriveGizmoActor.generated.h"

UCLASS(Transient, NotPlaceable, Hidden, NotBlueprintable, NotBlueprintType)
class OPENDRIVEEDITOR_API AOpenDriveGizmoActor : public AGizmoActor
{
	GENERATED_BODY()

public:
	
	AOpenDriveGizmoActor();

	static AOpenDriveGizmoActor* ConstructOpenDriveGizmoActor(UWorld* World, UGizmoViewContext* GizmoViewContext);
public :

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> MoveAlongS;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> MoveAlongT;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> ChangeLane;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> AlignWithLane;
};
