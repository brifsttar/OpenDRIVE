#include "OpenDriveFloatParameterSource.h"

#include "OpenDriveGizmoAxisSource.h"

float UOpenDriveFloatParameterSource::GetParameter() const
{
	return Parameter;
}

void UOpenDriveFloatParameterSource::BeginModify()
{
	check(AxisSource)

	LastChange = FGizmoFloatParameterChange(Parameter);

	InitialTransform = TransformSource->GetTransform();
	OpenDrivePosition = NewObject<UOpenDrivePosition>();
	OpenDrivePosition->SetTransform(InitialTransform);
	LaneId = OpenDrivePosition->GetLaneId();
	//InitialT = OpenDrivePosition->GetOffsetFromCurrentLaneCenter();
	InitialT = OpenDrivePosition->GetRealT();
	InitialS = OpenDrivePosition->GetS();
	RoadId = OpenDrivePosition->GetRoadId();
	CurTranslationAxis = AxisSource->GetDirection();

	if (SourceType == EOpenDriveSourceType::ChangeLane || SourceType == EOpenDriveSourceType::TranslateOnT)
	{
		if (UOpenDriveGizmoAxisSource* CastAxisSource = Cast<UOpenDriveGizmoAxisSource>(AxisSource.GetObject()))
		{
			CastAxisSource->SetUpdateDirection(true);
		}
	}
}

void UOpenDriveFloatParameterSource::SetParameter(float NewValue)
{
	Parameter = NewValue;
	LastChange.CurrentValue = NewValue;

	const double Delta = LastChange.GetChangeDelta();

	const FVector Translation = Delta * CurTranslationAxis;
	FTransform NewTransform = InitialTransform;
	NewTransform.AddToTranslation(Translation); 
	
	OpenDrivePosition->SetTransform(TransformSource->GetTransform());

	//if (OpenDrivePosition->OdrPosition().IsOffRoad()){return;} // Bug : always return true when on roads' edges
	
	OpenDrivePosition->SetTransform(NewTransform);
	
	if (OpenDrivePosition->OdrPosition().IsInJunction()){return;}
	
	if (OpenDrivePosition->GetRoadId() != RoadId){return;}

	float S = 0;
	float T = 0;
	switch(SourceType)
	{
	case EOpenDriveSourceType::TranslateOnS :
		S  = UuToMeters(OpenDrivePosition->GetS());
		OpenDrivePosition->SetTrackPosition(roadmanager::Position(RoadId, S , UuToMeters(InitialT)));
		break;

	case EOpenDriveSourceType::TranslateOnT :
		T  = UuToMeters(OpenDrivePosition->GetRealT());
		OpenDrivePosition->SetTrackPosition(roadmanager::Position(RoadId, UuToMeters(InitialS), T));
		break;

	case EOpenDriveSourceType::ChangeLane :
		OpenDrivePosition->AlignWithLaneCenter(); 
		break;
	}

	const FTransform OpenDriveTransform = OpenDrivePosition->GetTransform();
	
	if (bAlignToLane)
	{
		const FVector OpenDriveForwardVector = OpenDriveTransform.GetRotation().GetForwardVector();
		const FVector NewTransformForwardVector = NewTransform.GetRotation().GetForwardVector();
		const float DotProduct = FVector::DotProduct(NewTransformForwardVector, OpenDriveForwardVector);
		
		if (const float DotThreshold = FMath::Cos(45.0f); DotProduct < -DotThreshold) // Avoid 180° yaw flip (if Actor is moving backward on a lane)
		{
			const FQuat ModifiedRotation = OpenDriveTransform.GetRotation();
			const FQuat AdjustedRotation = FQuat(FVector::UpVector, PI) * ModifiedRotation;
			NewTransform.SetRotation(AdjustedRotation);
		}
		else
		{
			NewTransform.SetRotation(OpenDriveTransform.GetRotation());
		}
	}
	
	NewTransform.SetLocation(OpenDriveTransform.GetLocation());

	if (FVector::Distance(NewTransform.GetLocation(), TransformSource->GetTransform().GetLocation()) > 1.f) //if location difference between NewTransform and current modified Actor does exceed threshold, apply NewTransform
	{
		TransformSource->SetTransform(NewTransform);
		OnParameterChanged.Broadcast(this, LastChange);
	}
}

void UOpenDriveFloatParameterSource::EndModify()
{
	if (SourceType == EOpenDriveSourceType::ChangeLane || SourceType == EOpenDriveSourceType::TranslateOnT)
	{
		if (UOpenDriveGizmoAxisSource* CastAxisSource = Cast<UOpenDriveGizmoAxisSource>(AxisSource.GetObject()))
		{
			CastAxisSource->SetUpdateDirection(false);
		}
	}
}