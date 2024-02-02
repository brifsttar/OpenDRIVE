#include "OpenDRIVESubsystem.h"
#include "RoadManager.hpp"
#include "OpenDriveWorldSettings.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

void UOpenDRIVESubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	AOpenDriveWorldSettings* odrWorldSettings = Cast<AOpenDriveWorldSettings>(GetWorld()->GetWorldSettings());

	if (IsValid(odrWorldSettings)) {
		LoadOpenDrive(odrWorldSettings->OpenDriveAsset);

#if WITH_EDITOR
		if (GEditor && !odrWorldSettings->bRegisteredReimportCallback) {
			GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetReimport.AddUObject(odrWorldSettings, &AOpenDriveWorldSettings::OnObjectReimported);
			odrWorldSettings->bRegisteredReimportCallback = true;
		}
#endif
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
