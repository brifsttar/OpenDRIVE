#pragma once
#include "IDetailCustomization.h"

class AOpenDRIVEActor;

class FOpenDriveActorCustomDetails : public IDetailCustomization
{
public : 

	/* IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	/* End IDetailCustomization interface */

	static TSharedRef<IDetailCustomization> MakeInstance();
};