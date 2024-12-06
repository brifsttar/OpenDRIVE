#pragma once
#include "CoreMinimal.h"
#include "OpenDrivePosition.h"
#include "BaseGizmos/ParameterSourcesFloat.h"
#include "OpenDriveFloatParameterSource.generated.h"

UENUM()
enum class EOpenDriveSourceType : uint8
{
	TranslateOnS,
	TranslateOnT,
	ChangeLane
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

	UPROPERTY()
	float Parameter = 0.0f;

	UPROPERTY()
	EOpenDriveSourceType SourceType = EOpenDriveSourceType::TranslateOnS;

	UPROPERTY()
	int LaneId;

	UPROPERTY()
	int RoadId;

	UPROPERTY()
	float InitialT;

	UPROPERTY()
	float InitialS;

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
		UObject* Outer = (UObject*)GetTransientPackage())
	{
		UOpenDriveFloatParameterSource* NewSource = NewObject<UOpenDriveFloatParameterSource>(Outer);
		NewSource->AxisSource = Cast<UObject>(AxisSourceIn);
		NewSource->TransformSource = Cast<UObject>(TransformSourceIn);
		NewSource->OpenDrivePosition = NewObject<UOpenDrivePosition>(NewSource);
		return NewSource;
	}
};
