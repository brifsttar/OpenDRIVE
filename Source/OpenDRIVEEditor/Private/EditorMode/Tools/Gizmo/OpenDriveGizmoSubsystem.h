#pragma once
#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "OpenDrivePosition.h"
#include "OpenDriveGizmoSubsystem.generated.h"

UCLASS()
class UOpenDriveGizmoSubsystem : public UEditorSubsystem, public FTickableEditorObject
{
	GENERATED_BODY()

public :

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	// FTickableGameObject implementation Begin
	virtual UWorld* GetTickableGameObjectWorld() const { return GetWorld(); }
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsAllowedToTick() const override final;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override PURE_VIRTUAL(UOpenDriveGizmoSubsystem::GetStatId, return TStatId(););
	// FTickableGameObject implementation End

	void ToggleOpenDriveGizmoMode(bool bNewActive);

	bool IsInitialized() const {return bInitialized;}
	
protected :

	bool bCoordModeActive = false;

	UPROPERTY()
	UOpenDrivePosition* OpenDrivePosition;

	UPROPERTY()
	AActor* CurrentTargetActor;

	FDelegateHandle OnActorSelectedHandle;

	void OnActorSelected(UObject* selectedActor);
	
private :

	bool bInitialized = false;
};
