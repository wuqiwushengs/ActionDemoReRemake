#pragma once

class FSkillTreeViewModel;
class USkillExecutorConfig;

class SSkillTreeView:public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSkillTreeView){}
	SLATE_END_ARGS()

	SSkillTreeView();
	virtual ~SSkillTreeView() override;


	
	void Construct(const FArguments& InArgs, TSharedRef<FSkillTreeViewModel> SkillTreeViewModel, const TSharedRef<FUICommandList>& InCommandList);
private:
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	void UpdateTree(bool bExpandPersistent=false);
	
	TSharedPtr<FUICommandList> CommandList;
	TSharedPtr<FSkillTreeViewModel> SkillTreeViewModel;
	TSharedPtr<STreeView<TWeakObjectPtr<USkillExecutorConfig>>> TreeView;
	TSharedPtr<SScrollBox> ViewBox;
	TArray<TWeakObjectPtr<USkillExecutorConfig>> Subtrees;
	USkillExecutorConfig * RequestRenameState;
	TSharedRef<ITableRow> HandleGenerateRow(TWeakObjectPtr<USkillExecutorConfig> InState, const TSharedRef<STableViewBase>& InOwnerTableView);
	void HandleGetChildren(TWeakObjectPtr<USkillExecutorConfig> InParent, TArray<TWeakObjectPtr<USkillExecutorConfig>>& OutChildren);
	void HandleTreeSelectionChanged(TWeakObjectPtr<USkillExecutorConfig> SkillExecutorConfig, ESelectInfo::Type Arg);
	void HandleTreeExpansionChanged(TWeakObjectPtr<USkillExecutorConfig> SkillExecutorConfig, bool bArg);
	FReply HandleAddStateButton();
	TSharedPtr<SWidget> HandleContextMenuOpening();
	USkillExecutorConfig* GetFirstSelectedState();
	//CommandBind
	void HandleAddSiblingState();
	void HandleAddChildState();
	bool HasSelection();
	void HandleCutSelectedStates();
	void HandleCopySelectedStates();
	void HandleDeleteStates();
	void HandlePasteStatesAsSiblings();
	bool CanPaste();
	void HandlePasteStatesAsChildren();
	void HandleDuplicateSelectedStates();
	void HandleRenameState();
	void BindCommands();
	bool bItemDirty;
	bool bUpdatingSelection;
	

	//ViewModelHandlers
	void HandleModelAssetChanged();
	void HandleModelStatesRemoved(const TSet<USkillExecutorConfig*>& SkillExecutorConfigs);
	void HandleModelStatesMoved(const TSet<USkillExecutorConfig*>& SkillExecutorConfigs, const TSet<USkillExecutorConfig*>& Set);
	void HandleModelStateAdded(USkillExecutorConfig* ParentSkillExecutorConfig, USkillExecutorConfig* NewSkillExecutorConfig1);
	void HandleModelStatesChanged(const TSet<USkillExecutorConfig*>& SkillExecutorConfigs, const FPropertyChangedEvent& PropertyChangedEvent);
	void HandleModelSelectionChanged(const TArray<TWeakObjectPtr<USkillExecutorConfig>>& SelectedObjects);
};
