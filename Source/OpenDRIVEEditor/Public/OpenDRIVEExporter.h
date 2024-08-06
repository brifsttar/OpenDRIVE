#pragma once

#include "CoreMinimal.h"
#include "Exporters/Exporter.h"
#include "OpenDRIVEExporter.generated.h"

/**
 * Overrides default export command for .xodr files export support
 */
UCLASS()
class OPENDRIVEEDITOR_API UOpenDRIVEExporter : public UExporter
{
	GENERATED_BODY()
	
	UOpenDRIVEExporter(const FObjectInitializer& ObjectInitiliazer);

	/* UExporter interface */
	virtual bool ExportText(const FExportObjectInnerContext* Context, UObject* Object, const TCHAR* Type, FOutputDevice& Ar, FFeedbackContext* Warn, uint32 PortFlags) override;
};
