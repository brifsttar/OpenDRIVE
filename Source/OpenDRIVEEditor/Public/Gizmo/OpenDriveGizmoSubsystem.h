#pragma once
#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Engine/Selection.h"
#include "Gizmo/UOpenDriveTranslateComponent.h"
#include "OpenDriveGizmoSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGizmoActorChanged, AActor*, Actor);

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

	bool IsThereAGizmo(AActor* onActor, UOpenDriveTranslateComponent*& outComp);

	bool IsThereAGizmo(AActor* onActor);

	void SetEnableGizmo();

	bool bEnabled = true;

	UPROPERTY(BlueprintAssignable)
	FGizmoActorChanged OnGizmoActorChanged;

private : 

	FDelegateHandle selectionChangedHandle;
	AActor* currentActor;

	void OnSelectionChanged(UObject* newObject);
};
