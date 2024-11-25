#include "OpenDriveGizmoSubsystem.h"
#include "EditorModeManager.h"
#include "Selection.h"

void UOpenDriveGizmoSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	OpenDrivePosition = NewObject<UOpenDrivePosition>();

	OnActorSelectedHandle = USelection::SelectObjectEvent.AddUObject(this, &UOpenDriveGizmoSubsystem::OnActorSelected);
	
	check(!bInitialized);
	bInitialized = true;
}

void UOpenDriveGizmoSubsystem::Deinitialize()
{
	USelection::SelectObjectEvent.Remove(OnActorSelectedHandle);
	CurrentTargetActor = nullptr;
	check(bInitialized);
	bInitialized = false;
}

ETickableTickType UOpenDriveGizmoSubsystem::GetTickableTickType() const
{
	return IsTemplate() ? ETickableTickType::Never : FTickableEditorObject::GetTickableTickType();
}

bool UOpenDriveGizmoSubsystem::IsAllowedToTick() const
{
	return !IsTemplate() && bInitialized;
}

void UOpenDriveGizmoSubsystem::Tick(float DeltaTime)
{
	if (IsValid(CurrentTargetActor))
	{
		ECoordSystem CurrentCoordSystem =  GLevelEditorModeTools().GetCoordSystem();
		UE::Widget::EWidgetMode WidgetMode = GLevelEditorModeTools().GetWidgetMode();

		if (WidgetMode == UE::Widget::WM_Translate && CurrentCoordSystem == COORD_Local && bCoordModeActive)
		{
			OpenDrivePosition->SetTransform(CurrentTargetActor->GetTransform());
			OpenDrivePosition->AlignWithLaneCenter();
			CurrentTargetActor->SetActorRotation(OpenDrivePosition->GetTransform().GetRotation());
		}
	}
}

void UOpenDriveGizmoSubsystem::ToggleOpenDriveGizmoMode(bool bNewActive)
{
	bCoordModeActive = !bCoordModeActive;
	ECoordSystem NewCoordSystem = bCoordModeActive ? COORD_Local : COORD_World;
	GLevelEditorModeTools().SetCoordSystem(NewCoordSystem);
}

void UOpenDriveGizmoSubsystem::OnActorSelected(UObject* selectedActor)
{
	CurrentTargetActor = Cast<AActor>(selectedActor);
}
