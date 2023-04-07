#include "OpenDriveReader.h"
#include "OpenDriveComponent.h"
#include "Components/BillboardComponent.h"
#include "EngineUtils.h"

AOpenDriveReader::AOpenDriveReader() {
	PrimaryActorTick.bCanEverTick = false;
	bRunConstructionScriptOnDrag = true;

	struct FConstructorStatics {
		ConstructorHelpers::FObjectFinder<UTexture2D> Texture0;
		FConstructorStatics()
			: Texture0(TEXT("Texture2D'/Engine/EditorResources/Waypoint'")) {
		}
	};
	static FConstructorStatics ConstructorStatics;
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	SpriteTexture = ConstructorStatics.Texture0.Object;
	BillboardComponent->Sprite = SpriteTexture;
	RootComponent = BillboardComponent;

	OpenDrive = CreateDefaultSubobject<UOpenDriveComponent>(TEXT("OpenDRIVE"));
	OpenDrive->SetupAttachment(RootComponent);
}

void AOpenDriveReader::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	RoadId     = OpenDrive->GetRoadId();
	LaneId     = OpenDrive->GetLaneId();
	JunctionId = OpenDrive->GetJunctionId();
	S          = OpenDrive->GetS();
	T          = OpenDrive->GetT();
	H          = OpenDrive->GetH();
}
