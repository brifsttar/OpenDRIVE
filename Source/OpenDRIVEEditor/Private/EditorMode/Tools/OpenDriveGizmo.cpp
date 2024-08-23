#include "OpenDriveGizmo.h"

#define LOCTEXT_NAMESPACE "OpenDriveGizmo"

UOpenDriveGizmo* UOpenDriveGizmo::CreateGizmo(UInteractiveGizmoManager* Manager, const FString& GizmoBuilderIdentifier, const FString& GizmoIdentifier)
{
	UInteractiveGizmo* Gizmo = Manager->CreateGizmo(GizmoBuilderIdentifier, *GizmoIdentifier);
	UOpenDriveGizmo* NewGizmo = Cast<UOpenDriveGizmo>(Gizmo);

	if (NewGizmo)
	{
		NewGizmo->bUseContextGizmoMode = true;
		NewGizmo->ActiveGizmoMode = EToolContextTransformGizmoMode::Translation;
		NewGizmo->bUseContextCoordinateSystem = true;
		NewGizmo->CurrentCoordinateSystem = EToolContextCoordinateSystem::Local;
		NewGizmo->bSnapToWorldGrid = true;
		NewGizmo->SetUpdateCoordSystemFunction(
			[](UPrimitiveComponent* Component, EToolContextCoordinateSystem CoordSystem)
			{ return EToolContextCoordinateSystem::Local; }
		);

		return NewGizmo;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create gizmo"));
		return nullptr;
	}
}

void UOpenDriveGizmo::SetActiveTarget(UTransformProxy* Target, IToolContextTransactionProvider* TransactionProvider)
{
	if (ActiveTarget != nullptr)
	{
		ClearActiveTarget();
	}

	ActiveTarget = Target;

	// move gizmo to target location
	USceneComponent* GizmoComponent = GizmoActor->GetRootComponent();

	FTransform TargetTransform = Target->GetTransform();
	FTransform GizmoTransform = TargetTransform;
	GizmoTransform.SetScale3D(FVector(1, 1, 1));
	GizmoComponent->SetWorldTransform(GizmoTransform);

	UGizmoScaledAndUnscaledTransformSources* TransformSource = UGizmoScaledAndUnscaledTransformSources::Construct(
		UGizmoTransformProxyTransformSource::Construct(ActiveTarget, this),
		GizmoComponent, this);

	// This state target emits an explicit FChange that moves the GizmoActor root component during undo/redo.
	// It also opens/closes the Transaction that saves/restores the target object locations.
	if (TransactionProvider == nullptr)
	{
		TransactionProvider = GetGizmoManager();
	}

	StateTarget = UGizmoTransformChangeStateTarget::Construct(GizmoComponent,
		LOCTEXT("UCombinedTransformGizmoTransaction", "Transform"), TransactionProvider, this);
	StateTarget->DependentChangeSources.Add(MakeUnique<FTransformProxyChangeSource>(Target));

	CameraAxisSource = NewObject<UGizmoConstantFrameAxisSource>(this);

	// root component provides local X/Y/Z axis, identified by AxisIndex
	AxisXSource = UGizmoComponentAxisSource::Construct(GizmoComponent, 0, true, this);
	AxisYSource = UGizmoComponentAxisSource::Construct(GizmoComponent, 1, true, this);
	AxisZSource = UGizmoComponentAxisSource::Construct(GizmoComponent, 2, true, this);

	if (GizmoActor->TranslateX != nullptr)
	{
		UInteractiveGizmo* NewGizmo = AddOpenDriveAxisTranslationGizmo(GizmoActor->TranslateX, GizmoComponent, AxisXSource, TransformSource, StateTarget, 0, OpenDriveAxisType::S);
		ActiveComponents.Add(GizmoActor->TranslateX);
		TranslationSubGizmos.Add(FSubGizmoInfo{ GizmoActor->TranslateX, NewGizmo });
	}
	if (GizmoActor->TranslateY != nullptr)
	{
		UInteractiveGizmo* NewGizmo = AddOpenDriveAxisTranslationGizmo(GizmoActor->TranslateY, GizmoComponent, AxisYSource, TransformSource, StateTarget, 0, OpenDriveAxisType::T);
		ActiveComponents.Add(GizmoActor->TranslateY);
		TranslationSubGizmos.Add(FSubGizmoInfo{ GizmoActor->TranslateY, NewGizmo });
	}

	OnSetActiveTarget.Broadcast(this, ActiveTarget);
}

UInteractiveGizmo* UOpenDriveGizmo::AddOpenDriveAxisTranslationGizmo(UPrimitiveComponent* AxisComponent, USceneComponent* RootComponent, IGizmoAxisSource* AxisSource, IGizmoTransformSource* TransformSource, IGizmoStateTarget* StateTargetIn, int AxisIndex, OpenDriveAxisType AxisType)
{
	// create axis-position gizmo, axis-position parameter will drive translation
	UAxisPositionGizmo* TranslateGizmo = Cast<UAxisPositionGizmo>(GetGizmoManager()->CreateGizmo(AxisPositionBuilderIdentifier));
	check(TranslateGizmo);

	// axis source provides the translation axis
	TranslateGizmo->AxisSource = Cast<UObject>(AxisSource);

	// parameter source maps axis-parameter-change to translation of TransformSource's transform
	UOpenDriveGizmoAxisTranslationParameterSource* ParamSource = UOpenDriveGizmoAxisTranslationParameterSource::Construct(AxisSource, TransformSource, this, AxisType);
	ParamSource->PositionConstraintFunction = [this](const FVector& Pos, FVector& Snapped) { return PositionSnapFunction(Pos, Snapped); };
	ParamSource->AxisDeltaConstraintFunction = [this, AxisIndex](double AxisDelta, double& SnappedAxisDelta) { return PositionAxisDeltaSnapFunction(AxisDelta, SnappedAxisDelta, AxisIndex); };
	TranslateGizmo->ParameterSource = ParamSource;

	// sub-component provides hit target
	UGizmoComponentHitTarget* HitTarget = UGizmoComponentHitTarget::Construct(AxisComponent, this);
	if (this->UpdateHoverFunction)
	{
		HitTarget->UpdateHoverFunction = [AxisComponent, this](bool bHovering) { this->UpdateHoverFunction(AxisComponent, bHovering); };
	}
	TranslateGizmo->HitTarget = HitTarget;

	TranslateGizmo->StateTarget = Cast<UObject>(StateTargetIn);

	TranslateGizmo->ShouldUseCustomDestinationFunc = [this]() { return ShouldAlignDestination(); };
	TranslateGizmo->CustomDestinationFunc =
		[this](const UAxisPositionGizmo::FCustomDestinationParams& Params, FVector& OutputPoint) {
		return DestinationAlignmentRayCaster(*Params.WorldRay, OutputPoint);
		};

	ActiveGizmos.Add(TranslateGizmo);
	return TranslateGizmo;
}

UInteractiveGizmo* UOpenDriveGizmoBuilder::BuildGizmo(const FToolBuilderState& SceneState) const
{
	UOpenDriveGizmo* NewGizmo = NewObject<UOpenDriveGizmo>(SceneState.GizmoManager);
	NewGizmo->SetWorld(SceneState.World);

	UGizmoViewContext* GizmoViewContext = SceneState.ToolManager->GetContextObjectStore()->FindContext<UGizmoViewContext>();
	check(GizmoViewContext && GizmoViewContext->IsValidLowLevel());

	// use default gizmo actor if client has not given us a new builder
	NewGizmo->SetGizmoActorBuilder(GizmoActorBuilder ? GizmoActorBuilder : MakeShared<FCombinedTransformGizmoActorFactory>(GizmoViewContext));

	NewGizmo->SetSubGizmoBuilderIdentifiers(AxisPositionBuilderIdentifier, PlanePositionBuilderIdentifier, AxisAngleBuilderIdentifier);

	// override default hover function if proposed
	if (UpdateHoverFunction)
	{
		NewGizmo->SetUpdateHoverFunction(UpdateHoverFunction);
	}

	if (UpdateCoordSystemFunction)
	{
		NewGizmo->SetUpdateCoordSystemFunction(UpdateCoordSystemFunction);
	}

	return NewGizmo;
}

void UOpenDriveGizmoBuilder::RegisterGizmoBuilder(UInteractiveGizmoManager* Manager, UGizmoViewContext* GizmoViewContext, FString GizmoAxisBuilderIdentifier, FString GizmoBuilderIdentifier)
{
	UOpenDriveGizmoBuilder* CustomGizmoBuilder = NewObject<UOpenDriveGizmoBuilder>();
	CustomGizmoBuilder->AxisPositionBuilderIdentifier = GizmoAxisBuilderIdentifier;
	CustomGizmoBuilder->PlanePositionBuilderIdentifier = UInteractiveGizmoManager::DefaultPlanePositionBuilderIdentifier;
	CustomGizmoBuilder->AxisAngleBuilderIdentifier = UInteractiveGizmoManager::DefaultAxisAngleBuilderIdentifier;
	TSharedPtr<FCombinedTransformGizmoActorFactory> GizmoActorBuilder = MakeShared<FCombinedTransformGizmoActorFactory>(GizmoViewContext);
	GizmoActorBuilder->EnableElements = ETransformGizmoSubElements::TranslateAxisX | ETransformGizmoSubElements::TranslateAxisY;
	CustomGizmoBuilder->GizmoActorBuilder = GizmoActorBuilder;
	Manager->RegisterGizmoType(*GizmoBuilderIdentifier, CustomGizmoBuilder);
}

#undef LOCTEXT_NAMESPACE
