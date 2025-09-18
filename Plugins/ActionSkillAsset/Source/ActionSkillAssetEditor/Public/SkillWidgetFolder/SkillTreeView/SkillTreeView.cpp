#include "SkillTreeView.h"
#include "SPositiveActionButton.h"
#include "Command/SkillExecutorEditorCommand.h"
#include "SkillExecutorTreeViewModel/FSkillTreeViewModel.h"
#include "SkillWidgetFolder/SkillExecutorTreeViewRow.h"

#define LOCTEXT_NAMESPACE "SkillTreeView"
SSkillTreeView::SSkillTreeView()
	:RequestRenameState(nullptr),bItemDirty(false),bUpdatingSelection(false)
{
}

SSkillTreeView::~SSkillTreeView()
{
	if (SkillTreeViewModel)
	{
		SkillTreeViewModel->GetOnAssetChanged().RemoveAll(this);
		SkillTreeViewModel->GetOnStatesRemoved().RemoveAll(this);
		SkillTreeViewModel->GetOnStateAdded().RemoveAll(this);
		SkillTreeViewModel->GetOnStatesChanged().RemoveAll(this);
		SkillTreeViewModel->GetOnStatesMoved().RemoveAll(this);
		SkillTreeViewModel->GetOnStatesRemoved().RemoveAll(this);
	}
	
}



void SSkillTreeView::Construct(const FArguments& InArgs, TSharedRef<FSkillTreeViewModel> InSkillTreeViewModel,
                               const TSharedRef<FUICommandList>& InCommandList)
{
	SkillTreeViewModel=InSkillTreeViewModel;
	SkillTreeViewModel->GetOnAssetChanged().AddSP(this, &SSkillTreeView::HandleModelAssetChanged);
	SkillTreeViewModel->GetOnStatesRemoved().AddSP(this, &SSkillTreeView::HandleModelStatesRemoved);
	SkillTreeViewModel->GetOnStatesMoved().AddSP(this, &SSkillTreeView::HandleModelStatesMoved);
	SkillTreeViewModel->GetOnStateAdded().AddSP(this, &SSkillTreeView::HandleModelStateAdded);
	SkillTreeViewModel->GetOnStatesChanged().AddSP(this, &SSkillTreeView::HandleModelStatesChanged);
	SkillTreeViewModel->GetOnSelectionChanged().AddSP(this, &SSkillTreeView::HandleModelSelectionChanged);

	TSharedRef<SScrollBar> HorizontalScrollBar = SNew(SScrollBar)
	.Orientation(Orient_Horizontal)
	.Thickness(FVector2D(12.0f, 12.0f));

	TSharedRef<SScrollBar> VerticalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Vertical)
		.Thickness(FVector2D(12.0f, 12.0f));
	SkillTreeViewModel->GetSubTrees(Subtrees);
	TreeView = SNew(STreeView<TWeakObjectPtr<USkillExecutorConfig>>)
		.OnGenerateRow(this, &SSkillTreeView::HandleGenerateRow)
		.OnGetChildren(this, &SSkillTreeView::HandleGetChildren)
		.TreeItemsSource(&Subtrees)
		.ItemHeight(32)
		.OnSelectionChanged(this, &SSkillTreeView::HandleTreeSelectionChanged)
		.OnExpansionChanged(this, &SSkillTreeView::HandleTreeExpansionChanged)
		.OnContextMenuOpening(this, &SSkillTreeView::HandleContextMenuOpening)
		.AllowOverscroll(EAllowOverscroll::No)
		//虽然在这里设置了，但是并不会自己嵌进去，需要自行放置在某个位置，在下面ui构造中可以看到
		.ExternalScrollbar(VerticalScrollBar);
	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.Padding(2.0f)
			[
				SNew(SHorizontalBox)
				// New State
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.Padding(4.0f, 2.0f)
				.AutoWidth()
				[
					SNew(SPositiveActionButton)
					.ToolTipText(LOCTEXT("AddStateToolTip", "Add New RootState"))
					.Icon(FAppStyle::Get().GetBrush("Icons.Plus")) 
					.Text(LOCTEXT("AddState", "Add RootState"))
					.OnClicked(this, &SSkillTreeView::HandleAddStateButton)
				]
			]
		]
		//添加TreeView
		+SVerticalBox::Slot()
		.Padding(0.0f, 6.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(0.0f)
			[
				SAssignNew(ViewBox, SScrollBox)
				.Orientation(Orient_Horizontal)
				.ExternalScrollbar(HorizontalScrollBar)
				+SScrollBox::Slot()
				.FillSize(1.0f)
				[
					TreeView.ToSharedRef()
				]
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				VerticalScrollBar
			]
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			HorizontalScrollBar
		]
	];

	UpdateTree(true);
	FSkillExecutorEditorCommand::Register();
	CommandList = InCommandList;
	//命令在这里绑定
	BindCommands();
}
void SSkillTreeView::BindCommands()
{
	const FSkillExecutorEditorCommand& Commands = FSkillExecutorEditorCommand::Get();
	if (!CommandList.IsValid())
	{
		UE_LOG(LogTemp,Warning,TEXT("Can't find Command"))
	}
	CommandList->MapAction(
		Commands.AddSiblingState,
		FExecuteAction::CreateSP(this, &SSkillTreeView::HandleAddSiblingState),
		FCanExecuteAction());

	CommandList->MapAction(
		Commands.AddChildState,
		FExecuteAction::CreateSP(this, &SSkillTreeView::HandleAddChildState),
		FCanExecuteAction::CreateSP(this, &SSkillTreeView::HasSelection));

	CommandList->MapAction(
		Commands.CutStates,
		FExecuteAction::CreateSP(this, &SSkillTreeView::HandleCutSelectedStates),
		FCanExecuteAction::CreateSP(this, &SSkillTreeView::HasSelection));

	CommandList->MapAction(
		Commands.CopyStates,
		FExecuteAction::CreateSP(this, &SSkillTreeView::HandleCopySelectedStates),
		FCanExecuteAction::CreateSP(this, &SSkillTreeView::HasSelection));

	CommandList->MapAction(
		Commands.DeleteStates,
		FExecuteAction::CreateSP(this, &SSkillTreeView::HandleDeleteStates),
		FCanExecuteAction::CreateSP(this, &SSkillTreeView::HasSelection));

	CommandList->MapAction(
		Commands.PasteStatesAsSiblings,
		FExecuteAction::CreateSP(this, &SSkillTreeView::HandlePasteStatesAsSiblings),
		FCanExecuteAction::CreateSP(this, &SSkillTreeView::CanPaste));

	CommandList->MapAction(
		Commands.PasteStatesAsChildren,
		FExecuteAction::CreateSP(this, &SSkillTreeView::HandlePasteStatesAsChildren),
		FCanExecuteAction::CreateSP(this, &SSkillTreeView::CanPaste));

	CommandList->MapAction(
		Commands.DuplicateStates,
		FExecuteAction::CreateSP(this, &SSkillTreeView::HandleDuplicateSelectedStates),
		FCanExecuteAction::CreateSP(this, &SSkillTreeView::HasSelection));

	CommandList->MapAction(
		Commands.RenameState,
		FExecuteAction::CreateSP(this, &SSkillTreeView::HandleRenameState),
		FCanExecuteAction::CreateSP(this, &SSkillTreeView::HasSelection));
	
}
FReply SSkillTreeView::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	//当按键按下的时候执行绑定的命令，在这里执行所有绑定的命令
	if(CommandList->ProcessCommandBindings(InKeyEvent))
	{
		return FReply::Handled();
	}
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

void SSkillTreeView::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (bItemDirty)
	{	//当有内容被更改时执行更新
		UpdateTree(/*bExpandPersistent*/true);
	}

	//将输入的名字进行更改
	if (RequestRenameState && !TreeView->IsPendingRefresh())
	{
		if (TSharedPtr<SSkillExecutorTreeViewRow> Row = StaticCastSharedPtr<SSkillExecutorTreeViewRow>(TreeView->WidgetFromItem(RequestRenameState)))
		{
			Row->RequestRename();
		}
		RequestRenameState = nullptr;
	}
}

void SSkillTreeView::UpdateTree(bool bExpandPersistent)
{
	if (!SkillTreeViewModel)
	{
		return;
	}

	TSet<TWeakObjectPtr<USkillExecutorConfig>> ExpandedStates;
	if (bExpandPersistent)
	{
		// Get expanded state from the tree data.
		SkillTreeViewModel->GetPersistentExpandedStates(ExpandedStates);
	}
	else
	{
		// Restore current expanded state.
		TreeView->GetExpandedItems(ExpandedStates);
	}

	// Remember selection
	TArray<TWeakObjectPtr<USkillExecutorConfig>> SelectedStates;
	SkillTreeViewModel->GetSelectedStates(SelectedStates);

	// Regenerate items
	SkillTreeViewModel->GetSubTrees(Subtrees);
	TreeView->SetTreeItemsSource(&Subtrees);

	// Restore expanded state
	for (const TWeakObjectPtr<USkillExecutorConfig>& State : ExpandedStates)
	{
		TreeView->SetItemExpansion(State, true);
	}

	// Restore selected state
	TreeView->ClearSelection();
	TreeView->SetItemSelection(SelectedStates, true);

	TreeView->RequestTreeRefresh();

	bItemDirty = false;
}
TSharedRef<ITableRow> SSkillTreeView::HandleGenerateRow(TWeakObjectPtr<USkillExecutorConfig> InState,
	const TSharedRef<STableViewBase>& InOwnerTableView)
{
	return SNew(SSkillExecutorTreeViewRow, InOwnerTableView, InState, ViewBox, SkillTreeViewModel.ToSharedRef());
}

void SSkillTreeView::HandleGetChildren(TWeakObjectPtr<USkillExecutorConfig> InParent,
	TArray<TWeakObjectPtr<USkillExecutorConfig>>& OutChildren)
{
	if (const USkillExecutorConfig* Parent = InParent.Get())
	{
		OutChildren.Append(Parent->Children);
	}
}

void SSkillTreeView::HandleTreeSelectionChanged(TWeakObjectPtr<USkillExecutorConfig> SkillExecutorConfig,
	ESelectInfo::Type Arg)
{
	if (!SkillTreeViewModel)
	{
		return;
	}
	//避免是代码设置的选中
	// Do not report code based selection changes.
	if (Arg == ESelectInfo::Direct)
	{
		return;
	}
	TArray<TWeakObjectPtr<USkillExecutorConfig>> SelectedItems = TreeView->GetSelectedItems();
	bUpdatingSelection = true;
	//这里的流程是获取到选中的内容，然后再设置ViewModel中选中的内容，这个内部会调用选中更改的委托，然后再进行刷新等操作
	SkillTreeViewModel->SetSelection(SelectedItems);
	bUpdatingSelection = false;
}

void SSkillTreeView::HandleTreeExpansionChanged(TWeakObjectPtr<USkillExecutorConfig> SkillExecutorConfig, bool bArg)
{
	if (USkillExecutorConfig* State = SkillExecutorConfig.Get())
	{
		State->bExpand =bArg;
	}
}

FReply SSkillTreeView::HandleAddStateButton()
{
	if (SkillTreeViewModel == nullptr)
	{
		return FReply::Handled();
	}
	
		//点击那个就直接往总的子树上面加
		// Add root state at the lowest level.
		SkillTreeViewModel->AddState(nullptr);

	return FReply::Handled();
}

TSharedPtr<SWidget> SSkillTreeView::HandleContextMenuOpening()
{
	
	if (!SkillTreeViewModel)
	{
		return nullptr;
	}

	FMenuBuilder MenuBuilder(true, CommandList);

	MenuBuilder.AddSubMenu(
		LOCTEXT("AddConfig", "Add Config"),
		FText(),
		FNewMenuDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder)
		{
			MenuBuilder.AddMenuEntry(FSkillExecutorEditorCommand::Get().AddSiblingState);
			MenuBuilder.AddMenuEntry(FSkillExecutorEditorCommand::Get().AddChildState);
		}),
		/*bInOpenSubMenuOnClick =*/false,
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Plus")
	);

	MenuBuilder.AddSeparator();
	//这个是增加一个按钮在主菜单
	MenuBuilder.AddMenuEntry(FSkillExecutorEditorCommand::Get().CutStates);
	MenuBuilder.AddMenuEntry(FSkillExecutorEditorCommand::Get().CopyStates);

	//这个是增加子菜单
	MenuBuilder.AddSubMenu(
		LOCTEXT("Paste", "Paste"),
		FText(),
		FNewMenuDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder)
		{
			MenuBuilder.AddMenuEntry(FSkillExecutorEditorCommand::Get().PasteStatesAsSiblings);
			MenuBuilder.AddMenuEntry(FSkillExecutorEditorCommand::Get().PasteStatesAsChildren);
		}),
		/*bInOpenSubMenuOnClick =*/false,
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "GenericCommands.Paste")
	);
	
	MenuBuilder.AddMenuEntry(FSkillExecutorEditorCommand::Get().DuplicateStates);
	MenuBuilder.AddMenuEntry(FSkillExecutorEditorCommand::Get().DeleteStates);
	MenuBuilder.AddMenuEntry(FSkillExecutorEditorCommand::Get().RenameState);

	return MenuBuilder.MakeWidget();
}

USkillExecutorConfig* SSkillTreeView::GetFirstSelectedState()
{
	TArray<USkillExecutorConfig*> SelectedStates;
	if (SkillTreeViewModel)
	{
		SkillTreeViewModel->GetSelectedStates(SelectedStates);
	}
	return SelectedStates.IsEmpty() ? nullptr : SelectedStates[0];
}

void SSkillTreeView::HandleAddSiblingState()
{
	if (SkillTreeViewModel)
	{
		SkillTreeViewModel->AddState(GetFirstSelectedState());
	}
}

void SSkillTreeView::HandleAddChildState()
{
	if (SkillTreeViewModel)
	{
		USkillExecutorConfig* ParentState = GetFirstSelectedState();
		if (ParentState)
		{
			SkillTreeViewModel->AddChildState(ParentState);
			TreeView->SetItemExpansion(ParentState, true);
		}
	}
}

bool SSkillTreeView::HasSelection()
{
	return SkillTreeViewModel && SkillTreeViewModel->HasSelected();
}

void SSkillTreeView::HandleCutSelectedStates()
{
	if (SkillTreeViewModel)
	{
		SkillTreeViewModel->CopySelectedStates();
		SkillTreeViewModel->RemoveSelectedStates();
	}
}

void SSkillTreeView::HandleCopySelectedStates()
{
	if (SkillTreeViewModel)
	{
		SkillTreeViewModel->CopySelectedStates();
	}
}
void SSkillTreeView::HandlePasteStatesAsSiblings()
{
	if (SkillTreeViewModel)
	{
		SkillTreeViewModel->PasteStatesFromClipboard(GetFirstSelectedState());
	}
}
void SSkillTreeView::HandlePasteStatesAsChildren()
{
	if (SkillTreeViewModel)
	{
		SkillTreeViewModel->PasteStatesAsChildrenFromClipboard(GetFirstSelectedState());
	}
}
bool SSkillTreeView::CanPaste()
{
	return SkillTreeViewModel
			&& SkillTreeViewModel->HasSelected()
			&& SkillTreeViewModel->CanPasteStatesFromClipBoard();
}

void SSkillTreeView::HandleDuplicateSelectedStates()
{
	if (SkillTreeViewModel)
	{
		SkillTreeViewModel->PasteStatesAsChildrenFromClipboard(GetFirstSelectedState());
	}
}

void SSkillTreeView::HandleRenameState()
{
	RequestRenameState = GetFirstSelectedState();
}

void SSkillTreeView::HandleDeleteStates()
{
	if (SkillTreeViewModel)
	{
		SkillTreeViewModel->RemoveSelectedStates();
	}
}

void SSkillTreeView::HandleModelAssetChanged()
{
	bItemDirty = true;
}

void SSkillTreeView::HandleModelStatesRemoved(const TSet<USkillExecutorConfig*>& SkillExecutorConfigs)
{
	bItemDirty = true;
}

void SSkillTreeView::HandleModelStatesMoved(const TSet<USkillExecutorConfig*>& SkillExecutorConfigs,
	const TSet<USkillExecutorConfig*>& Set)
{
	bItemDirty=true;
}

void SSkillTreeView::HandleModelStateAdded(USkillExecutorConfig* ParentSkillExecutorConfig,
	USkillExecutorConfig* NewSkillExecutorConfig)
{
	bItemDirty=true;
	RequestRenameState= NewSkillExecutorConfig;
	if (SkillTreeViewModel.IsValid())
	{
		SkillTreeViewModel->SetSelection(NewSkillExecutorConfig);
	}
}

void SSkillTreeView::HandleModelStatesChanged(const TSet<USkillExecutorConfig*>& SkillExecutorConfigs,
	const FPropertyChangedEvent& PropertyChangedEvent)
{
	//这里不管更改了什么都重建一下List
	TreeView->RebuildList();
}

void SSkillTreeView::HandleModelSelectionChanged(const TArray<TWeakObjectPtr<USkillExecutorConfig>>& SelectedObjects)
{
	if (bUpdatingSelection)
	{
		return;
	}
	TreeView->ClearSelection();

	if (SelectedObjects.Num() > 0)
	{
		TreeView->SetItemSelection(SelectedObjects, /*bSelected*/true);

		if (SelectedObjects.Num() == 1)
		{	//将选中的内容滚动到视口内部
			TreeView->RequestScrollIntoView(SelectedObjects[0]);	
		}
	}
	
}
#undef LOCTEXT_NAMESPACE