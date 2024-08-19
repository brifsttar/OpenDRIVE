#include "OpenDriveGizmo.h"

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
		return NewGizmo;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create gizmo"));
		return nullptr;
	}
}

UInteractiveGizmo* UOpenDriveGizmo::AddAxisTranslationGizmo(UPrimitiveComponent* AxisComponent, USceneComponent* RootComponent, IGizmoAxisSource* AxisSource, IGizmoTransformSource* TransformSource, IGizmoStateTarget* StateTargetIn, int AxisIndex)
{
	// create axis-position gizmo, axis-position parameter will drive translation
	UAxisPositionGizmo* TranslateGizmo = Cast<UAxisPositionGizmo>(GetGizmoManager()->CreateGizmo(AxisPositionBuilderIdentifier));
	check(TranslateGizmo);

	// axis source provides the translation axis
	TranslateGizmo->AxisSource = Cast<UObject>(AxisSource);

	// parameter source maps axis-parameter-change to translation of TransformSource's transform
	UOpenDriveGizmoAxisTranslationParameterSource* ParamSource = UOpenDriveGizmoAxisTranslationParameterSource::Construct(AxisSource, TransformSource, this);
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
	GizmoActorBuilder->EnableElements = ETransformGizmoSubElements::TranslateAxisX;
	CustomGizmoBuilder->GizmoActorBuilder = GizmoActorBuilder;
	Manager->RegisterGizmoType(*GizmoBuilderIdentifier, CustomGizmoBuilder);
}
