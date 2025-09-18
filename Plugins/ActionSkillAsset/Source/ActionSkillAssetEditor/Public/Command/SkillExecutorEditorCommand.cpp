#include "SkillExecutorEditorCommand.h"
#define LOCTEXT_NAMESPACE "StateTreeEditor"
FSkillExecutorEditorCommand::FSkillExecutorEditorCommand()
	:TCommands(
		TEXT("SkillTreeEditor"), // Context name for fast lookup
		LOCTEXT("SkillTreeEditor", "SkillTree Editor"), // Localized context name for displaying
		NAME_None,
		TEXT("SkillTreeEditorStyle"))
{
}

void FSkillExecutorEditorCommand::RegisterCommands()
{
	UI_COMMAND(AddSiblingState, "Add Sibling State", "Add a Sibling Config", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddChildState, "Add Child State", "Add a Child Config", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CutStates, "Cut", "Cut Selected Config", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::X));
	UI_COMMAND(CopyStates, "Copy", "Copy Selected Config", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::C));
	UI_COMMAND(PasteStatesAsSiblings, "Paste As Siblings", "Paste Config as Siblings", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::V));
	UI_COMMAND(PasteStatesAsChildren, "Paste As Children", "Paste Config as Children", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control | EModifierKey::Shift, EKeys::V));
	UI_COMMAND(DuplicateStates, "Duplicate", "Duplicate Selected Config", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::D));
	UI_COMMAND(DeleteStates, "Delete", "Delete Selected Config", EUserInterfaceActionType::Button, FInputChord(EKeys::Delete));
	UI_COMMAND(RenameState, "Rename", "Rename Selected Config", EUserInterfaceActionType::Button, FInputChord(EKeys::F2));
}
#undef LOCTEXT_NAMESPACE