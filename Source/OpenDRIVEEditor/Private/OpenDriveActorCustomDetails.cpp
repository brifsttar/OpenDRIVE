#include "OpenDriveActorCustomDetails.h"
#include "OpenDRIVEActor.h"
#include "PropertyEditing.h"

void FOpenDriveActorCustomDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects; 

	DetailBuilder.GetObjectsBeingCustomized(Objects);

	if (Objects.Num() != 1)
	{
		return;
	}

	AOpenDRIVEActor* actor = Cast<AOpenDRIVEActor>(Objects[0].Get());

	TArray<FName> categoriesName;
	DetailBuilder.GetCategoryNames(categoriesName);

	for (auto name : categoriesName)
	{
		if (name != "OpenDRIVE")
		{
			DetailBuilder.HideCategory(name);
		}
	}
}

TSharedRef<IDetailCustomization> FOpenDriveActorCustomDetails::MakeInstance()
{
	return MakeShareable(new FOpenDriveActorCustomDetails);
}
