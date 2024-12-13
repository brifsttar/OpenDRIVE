#pragma once
#include "CoreMinimal.h"
#include "OpenDrivePosition.h"
#include "BaseGizmos/ParameterSourcesFloat.h"
#include "OpenDriveFloatParameterSource.generated.h"

UENUM()
enum class EOpenDriveSourceType : uint8
{
	TranslateOnS = 0,
	TranslateOnT = 1,
	ChangeLane = 2
};

UCLASS()
class OPENDRIVEEDITOR_API UOpenDriveFloatParameterSource : public UGizmoBaseFloatParameterSource
{
	GENERATED_BODY()

	public :
	virtual float GetParameter() const override;
	virtual void BeginModify() override;
	virtual void SetParameter(float NewValue) override;
	virtual void EndModify() override;

	void AutoAlignToLane(const bool bShouldAlign) { bAlignToLane = bShouldAlign; }
	void OverrideHeight(const bool bNewOverrideHeight) { bOverrideHeight = bNewOverrideHeight; }

	UPROPERTY()
	float Parameter = 0.0f;

	UPROPERTY()
	EOpenDriveSourceType SourceType = EOpenDriveSourceType::TranslateOnS;
	
	UPROPERTY()
	int RoadId;

	UPROPERTY()
	float InitialT;

	UPROPERTY()
	float InitialS;

	UPROPERTY()
	bool bAlignToLane;

	UPROPERTY()
	bool bOverrideHeight;

	UPROPERTY()
	FGizmoFloatParameterChange LastChange;

	UPROPERTY()
	FVector CurTranslationAxis;

	UPROPERTY()
	FTransform InitialTransform;

	UPROPERTY()
	TScriptInterface<IGizmoAxisSource> AxisSource;

	UPROPERTY()
	TScriptInterface<IGizmoTransformSource> TransformSource;

	UPROPERTY()
	TObjectPtr<UOpenDrivePosition> OpenDrivePosition;
	
	static UOpenDriveFloatParameterSource* Construct(
		IGizmoAxisSource* AxisSourceIn,
		IGizmoTransformSource* TransformSourceIn,
		UObject* Outer = GetTransientPackage())
	{
		UOpenDriveFloatParameterSource* NewSource = NewObject<UOpenDriveFloatParameterSource>(Outer);
		NewSource->AxisSource = Cast<UObject>(AxisSourceIn);
		NewSource->TransformSource = Cast<UObject>(TransformSourceIn);
		NewSource->OpenDrivePosition = NewObject<UOpenDrivePosition>(NewSource);
		return NewSource;
	}
};
