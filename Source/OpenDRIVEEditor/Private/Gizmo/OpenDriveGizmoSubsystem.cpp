#include "Gizmo/OpenDriveGizmoSubsystem.h"

void UOpenDriveGizmoSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	selectionChangedHandle = USelection::SelectObjectEvent.AddUObject(this, &UOpenDriveGizmoSubsystem::OnSelectionChanged);
}

void UOpenDriveGizmoSubsystem::Deinitialize()
{
	USelection::SelectObjectEvent.Remove(selectionChangedHandle);
}

void UOpenDriveGizmoSubsystem::AddGizmo(AActor* OnActor)
{
	if (!IsThereAGizmo(OnActor))
	{
		UOpenDriveTranslateComponent* translateComponent = NewObject<UOpenDriveTranslateComponent>(OnActor, TEXT("OdrTranslateComponent"));
		USceneComponent* rootComp = OnActor->GetRootComponent();
		if (rootComp != nullptr)
		{
			translateComponent->SetupAttachment(OnActor->GetRootComponent());
		}
		else
		{
			OnActor->SetRootComponent(translateComponent);
		}
		translateComponent->RegisterComponent();
	}
}

void UOpenDriveGizmoSubsystem::RemoveGizmo(AActor* OnActor)
{
	UOpenDriveTranslateComponent* component;
	if (IsThereAGizmo(OnActor, component))
	{
		OnActor->RemoveInstanceComponent(component);
	}
}

void UOpenDriveGizmoSubsystem::SetEnableGizmo()
{
	bEnabled = !bEnabled;
	UE_LOG(LogTemp, Log, TEXT("Gizmo : %s"), (bEnabled ? TEXT("True") : TEXT("False")) );

	if (currentActor != nullptr)
	{
		AddGizmo(currentActor);
	}
}

void UOpenDriveGizmoSubsystem::OnSelectionChanged(UObject* newObject)
{
	if (currentActor != nullptr)
	{
		RemoveGizmo(currentActor);
	}

	AActor* newActor = Cast<AActor>(newObject);
	if (newActor != nullptr)
	{
		AddGizmo(newActor);
		currentActor = newActor;
	}
}
