// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Exporters/Exporter.h"
#include "OpenDRIVEExporter.generated.h"

/**
 * 
 */
UCLASS()
class OPENDRIVEEDITOR_API UOpenDRIVEExporter : public UExporter
{
	GENERATED_BODY()
	
	UOpenDRIVEExporter(const FObjectInitializer& ObjectInitiliazer);

	virtual bool ExportText(const FExportObjectInnerContext* Context, UObject* Object, const TCHAR* Type, FOutputDevice& Ar, FFeedbackContext* Warn, uint32 PortFlags) override;
};
