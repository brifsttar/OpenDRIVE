#include "OpenDriveExporter.h"
#include "OpenDriveAsset.h"

UOpenDriveExporter::UOpenDriveExporter(const FObjectInitializer& ObjectInitiliazer) : Super(ObjectInitiliazer)
{
	SupportedClass = UOpenDriveAsset::StaticClass();
	bText = true; 
	PreferredFormatIndex = 0;
	FormatExtension.Add(TEXT("xodr"));
	FormatDescription.Add(TEXT("XODR File"));
}

bool UOpenDriveExporter::ExportText(const FExportObjectInnerContext* Context, UObject* Object, const TCHAR* Type, FOutputDevice& Ar, FFeedbackContext* Warn, uint32 PortFlags)
{
	const UOpenDriveAsset* OpenDriveAsset = CastChecked<UOpenDriveAsset>(Object);
	const FString Odr = OpenDriveAsset->XodrContent;
	Ar.Log(Odr);
	return true;
}
