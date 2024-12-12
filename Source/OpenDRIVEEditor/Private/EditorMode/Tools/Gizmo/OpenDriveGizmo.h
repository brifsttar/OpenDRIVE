#pragma once
#include "CoreMinimal.h"
#include "InteractiveGizmo.h"
#include "InteractiveGizmoBuilder.h"
#include "OpenDriveGizmoActor.h"
#include "BaseGizmos/TransformProxy.h"
#include "BaseGizmos/TransformSubGizmoUtil.h"
#include "OpenDrivePosition.h"
#include "OpenDriveGizmo.generated.h"

class UOpenDriveTranslationGizmo;
class UOpenDriveGizmoAxisSource;

struct FGizmoSharedState
{
	UOpenDriveGizmoAxisSource *AxisSSource = nullptr;
	UOpenDriveGizmoAxisSource *AxisTSource = nullptr;
	UOpenDriveGizmoAxisSource *AxisChangeLaneSource = nullptr;
	UGizmoTransformChangeStateTarget* StateTarget = nullptr;
	UGizmoScaledAndUnscaledTransformSources* TransformSource = nullptr;
};

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
	virtual void Tick(float DeltaTime) override;

	void SetActiveTarget(UTransformProxy* Target, IToolContextTransactionProvider* TransactionProvider);
	void ClearActiveTarget();

	void SetVisibility(bool bVisible) const;

	void AutoAlignWithLane(const bool bAutoAlignWithLane) const;

	FORCEINLINE bool GetShouldAlignWithLane() const {return bAlignWithLane;}

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
	TObjectPtr<UOpenDriveTranslationGizmo> TranslationSGizmo;

	UPROPERTY()
	TObjectPtr<UOpenDriveTranslationGizmo> TranslationTGizmo;

	UPROPERTY()
	TObjectPtr<UOpenDriveTranslationGizmo> ChangeLaneGizmo;
	
	UPROPERTY()
	UGizmoViewContext* GizmoViewContext;

	UPROPERTY()
	TObjectPtr<UTransformProxy> ActiveTarget;

	UPROPERTY()
	UOpenDrivePosition* OpenDrivePosition;
	
protected :

	TUniquePtr<FGizmoSharedState> TransformSubGizmoSharedState;

	UPROPERTY()
	TObjectPtr<UGizmoTransformChangeStateTarget> StateTarget;

	UPROPERTY()
	TObjectPtr<UOpenDriveGizmoAxisSource> AxisSSource;
	
	UPROPERTY()
	TObjectPtr<UOpenDriveGizmoAxisSource> AxisTSource;

	UPROPERTY()
	TObjectPtr<UOpenDriveGizmoAxisSource> AxisChangeLaneSource;

	UPROPERTY()
	bool bAlignWithLane;
};


