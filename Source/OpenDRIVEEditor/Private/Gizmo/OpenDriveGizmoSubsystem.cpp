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
		translateComponent->SetFlags(EObjectFlags::RF_Transient);
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

bool UOpenDriveGizmoSubsystem::IsThereAGizmo(AActor* onActor, UOpenDriveTranslateComponent*& outComp)
{
	outComp = onActor->GetComponentByClass<UOpenDriveTranslateComponent>();  
	return outComp != nullptr;
}

bool UOpenDriveGizmoSubsystem::IsThereAGizmo(AActor* onActor)
{
	UOpenDriveTranslateComponent* comp;  
	return IsThereAGizmo(onActor, comp);
}

void UOpenDriveGizmoSubsystem::SetEnableGizmo()
{
	if (currentActor != nullptr)
	{
		bEnabled = !bEnabled;
		UE_LOG(LogTemp, Display, TEXT("Odr Gizmo : %s"), bEnabled ? TEXT("TRUE") : TEXT("FALSE"));
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
	else
	{
		bEnabled = false;
	}
}
