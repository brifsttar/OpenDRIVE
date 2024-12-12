#pragma once
#include "CoreMinimal.h"
#include "OpenDrivePosition.h"
#include "BaseGizmos/GizmoInterfaces.h"
#include "UObject/Object.h"
#include "OpenDriveGizmoAxisSource.generated.h"

/**
 * 
 */
UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveGizmoAxisSource : public UObject, public IGizmoAxisSource
{
	GENERATED_BODY()

public :

	void Initialize();
	
	virtual FVector GetOrigin() const override;
	virtual FVector GetDirection() const override;
	virtual bool HasTangentVectors() const override;
	virtual void GetTangentVectors(FVector& TangentXOut, FVector& TangentYOut) const override;

	FORCEINLINE void SetDirection(const FVector& NewDirection) { Direction = NewDirection; }
	FORCEINLINE void SetUpdateDirection(const bool bFreeze) { bFreezeDirection = bFreeze; }
	
	UPROPERTY()
	USceneComponent* Component;

	UPROPERTY()
	int AxisIndex = 0; // 0=S 1=T 2=ChangeLane

	UPROPERTY()
	FVector Direction;

	UPROPERTY()
	UOpenDrivePosition* OpenDrivePosition;
	
	DECLARE_DELEGATE_OneParam(FOnDirectionChanged, const FVector&);
	FOnDirectionChanged OnDirectionChanged;

	static UOpenDriveGizmoAxisSource* Construct(
		USceneComponent* ComponentIn,
		const int AxisIndexIn,
		UObject* Outer = GetTransientPackage())
	{
		UOpenDriveGizmoAxisSource* NewSource = NewObject<UOpenDriveGizmoAxisSource>(Outer);
		NewSource->Component = ComponentIn;
		NewSource->AxisIndex = FMath::Clamp(AxisIndexIn, 0,2);
		NewSource->OpenDrivePosition = NewObject<UOpenDrivePosition>();
		NewSource->Initialize();
		return NewSource;
	}

private :

	UPROPERTY()
	bool bFreezeDirection;

};
