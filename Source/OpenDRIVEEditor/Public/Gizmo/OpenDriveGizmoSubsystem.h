#pragma once
#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Engine/Selection.h"
#include "Gizmo/UOpenDriveTranslateComponent.h"
#include "OpenDriveGizmoSubsystem.generated.h"

UCLASS()
class UOpenDriveGizmoSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public : 

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	void AddGizmo(AActor* OnActor);

	void RemoveGizmo(AActor* OnActor);

	inline bool IsThereAGizmo(AActor* onActor, UOpenDriveTranslateComponent*& outComp) { outComp = onActor->GetComponentByClass<UOpenDriveTranslateComponent>();  return outComp != nullptr; };

	inline bool IsThereAGizmo(AActor* onActor) { UOpenDriveTranslateComponent* comp;  return IsThereAGizmo(onActor, comp); };

	void SetEnableGizmo();

	bool bEnabled = false;

private : 

	FDelegateHandle selectionChangedHandle;
	AActor* currentActor;

	void OnSelectionChanged(UObject* newObject);
};
