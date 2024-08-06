#include "OpenDRIVEExporter.h"
#include "OpenDriveAsset.h"

UOpenDRIVEExporter::UOpenDRIVEExporter(const FObjectInitializer& ObjectInitiliazer) : Super(ObjectInitiliazer)
{
	SupportedClass = UOpenDriveAsset::StaticClass();
	bText = true; 
	PreferredFormatIndex = 0;
	FormatExtension.Add(TEXT("xodr"));
	FormatDescription.Add(TEXT("XODR File"));
}

bool UOpenDRIVEExporter::ExportText(const FExportObjectInnerContext* Context, UObject* Object, const TCHAR* Type, FOutputDevice& Ar, FFeedbackContext* Warn, uint32 PortFlags)
{
	UOpenDriveAsset* openDriveAsset = CastChecked<UOpenDriveAsset>(Object);
	FString odr = openDriveAsset->XodrContent;
	Ar.Log(odr);
	return true;
}
