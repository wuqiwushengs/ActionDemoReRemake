#pragma once
#include "ActionSkillAssetRuntime/Public/ActionAbilityResourceFold/SkillExecutorFold/SkillExecutorDescriptor.h"

class FSkillTreeViewModel;
class USkillExecutorConfig;
class SSkillExecutorTreeViewRow:public STableRow<TWeakObjectPtr<USkillExecutorConfig>>
{
public:
	SLATE_BEGIN_ARGS(SSkillExecutorTreeViewRow){}
	SLATE_END_ARGS()

	
	void Construct(const FArguments & InArgs,const TSharedRef<STableViewBase>& InOwnerTableView,TWeakObjectPtr<USkillExecutorConfig>InConfig,const TSharedPtr<SScrollBox>& ViewBox, TSharedPtr<FSkillTreeViewModel> InSkillTreeViewModel);
	void RequestRename() const;
private:
	FSlateColor GetTitleColor() const;
	FSlateColor GetActiveStateColor() const;
	FSlateColor GetSubTreeMarketColor() const;
	FText GetStateDesc() const;
	FText GetStateID() const;
	const FSlateBrush* GetSelectorIcon() const;
	FText GetSelectorTooltip() const;
	void HandleNodeLabelTextCommitted(const FText& Text, ETextCommit::Type Arg);
	FText GetStateTypeTooltip() const;
	bool IsStateSelected();
	bool IsRootState() const;
	
	FReply HandleDragDetected(const FGeometry& Geometry, const FPointerEvent& PointerEvent);
	TOptional<EItemDropZone> HandleCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, TWeakObjectPtr<USkillExecutorConfig> SkillExecutorDescriptor);
	FReply HandleAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, TWeakObjectPtr<USkillExecutorConfig> SkillExecutorDescriptor);
	TSharedPtr<FSkillTreeViewModel>SkillTreeViewModel;
	TWeakObjectPtr<USkillExecutorConfig> WeakConfig;
	TWeakObjectPtr<USkillExecutorEditorData> EditorData;
	TSharedPtr<SInlineEditableTextBlock> NameTextBlock;
};
