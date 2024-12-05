#include "OpenDriveGizmo.h"
#include "ContextObjectStore.h"
#include "InteractiveGizmoManager.h"
#include "InteractiveToolManager.h"
#include "SubGizmos/OpenDriveTranslationGizmo.h"
#include "OpenDrivePosition.h"
#include "SubGizmos/OpenDriveAlignToLaneGizmo.h"
#include "BaseGizmos/GizmoViewContext.h"

UInteractiveGizmo* UOpenDriveGizmoBuilder::BuildGizmo(const FToolBuilderState& SceneState) const
{
	UOpenDriveGizmo* Gizmo = NewObject<UOpenDriveGizmo>(SceneState.GizmoManager);
	Gizmo->World = SceneState.World;
	UGizmoViewContext* GizmoViewContext = SceneState.ToolManager->GetContextObjectStore()->FindContext<UGizmoViewContext>();
	check(GizmoViewContext && GizmoViewContext->IsValidLowLevel());
	Gizmo->GizmoViewContext = GizmoViewContext;
	Gizmo->LaneChangeGizmoBuilderIdentifier = LaneChangeGizmoBuilderIdentifier;
	Gizmo->MoveAlongLaneGizmoBuilderIdentifier = MoveAlongLaneGizmoBuilderIdentifier;
	return Gizmo;
}

void UOpenDriveGizmo::Setup()
{
	Super::Setup();
	OpenDrivePosition = NewObject<UOpenDrivePosition>();
	GizmoActor = AOpenDriveGizmoActor::ConstructOpenDriveGizmoActor(World, GizmoViewContext);
}

void UOpenDriveGizmo::Shutdown()
{
	ClearActiveTarget();

	if (GizmoActor)
	{
		GizmoActor->Destroy();
		GizmoActor = nullptr;
	}
	
	Super::Shutdown();
}

void UOpenDriveGizmo::SetActiveTarget(UTransformProxy* Target, IToolContextTransactionProvider* TransactionProvider)
{
	if (ActiveTarget != nullptr)
	{
		ClearActiveTarget();
	}

	ActiveTarget = Target;
	USceneComponent* GizmoComponent = GizmoActor->GetRootComponent();
	FTransform TargetTransform = Target->GetTransform();
	FTransform GizmoTransform = TargetTransform;
	GizmoTransform.SetScale3D(FVector::OneVector);
	GizmoComponent->SetWorldTransform(GizmoTransform);

	TransformSubGizmoSharedState = MakeUnique<UE::GizmoUtil::FTransformSubGizmoSharedState>();
	
	if (UOpenDriveTranslationGizmo* MoveAlongSGizmo = Cast<UOpenDriveTranslationGizmo>(GetGizmoManager()->CreateGizmo(MoveAlongLaneGizmoBuilderIdentifier)))
	{
		MoveAlongSGizmo->Initialize(GizmoActor->MoveAlongS, Target, TransactionProvider, TransformSubGizmoSharedState.Get(), 0, EOpenDriveSourceType::TranslateOnS);
		ActiveGizmos.Add(MoveAlongSGizmo);
	}
	if (UOpenDriveTranslationGizmo* MoveAlongTGizmo = Cast<UOpenDriveTranslationGizmo>(GetGizmoManager()->CreateGizmo(MoveAlongLaneGizmoBuilderIdentifier)))
	{
		MoveAlongTGizmo->Initialize(GizmoActor->MoveAlongT, Target, TransactionProvider, TransformSubGizmoSharedState.Get(), 1, EOpenDriveSourceType::TranslateOnT);
		ActiveGizmos.Add(MoveAlongTGizmo);
	}
	if (UOpenDriveTranslationGizmo* ChangeLaneGizmo = Cast<UOpenDriveTranslationGizmo>(GetGizmoManager()->CreateGizmo(MoveAlongLaneGizmoBuilderIdentifier)))
	{
		ChangeLaneGizmo->Initialize(GizmoActor->ChangeLane, Target, TransactionProvider, TransformSubGizmoSharedState.Get(), 1, EOpenDriveSourceType::ChangeLane);
		ActiveGizmos.Add(ChangeLaneGizmo);
	}
	if (UOpenDriveAlignToLaneGizmo* AlignToLaneGizmo = Cast<UOpenDriveAlignToLaneGizmo>(GetGizmoManager()->CreateGizmo(LaneChangeGizmoBuilderIdentifier)))
	{
		AlignToLaneGizmo->Initialize(GizmoActor->AlignWithLane, Target, TransactionProvider, TransformSubGizmoSharedState.Get());
		ActiveGizmos.Add(AlignToLaneGizmo);
	}

	AxisXSource = TransformSubGizmoSharedState->CardinalAxisSources[0];
	AxisYSource = TransformSubGizmoSharedState->CardinalAxisSources[1];
	StateTarget = TransformSubGizmoSharedState->StateTarget;
}

void UOpenDriveGizmo::ClearActiveTarget()
{
	for (UInteractiveGizmo* Gizmo : ActiveGizmos)
	{
		GetGizmoManager()->DestroyGizmo(Gizmo);
	}
	ActiveGizmos.SetNum(0);
	AxisXSource = nullptr;
	AxisYSource = nullptr;
	StateTarget = nullptr;
	ActiveTarget = nullptr;
}

void UOpenDriveGizmo::SetVisibility(bool bVisible) const
{
	GizmoActor->SetActorHiddenInGame(bVisible == false);
#if WITH_EDITOR
	GizmoActor->SetIsTemporarilyHiddenInEditor(bVisible == false);
#endif
}
