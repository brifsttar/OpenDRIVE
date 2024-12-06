#include "OpenDriveFloatParameterSource.h"

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
	InitialT = OpenDrivePosition->GetT();
	InitialS = OpenDrivePosition->GetS();
	RoadId = OpenDrivePosition->GetRoadId();
	CurTranslationAxis = AxisSource->GetDirection();
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

	switch(SourceType)
	{
	case EOpenDriveSourceType::TranslateOnS :
		OpenDrivePosition->SetLaneById(LaneId);
		OpenDrivePosition->AlignWithLaneCenter(); 
		OpenDrivePosition->SetT(InitialT);
		break;

	case EOpenDriveSourceType::TranslateOnT :
		break;

	case EOpenDriveSourceType::ChangeLane :
		OpenDrivePosition->AlignWithLaneCenter(); 
		break;
	}

	const FTransform OpenDriveTransform = OpenDrivePosition->GetTransform();
	const FVector OpenDriveForwardVector = OpenDriveTransform.GetRotation().GetForwardVector();
	const FVector NewTransformForwardVector = NewTransform.GetRotation().GetForwardVector();

	const float DotProduct = FVector::DotProduct(NewTransformForwardVector, OpenDriveForwardVector);

	if (const float DotThreshold = FMath::Cos(45.0f); DotProduct < -DotThreshold)
	{
		const FQuat ModifiedRotation = OpenDriveTransform.GetRotation();
		const FQuat AdjustedRotation = FQuat(FVector::UpVector, PI) * ModifiedRotation;

		NewTransform.SetRotation(AdjustedRotation);
	}
	else
	{
		NewTransform.SetRotation(OpenDriveTransform.GetRotation());
	}

	const FVector Location = OpenDriveTransform.GetLocation();
	const FVector NewLocation = FVector(Location.X, Location.Y, NewTransform.GetLocation().Z);
	NewTransform.SetLocation(NewLocation);

	if (FVector::Distance(NewTransform.GetLocation(), TransformSource->GetTransform().GetLocation()) > 5.f) //if location difference between NewTransform and current modified Actor does exceed threshold, apply NewTransform
	{
		TransformSource->SetTransform(NewTransform);
		OnParameterChanged.Broadcast(this, LastChange);
	}
}

void UOpenDriveFloatParameterSource::EndModify()
{
	
}