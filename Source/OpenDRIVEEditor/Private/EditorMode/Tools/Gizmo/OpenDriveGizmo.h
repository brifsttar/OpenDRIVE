#pragma once
#include "CoreMinimal.h"
#include "InteractiveGizmo.h"
#include "InteractiveGizmoBuilder.h"
#include "OpenDriveGizmoActor.h"
#include "BaseGizmos/TransformProxy.h"
#include "BaseGizmos/TransformSubGizmoUtil.h"
#include "OpenDrivePosition.h"
#include "OpenDriveGizmo.generated.h"

UCLASS()
class UOpenDriveGizmoBuilder : public UInteractiveGizmoBuilder
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FString LaneChangeGizmoBuilderIdentifier;
	UPROPERTY()
	FString MoveAlongLaneGizmoBuilderIdentifier;
	virtual UInteractiveGizmo* BuildGizmo(const FToolBuilderState& SceneState) const override;
};

/**
 * 
 */
UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveGizmo : public UInteractiveGizmo
{
	GENERATED_BODY()

public :

	// InteractiveGizmo implementation
	virtual void Setup() override;
	virtual void Shutdown() override;

	void SetActiveTarget(UTransformProxy* Target, IToolContextTransactionProvider* TransactionProvider);
	void ClearActiveTarget();

	void SetVisibility(bool bVisible) const;

	UPROPERTY()
	FString LaneChangeGizmoBuilderIdentifier;

	UPROPERTY()
	FString MoveAlongLaneGizmoBuilderIdentifier;

	UPROPERTY()
	UWorld* World;

	UPROPERTY()
	AOpenDriveGizmoActor* GizmoActor;

	UPROPERTY()
	TArray<TObjectPtr<UInteractiveGizmo>> ActiveGizmos;
	
	UPROPERTY()
	UGizmoViewContext* GizmoViewContext;

	UPROPERTY()
	TObjectPtr<UTransformProxy> ActiveTarget;

	UPROPERTY()
	UOpenDrivePosition* OpenDrivePosition;

protected :

	TUniquePtr<UE::GizmoUtil::FTransformSubGizmoSharedState> TransformSubGizmoSharedState;

	UPROPERTY()
	TObjectPtr<UGizmoTransformChangeStateTarget> StateTarget;

	UPROPERTY()
	TObjectPtr<UGizmoComponentAxisSource> AxisXSource;
	
	UPROPERTY()
	TObjectPtr<UGizmoComponentAxisSource> AxisYSource;
};
