#pragma once
#include "CoreMinimal.h"
#include "BaseGizmos/GizmoActor.h"
#include "BaseGizmos/GizmoViewContext.h"
#include "OpenDriveGizmoActor.generated.h"

UCLASS(Transient, NotPlaceable, Hidden, NotBlueprintable, NotBlueprintType)
class OPENDRIVEEDITOR_API AOpenDriveGizmoActor : public AGizmoActor
{
	GENERATED_BODY()

public:
	
	AOpenDriveGizmoActor();
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

class FOpenDriveGizmoActorFactory
{
public :

	FOpenDriveGizmoActorFactory(UGizmoViewContext* GizmoViewContextIn)
	{
		GizmoViewContext = GizmoViewContextIn;
	}

	virtual AOpenDriveGizmoActor* CreateNewGizmoActor(UWorld* World) const;

protected:
	/**
	 * The default gizmos that we use need to have the current view information stored for them via
	 * the ITF context store so that they can figure out how big they are for hit testing, so this
	 * pointer needs to be set (and kept alive elsewhere) for the actor factory to work properly.
	 */
	UGizmoViewContext* GizmoViewContext = nullptr;
};
