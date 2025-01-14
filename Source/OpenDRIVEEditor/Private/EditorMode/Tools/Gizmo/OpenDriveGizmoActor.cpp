#include "OpenDriveGizmoActor.h"
#include "Components/OpenDriveGizmoArrowComponent.h"
#include "Components/SphereComponent.h"

AOpenDriveGizmoActor::AOpenDriveGizmoActor()
{
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(FName("Center"));
	RootComponent = SphereComponent;
	SphereComponent->InitSphereRadius(1.0f);
	SphereComponent->SetVisibility(false);
	SphereComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

AOpenDriveGizmoActor* FOpenDriveGizmoActorFactory::CreateNewGizmoActor(UWorld* World) const
{
	AOpenDriveGizmoActor* NewActor = World->SpawnActor<AOpenDriveGizmoActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	
	auto MakeAxisCompFunc = [&](const FLinearColor& AxisColor, const FVector& Axis, float Length, float Gap, float GizmoThickness)
	{
		UOpenDriveGizmoArrowComponent* Component = UOpenDriveGizmoArrowComponent::AddOpenDriveArrowComponent(World, NewActor, GizmoViewContext, AxisColor, Axis, Length);
		Component->Gap = Gap;
		Component->Thickness = GizmoThickness;
		Component->NotifyExternalPropertyUpdates();
		return Component;
	};
	
	NewActor->MoveAlongS = MakeAxisCompFunc(FLinearColor::Yellow, FVector(1,0,0), 60.0f, 20.0f, 5.0f);
	NewActor->MoveAlongT = MakeAxisCompFunc(FLinearColor::Red, FVector(0,1,0), 40.0f, 20.0f, 5.0f);
	NewActor->ChangeLane = MakeAxisCompFunc(FLinearColor::Green, FVector(0,1,0), 20.0f, 70.0f, 5.0f);
	NewActor->AlignWithLane =  MakeAxisCompFunc(FLinearColor::Blue, FVector(0,0,1), 10.0f, 0.0f, 10.0f);
	return NewActor;
}
