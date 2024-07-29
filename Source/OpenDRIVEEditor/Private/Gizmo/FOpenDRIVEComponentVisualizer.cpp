#include "Gizmo/FOpenDRIVEComponentVisualizer.h"
#include "Gizmo/OpenDriveGizmoSubsystem.h"
#include "UnrealEd.h"

IMPLEMENT_HIT_PROXY(HTargetingVisProxy, HComponentVisProxy);

void FOpenDRIVEComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	UOpenDriveGizmoSubsystem* subsystem = GEditor->GetEditorSubsystem<UOpenDriveGizmoSubsystem>();
	if (subsystem)
	{
		if (subsystem->bEnabled)
		{
			const UOpenDriveTranslateComponent* targetComponent = Cast<UOpenDriveTranslateComponent>(Component);
			if (targetComponent != nullptr)
			{
				const FVector location = targetComponent->GetOwner()->GetActorLocation();
				_pointPosition = FVector(location.X, location.Y, location.Z + targetComponent->PivotPosition);

				PDI->SetHitProxy(new HTargetingVisProxy(Component));
				PDI->DrawPoint(_pointPosition, FLinearColor::Red, 20, SDPG_World);
				PDI->SetHitProxy(NULL);
			}
		}
	}
}

//have we handled the click or not ? 
bool FOpenDRIVEComponentVisualizer::VisProxyHandleClick(FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click)
{
	bool bEditing = false;

	if (VisProxy && VisProxy->Component.IsValid()) 
	{
		const UOpenDriveTranslateComponent* component = CastChecked<UOpenDriveTranslateComponent>(VisProxy->Component.Get());

		AActor* oldOpenDriveOwningActor = OpenDriveComponentPropertyPath.GetParentOwningActor();
		OpenDriveComponentPropertyPath = FComponentPropertyPath(component);
		AActor* newOpenDriveOwningActor = OpenDriveComponentPropertyPath.GetParentOwningActor();

		if (OpenDriveComponentPropertyPath.IsValid()) 
		{
			if (oldOpenDriveOwningActor != newOpenDriveOwningActor) 
			{
				//do stuff if new actor
			}

			//handles different click proxy
			if (VisProxy->IsA(HTargetingVisProxy::StaticGetType())) 
			{
				//HTargetingVisProxy* proxy = static_cast<HTargetingVisProxy*>(VisProxy);
				bEditing = true;
			}
		}
		else 
		{
			OpenDriveComponentPropertyPath.Reset();
		}
	}
	return bEditing;
}

bool FOpenDRIVEComponentVisualizer::GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocation) const
{
	UOpenDriveTranslateComponent* component = GetEditedOpenDriveTranslateComponent();

	if (component != nullptr) 
	{
		OutLocation = _pointPosition;
		return true;
	}

	return false;
}

bool FOpenDRIVEComponentVisualizer::GetCustomInputCoordinateSystem(const FEditorViewportClient* ViewportClient, FMatrix& OutMatrix) const
{
	if (ViewportClient->GetWidgetMode() == UE::Widget::WM_Translate)
	{
		UOpenDriveTranslateComponent* component = GetEditedOpenDriveTranslateComponent();

		if (component != nullptr)
		{
			UOpenDrivePosition* position = component->Position;

			if (position != nullptr) 
			{
				AActor* owner = component->GetOwner();
				FTransform ownerTransform = owner->GetTransform();

				position->SetTransform(ownerTransform);
				position->AlignWithLaneCenter();

				ownerTransform.SetRotation(position->GetTransform().GetRotation());

				owner->SetActorTransform(ownerTransform);

				FQuat rotationQuat = ownerTransform.GetRotation();
				OutMatrix = FRotationMatrix::Make(rotationQuat);

				return true;
			}
		}
	}
	return false;
}

UOpenDriveTranslateComponent* FOpenDRIVEComponentVisualizer::GetEditedOpenDriveTranslateComponent() const
{
	return Cast<UOpenDriveTranslateComponent>(OpenDriveComponentPropertyPath.GetComponent());
}
