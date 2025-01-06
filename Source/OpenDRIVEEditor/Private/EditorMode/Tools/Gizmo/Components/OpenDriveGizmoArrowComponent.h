#pragma once
#include "CoreMinimal.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "OpenDriveGizmoArrowComponent.generated.h"

//todo look deeper into GizmoComponent to improve this one, which is just a clone of UGizmoBaseArrowComponent, without the annoying axes flipping

UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveGizmoArrowComponent : public UGizmoBaseComponent
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	FVector Direction = FVector::UpVector;

	UPROPERTY()
	float Gap = 5.0f;

	UPROPERTY()
	float Length = 100.0f;

	UPROPERTY()
	float Thickness = 2.0f;

	static UOpenDriveGizmoArrowComponent* AddOpenDriveArrowComponent(
	UWorld* World, AActor* Actor, UGizmoViewContext* GizmoViewContext,
	const FLinearColor& Color, const FVector& LocalDirection, const float Length = 80.0f);

private :

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual bool LineTraceComponent(FHitResult& OutHit, const FVector Start, const FVector End, const FCollisionQueryParams& Params) override;
	//~ End UPrimitiveComponent Interface.

	//~ Begin USceneComponent Interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ Begin USceneComponent Interface.


};
