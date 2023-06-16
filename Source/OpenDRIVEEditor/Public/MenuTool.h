#pragma once 
#include "IOpenDriveModuleInterface.h"

class MenuTool : public IOpenDRIVEModuleListenerInterface, public TSharedFromThis<MenuTool>
{
public : 

	virtual ~MenuTool() {}

	virtual void OnStartupModule() override;
	virtual void OnShutdownModule() override;

	void MakeMenuEntry(FMenuBuilder& menuBuilder);
	void MakeSubMenu(FMenuBuilder& menuBuilder);

protected : 

    TSharedPtr<FUICommandList> CommandList;

    void MapCommands();

    // UI Command functions
    void MenuCommand1();
    void MenuCommand2();
    void MenuCommand3();

    FName TagToAdd;

    FReply AddTag();
    FText GetTagToAddText() const;
    void OnTagToAddTextCommited(const FText& InText, ETextCommit::Type CommitInfo);
};