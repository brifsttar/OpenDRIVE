#include "OpenDriveFloatParameterSource.h"
#include "OpenDriveGizmoAxisSource.h"
#include "OpenDriveUtils.h"

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
	
	InitialT = OpenDrivePosition->GetRealT();
	InitialS = OpenDrivePosition->GetS();
	RoadId = OpenDrivePosition->GetRoadId();
	LaneId = OpenDrivePosition->GetLaneId();
	
	CurTranslationAxis = AxisSource->GetDirection();

	if (SourceType == EOpenDriveSourceType::ChangeLane || SourceType == EOpenDriveSourceType::TranslateOnT)
	{
		if (UOpenDriveGizmoAxisSource* CastAxisSource = Cast<UOpenDriveGizmoAxisSource>(AxisSource.GetObject()))
		{
			CastAxisSource->SetUpdateDirection(true); // if the axis we're dragging is updated, the case where we change lane could make its direction go in the opposite way, so it's frozen while interacting 
		}
	}
}

void UOpenDriveFloatParameterSource::SetParameter(float NewValue)
{
	Parameter = NewValue;
	LastChange.CurrentValue = NewValue;
	const double Delta = CalculateDelta();
	
	switch(SourceType)
	{
	case EOpenDriveSourceType::TranslateOnS :
		OpenDrivePosition->SetS(InitialS + Delta); 
		break;

	case EOpenDriveSourceType::TranslateOnT :
		OpenDrivePosition->SetRealT(InitialT - Delta);
		break;

	case EOpenDriveSourceType::ChangeLane :
		OpenDrivePosition->SetRealT(InitialT - Delta);
		OpenDrivePosition->AlignWithLaneCenter(); 
		break;
	}
	
	if (
		OpenDrivePosition->OdrPosition().IsInJunction() ||
		OpenDrivePosition->GetRoadId() != RoadId ||
		OpenDrivePosition->OdrPosition().IsOffRoad()
	) {
		return;
	}
	
	FTransform OpenDriveTransform = OpenDrivePosition->GetTransform();
	
	const EAlignmentMethod AlignmentMethod = bAlignToLane ? AlignButKeepDirection : NoAlignment;
	if (const FTransform FinalTransform = FOpenDriveUtils::OdrToUE(TransformSource->GetTransform(), OpenDriveTransform, AlignmentMethod, bOverrideHeight);
		FVector::Distance(FinalTransform.GetLocation(), TransformSource->GetTransform().GetLocation()) > 1.f) // todo: add a customizable snap value ? 
	{
		TransformSource->SetTransform(FinalTransform);
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

	OpenDrivePosition = nullptr;
}
