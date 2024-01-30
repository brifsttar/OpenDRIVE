#include "OpenDRIVESubsystem.h"
#include "RoadManager.hpp"
#include "OpenDriveWorldSettings.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

void UOpenDRIVESubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (AWorldSettings* worldSettings = GetWorld()->GetWorldSettings()) {
		if (AOpenDriveWorldSettings* odrWorldSettings = Cast<AOpenDriveWorldSettings>(worldSettings)) {
			if (IsValid(odrWorldSettings->OpenDriveAsset)) {
				LoadOpenDrive(odrWorldSettings->OpenDriveAsset);
			}
		}
	}
}

void UOpenDRIVESubsystem::LoadOpenDrive(UOpenDriveAsset* OpenDriveAsset)
{
	if (!OpenDriveAsset) return;

	if (!roadmanager::Position::GetOpenDrive()->LoadOpenDriveContent(TCHAR_TO_UTF8(*(OpenDriveAsset->XodrContent)))) {
		UE_LOG(OpenDriveLog, Error, TEXT("Failed to load OpenDRIVE asset %s"), *(OpenDriveAsset->GetFName().ToString()));
		return;
	}
}


