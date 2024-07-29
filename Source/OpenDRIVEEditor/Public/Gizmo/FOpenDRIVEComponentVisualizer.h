#pragma once 
#include "ComponentVisualizer.h"
#include "IOpenDriveModuleInterface.h"
#include "UOpenDriveTranslateComponent.h"
#include "UnrealEd.h"

/** Base struct for clickable targeting editing proxies */
struct HTargetingVisProxy : public HComponentVisProxy
{
	DECLARE_HIT_PROXY();

	HTargetingVisProxy(const UActorComponent* InComponent) 
	: HComponentVisProxy(InComponent, HPP_Wireframe)
	{}
};

class FOpenDRIVEComponentVisualizer : public FComponentVisualizer
{
public : 

	// FComponentVisualizer
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual bool VisProxyHandleClick(FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click);
	virtual bool GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocation) const override;
	//virtual bool HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale) override;
	virtual bool GetCustomInputCoordinateSystem(const FEditorViewportClient* ViewportClient, FMatrix& OutMatrix) const override;
	//virtual void EndEditing() override;

	UOpenDriveTranslateComponent* GetEditedOpenDriveTranslateComponent() const;

protected : 

	FComponentPropertyPath OpenDriveComponentPropertyPath;

private : 

	FVector _pointPosition;
};

class FOpenDriveCVModuleListener : public IOpenDRIVEModuleListenerInterface
{
public:

	virtual void OnStartupModule() override
	{
		if (GUnrealEd != nullptr)
		{
			TSharedPtr<FOpenDRIVEComponentVisualizer> visualizer = MakeShareable(new FOpenDRIVEComponentVisualizer);

			if (visualizer.IsValid())
			{
				GUnrealEd->RegisterComponentVisualizer(UOpenDriveTranslateComponent::StaticClass()->GetFName(), visualizer);
				visualizer->OnRegister();
			}
		}
	}

	virtual void OnShutdownModule() override
	{
		if (GUnrealEd != nullptr)
		{
			GUnrealEd->UnregisterComponentVisualizer(UOpenDriveTranslateComponent::StaticClass()->GetFName());
		}
	}
};
