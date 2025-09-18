#pragma once

class FSkillExecutorEditorCommand:public  TCommands<FSkillExecutorEditorCommand>
{
public:
	FSkillExecutorEditorCommand();
	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> AddSiblingState;
	TSharedPtr<FUICommandInfo> AddChildState;
	TSharedPtr<FUICommandInfo> RenameState;
	TSharedPtr<FUICommandInfo> CutStates;
	TSharedPtr<FUICommandInfo> CopyStates;
	TSharedPtr<FUICommandInfo> PasteStatesAsSiblings;
	TSharedPtr<FUICommandInfo> PasteStatesAsChildren;
	TSharedPtr<FUICommandInfo> DuplicateStates;
	TSharedPtr<FUICommandInfo> DeleteStates;
};
