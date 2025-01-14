#include "OpenDriveGizmo.h"
#include "ContextObjectStore.h"
#include "InteractiveGizmoManager.h"
#include "InteractiveToolManager.h"
#include "SubGizmos/OpenDriveTranslationGizmo.h"
#include "OpenDrivePosition.h"
#include "SubGizmos/OpenDriveAlignToLaneGizmo.h"
#include "BaseGizmos/GizmoViewContext.h"
#include "Components/OpenDriveGizmoArrowComponent.h"
#include "Sources/OpenDriveFloatParameterSource.h"
#include "Sources/OpenDriveGizmoAxisSource.h"

UInteractiveGizmo* UOpenDriveGizmoBuilder::BuildGizmo(const FToolBuilderState& SceneState) const
{
	UOpenDriveGizmo* Gizmo = NewObject<UOpenDriveGizmo>(SceneState.GizmoManager);
	Gizmo->World = SceneState.World;
	UGizmoViewContext* GizmoViewContext = SceneState.ToolManager->GetContextObjectStore()->FindContext<UGizmoViewContext>();
	check(GizmoViewContext && GizmoViewContext->IsValidLowLevel());
	Gizmo->GizmoViewContext = GizmoViewContext;
	Gizmo->LaneChangeGizmoBuilderIdentifier = LaneChangeGizmoBuilderIdentifier;
	Gizmo->MoveAlongLaneGizmoBuilderIdentifier = MoveAlongLaneGizmoBuilderIdentifier;
	Gizmo->GizmoActorBuilder = MakeShared<FOpenDriveGizmoActorFactory>(GizmoViewContext);
	return Gizmo;
}

void UOpenDriveGizmo::Setup()
{
	Super::Setup();
	OpenDrivePosition = NewObject<UOpenDrivePosition>();
	GizmoActor = GizmoActorBuilder->CreateNewGizmoActor(World);
}

void UOpenDriveGizmo::Shutdown()
{
	ClearActiveTarget();

	if (GizmoActor)
	{
		GizmoActor->Destroy();
		GizmoActor = nullptr;
	}
}

void UOpenDriveGizmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateSubGizmosAxesDirections(); 
}

void UOpenDriveGizmo::SetActiveTarget(UTransformProxy* Target, IToolContextTransactionProvider* TransactionProvider)
{
	if (ActiveTarget != nullptr)
	{
		ClearActiveTarget();
	}
	
	ActiveTarget = Target;
	SetGizmoLocation();

	TransformSubGizmoSharedState = MakeUnique<FGizmoSharedState>();
	
	if (UOpenDriveTranslationGizmo* MoveAlongSGizmo = Cast<UOpenDriveTranslationGizmo>(GetGizmoManager()->CreateGizmo(MoveAlongLaneGizmoBuilderIdentifier)))
	{
		MoveAlongSGizmo->Initialize(GizmoActor->MoveAlongS, Target, TransactionProvider, TransformSubGizmoSharedState.Get(), 0, EOpenDriveSourceType::TranslateOnS);
		TranslationSGizmo = MoveAlongSGizmo;
		ActiveGizmos.Add(MoveAlongSGizmo);
	}
	if (UOpenDriveTranslationGizmo* MoveAlongTGizmo = Cast<UOpenDriveTranslationGizmo>(GetGizmoManager()->CreateGizmo(MoveAlongLaneGizmoBuilderIdentifier)))
	{
		MoveAlongTGizmo->Initialize(GizmoActor->MoveAlongT, Target, TransactionProvider, TransformSubGizmoSharedState.Get(), 1, EOpenDriveSourceType::TranslateOnT);
		TranslationTGizmo = MoveAlongTGizmo;
		ActiveGizmos.Add(MoveAlongTGizmo);
	}
	if (UOpenDriveTranslationGizmo* CastChangeLaneGizmo = Cast<UOpenDriveTranslationGizmo>(GetGizmoManager()->CreateGizmo(MoveAlongLaneGizmoBuilderIdentifier)))
	{
		CastChangeLaneGizmo->Initialize(GizmoActor->ChangeLane, Target, TransactionProvider, TransformSubGizmoSharedState.Get(), 2, EOpenDriveSourceType::ChangeLane);
		ChangeLaneGizmo = CastChangeLaneGizmo;
		ActiveGizmos.Add(ChangeLaneGizmo);
	}
	if (UOpenDriveAlignToLaneGizmo* AlignToLaneGizmo = Cast<UOpenDriveAlignToLaneGizmo>(GetGizmoManager()->CreateGizmo(LaneChangeGizmoBuilderIdentifier)))
	{
		AlignToLaneGizmo->Initialize(GizmoActor->AlignWithLane);
		ActiveGizmos.Add(AlignToLaneGizmo);
	}

	AxisSSource = TransformSubGizmoSharedState->AxisSSource;
	AxisTSource = TransformSubGizmoSharedState->AxisTSource;
	AxisChangeLaneSource = TransformSubGizmoSharedState->AxisChangeLaneSource;
	StateTarget = TransformSubGizmoSharedState->StateTarget;
}

void UOpenDriveGizmo::ClearActiveTarget()
{
	for (UInteractiveGizmo* Gizmo : ActiveGizmos)
	{
		GetGizmoManager()->DestroyGizmo(Gizmo);
	}
	ActiveGizmos.SetNum(0);
	AxisSSource = nullptr;
	AxisTSource = nullptr;
	AxisChangeLaneSource = nullptr;
	StateTarget = nullptr;
	ActiveTarget = nullptr;
	TranslationSGizmo = nullptr;
	TranslationTGizmo = nullptr;
	ChangeLaneGizmo = nullptr;
}

void UOpenDriveGizmo::SetVisibility(const bool bVisible) const
{
	GizmoActor->SetActorHiddenInGame(bVisible == false);
#if WITH_EDITOR
	GizmoActor->SetIsTemporarilyHiddenInEditor(bVisible == false);
#endif
}

void UOpenDriveGizmo::AutoAlignWithLane(const bool bAutoAlignWithLane) const
{
	if (TranslationTGizmo != nullptr && TranslationSGizmo != nullptr && ChangeLaneGizmo != nullptr)
	{
		TranslationTGizmo->AutoAlignToLane(bAutoAlignWithLane);
		TranslationSGizmo->AutoAlignToLane(bAutoAlignWithLane);
		ChangeLaneGizmo->AutoAlignToLane(bAutoAlignWithLane);
	}
}

void UOpenDriveGizmo::SetOverrideHeight(bool bOverrideHeight) const
{
	if (TranslationTGizmo != nullptr && TranslationSGizmo != nullptr && ChangeLaneGizmo != nullptr)
	{
		TranslationTGizmo->OverrideHeight(bOverrideHeight);
		TranslationSGizmo->OverrideHeight(bOverrideHeight);
		ChangeLaneGizmo->OverrideHeight(bOverrideHeight);
	}
}

void UOpenDriveGizmo::SetGizmoLocation() const
{
	USceneComponent* GizmoComponent = GizmoActor->GetRootComponent();
	const FTransform TargetTransform = ActiveTarget->GetTransform();
	FTransform GizmoTransform = TargetTransform;
	GizmoTransform.SetScale3D(FVector::OneVector);
	GizmoComponent->SetWorldTransform(GizmoTransform);
}

void UOpenDriveGizmo::ResetGizmo(const FTransform& ActorTransform) const
{
	if (ActiveTarget != nullptr)
	{
		ActiveTarget->SetTransform(ActorTransform);
	}
	SetGizmoLocation();
}

void UOpenDriveGizmo::UpdateSubGizmosAxesDirections() const
{
	if (GizmoActor->MoveAlongT != nullptr && GizmoActor->ChangeLane != nullptr && GizmoActor->MoveAlongS != nullptr && ActiveTarget != nullptr)
	{
		const FTransform TargetTransform = ActiveTarget->GetTransform();
		const FVector TargetLocationNoZ(TargetTransform.GetLocation().X, TargetTransform.GetLocation().Y, 0);
		const FTransform SimplifiedTransform(TargetTransform.GetRotation(), TargetLocationNoZ, FVector::OneVector);
		
		if (UOpenDriveGizmoArrowComponent* CastComponent = Cast<UOpenDriveGizmoArrowComponent>(GizmoActor->MoveAlongS))
		{
			CastComponent->Direction = SimplifiedTransform.InverseTransformVector(AxisSSource->GetDirection());
			CastComponent->MarkRenderStateDirty();
		}
		if (UOpenDriveGizmoArrowComponent* CastComponent = Cast<UOpenDriveGizmoArrowComponent>(GizmoActor->MoveAlongT))
		{
			CastComponent->Direction = SimplifiedTransform.InverseTransformVector(AxisTSource->GetDirection());
			CastComponent->MarkRenderStateDirty();
		}
		if (UOpenDriveGizmoArrowComponent* CastComponent = Cast<UOpenDriveGizmoArrowComponent>(GizmoActor->ChangeLane))
		{
			CastComponent->Direction = SimplifiedTransform.InverseTransformVector(AxisChangeLaneSource->GetDirection());
			CastComponent->MarkRenderStateDirty();
		}
	}
}
