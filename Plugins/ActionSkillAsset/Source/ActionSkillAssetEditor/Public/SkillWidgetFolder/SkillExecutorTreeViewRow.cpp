#include "SkillExecutorTreeViewRow.h"
#include "StateTreeEditorStyle.h"
#include "SkillExecutorConfigAssetEditorFold/FSkillExecutorConfigEditor.h"
#include "SkillExecutorTreeViewModel/FSkillTreeViewModel.h"
#include "SkillExecutorTreeViewModel/SkillExecutorEditorData.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "SkillExecutorTreeViewRow"
void SSkillExecutorTreeViewRow::Construct(const FArguments& InArgs,const TSharedRef<STableViewBase>& InOwnerTableView, TWeakObjectPtr<USkillExecutorConfig>InConfig,const TSharedPtr<SScrollBox>& ViewBox, TSharedPtr<FSkillTreeViewModel> InSkillTreeViewModel)
{
	WeakConfig=InConfig;
	SkillTreeViewModel=InSkillTreeViewModel;
	const USkillExecutorConfig * Config=InConfig.Get();
	EditorData=Config!=nullptr?Config->GetTypedOuter<USkillExecutorEditorData>():nullptr;
	ConstructInternal(
		STableRow::FArguments()
		.Padding(5.0f)
		.OnDragDetected(this,&SSkillExecutorTreeViewRow::HandleDragDetected)
		.OnCanAcceptDrop(this,&SSkillExecutorTreeViewRow::HandleCanAcceptDrop)
		.OnAcceptDrop(this,&SSkillExecutorTreeViewRow::HandleAcceptDrop)
		.Style(&FStateTreeEditorStyle::Get().GetWidgetStyle<FTableRowStyle>("StateTree.Selection"))
		,InOwnerTableView);
	//背景还有Icon的颜色
	static const FLinearColor LinkBackground = FLinearColor(FColor(84, 84, 84));
	static constexpr FLinearColor IconTint = FLinearColor(1, 1, 1, 0.5f);

	this->ChildSlot
	.HAlign(HAlign_Fill)
	[
		//这个位置是这一整条到界面的左右边界
		SNew(SBox)
		//这个滚动框是整个屏幕的滚动框，当状态树内容太多时需要调整其最小值
		.MinDesiredWidth_Lambda([WeakOwnerViewBox=ViewBox.ToWeakPtr()]()
		{
				if (const TSharedPtr<SScrollBox> OwnerViewBox=WeakOwnerViewBox.Pin())
				{
					return OwnerViewBox->GetTickSpaceGeometry().GetLocalSize().X-1;
				}
		return 0.f;
		}
		)
		[
			//这个位置是这一整条到界面的左右边界
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Left)
			.AutoWidth()
			[
				//这个位置是箭头还有连线
				SNew(SExpanderArrow,SharedThis(this))
				//是否绘制“连线”。在树形控件中，通常会有一条线连接父子节点，设置为 true 时会显示这些线。
				.ShouldDrawWires(true)
				//每一级缩进的像素数。比如设置为 32，每多一层子节点就会向右缩进 32 像素。
				.IndentAmount(32)
				//.BaseIndentLevel(0)
				.BaseIndentLevel(0)
			]
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Left)
			.Padding(FMargin(0.0f,4.0f))
			.AutoWidth()
			[
				//这个是整个状态栏就是那个蓝框子
				SNew(SBox)
				.HeightOverride(28.0f)
				.VAlign(VAlign_Fill)
				[
					SNew(SBorder)
					.BorderImage(FStateTreeEditorStyle::Get().GetBrush("StateTree.State.Border"))
					.BorderBackgroundColor(this,&SSkillExecutorTreeViewRow::GetActiveStateColor)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						.AutoWidth()
						[
							SNew(SBox)
							.WidthOverride(4.0f)
							.HeightOverride(10.0f)
							[	//左边在子树会有一个颜色标识
								SNew(SBorder)
								.BorderImage(FAppStyle::GetBrush("WhiteBrush"))
								.BorderBackgroundColor_Raw(this,&SSkillExecutorTreeViewRow::GetSubTreeMarketColor)
							]
						]
						+SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						.AutoWidth()
						[
							SNew(SBox)
							.HeightOverride(28.0f)
							.VAlign(VAlign_Fill)
							[
								SNew(SBorder)
								.BorderImage(FAppStyle::GetBrush("WhiteBrush"))
								.BorderBackgroundColor(this,&SSkillExecutorTreeViewRow::GetTitleColor)
								.Padding(FMargin(4.0f,0.0f,12.0f,0.0f))
								[
									SNew(SOverlay)
									+SOverlay::Slot()
									[	//蓝框左边有个小图标
										SNew(SHorizontalBox)
										+SHorizontalBox::Slot()
										.VAlign(VAlign_Center)
										.AutoWidth()
										[
											SNew(SBox)
											.Padding(FMargin(0.0f,0.0f,4.0f,0.0f))
											.Visibility(EVisibility::Visible)
											[
												SNew(SImage)
												.ColorAndOpacity(IconTint)
												.Image(FStateTreeEditorStyle::Get().GetBrush("StateTreeEditor.Conditions"))
												.ToolTipText(LOCTEXT("SkillExecutorConfigImage","SkillConditions"))
											]
										]
										+ SHorizontalBox::Slot()
										.VAlign(VAlign_Center)
										.AutoWidth()
										[
											SNew(SBox)
											.Padding(FMargin(0.0f, 0.0f, 4.0f, 0.0f))
											[
												SNew(SImage)
												.Image(this, &SSkillExecutorTreeViewRow::GetSelectorIcon)
												.ColorAndOpacity(IconTint)
												.ToolTipText(this, &SSkillExecutorTreeViewRow::GetSelectorTooltip)
											]
										]
										+SHorizontalBox::Slot()
										.VAlign(VAlign_Center)
										.AutoWidth()
										[
											SAssignNew(NameTextBlock,SInlineEditableTextBlock)
											.Style(FStateTreeEditorStyle::Get(), "StateTree.State.TitleInlineEditableText")
											.OnVerifyTextChanged_Lambda([](const FText& NewLabel, FText& OutErrorMessage)
												{
													return !NewLabel.IsEmptyOrWhitespace();
												})
											.OnTextCommitted(this, &SSkillExecutorTreeViewRow::HandleNodeLabelTextCommitted)
											.Text(this, &SSkillExecutorTreeViewRow::GetStateDesc)
											.ToolTipText(this, &SSkillExecutorTreeViewRow::GetStateTypeTooltip)
											.Clipping(EWidgetClipping::ClipToBounds)
											.IsSelected(this, &SSkillExecutorTreeViewRow::IsStateSelected)
										]
										
									]
								]
							]
						]
					]
				]
			]
		]
	];
}

void SSkillExecutorTreeViewRow::RequestRename() const
{
	if (NameTextBlock)
	{
		NameTextBlock->EnterEditingMode();
	}
}

FSlateColor SSkillExecutorTreeViewRow::GetTitleColor() const
{
	const USkillExecutorConfig* State = WeakConfig.Get();
	const USkillExecutorEditorData* SKillData = EditorData.Get();

	if (State != nullptr && SKillData != nullptr)
	{
		return  State->StateColor;
	}

	return FLinearColor(FColor(31, 151, 167));
}

FSlateColor SSkillExecutorTreeViewRow::GetActiveStateColor() const
{
	if (const USkillExecutorConfig * Config=WeakConfig.Get())
	{
		if (SkillTreeViewModel && SkillTreeViewModel->IsSelected(Config))
		{
			return FLinearColor(FColor(236, 134, 39));
		}
	}
	return FLinearColor::Transparent;
}

FSlateColor SSkillExecutorTreeViewRow::GetSubTreeMarketColor() const
{
	if (const TObjectPtr<USkillExecutorConfig>State = WeakConfig.Get())
	{
		if ( EditorData->SubTrees.Find(State))
		{
			const FSlateColor TitleColor = GetTitleColor();
			return FLinearColor::Gray;
		}
	}

	return GetTitleColor();
}

FText SSkillExecutorTreeViewRow::GetStateDesc() const 
{
	if (const USkillExecutorConfig* State = WeakConfig.Get())
	{
		return FText::FromName(State->SkillDescriptorName);
	}
	return FText::FromName(FName());
}

FText SSkillExecutorTreeViewRow::GetStateID() const
{
	if (const USkillExecutorConfig* State = WeakConfig.Get())
	{
		return FText::FromString(*LexToString(State->ID));
	}
	return FText::FromName(FName());
}

const FSlateBrush* SSkillExecutorTreeViewRow::GetSelectorIcon() const
{
	if (const USkillExecutorConfig* config = WeakConfig.Get())
	{
		if (config->Parent==nullptr && EditorData->SubTrees.Find(config->Parent) )
		{
			return FStateTreeEditorStyle::Get().GetBrush("StateTreeEditor.TrySelectChildrenInOrder");
		}
		if (config->Children.Num()<=0)
		{
			return FStateTreeEditorStyle::Get().GetBrush("StateTreeEditor.SelectNone");
		}
		else
		{
			return FStateTreeEditorStyle::Get().GetBrush("StateTreeEditor.TryEnterState");	
		}
		
	}
	return nullptr;
}

FText SSkillExecutorTreeViewRow::GetSelectorTooltip() const
{
	return LOCTEXT("sKillExecutorTreeViewRow","A Skill Config");
}

void SSkillExecutorTreeViewRow::HandleNodeLabelTextCommitted(const FText& Text, ETextCommit::Type Arg)
{
	if (SkillTreeViewModel)
	{
		if (USkillExecutorConfig* State = WeakConfig.Get())
		{
			SkillTreeViewModel->RenameState(State, FName(*FText::TrimPrecedingAndTrailing(Text).ToString()));
		}
	}
}

FText SSkillExecutorTreeViewRow::GetStateTypeTooltip() const
{
	return LOCTEXT("SkillTree","Config");
}

bool SSkillExecutorTreeViewRow::IsStateSelected()
{
	if (const USkillExecutorConfig* State = WeakConfig.Get())
	{
		if (SkillTreeViewModel)
		{
			return SkillTreeViewModel->IsSelected(State);
		}
	}
	return false;
}

bool SSkillExecutorTreeViewRow::IsRootState() const
{
	if (const USkillExecutorConfig* State = WeakConfig.Get())
	{
		if (SkillTreeViewModel)
		{
			return SkillTreeViewModel->IsSelected(State);
		}
	}
	return false;
}

FReply SSkillExecutorTreeViewRow::HandleDragDetected(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
	return FReply::Handled().BeginDragDrop(FSkillTreeViewDragDrop::New(WeakConfig.Get()));
}
TOptional<EItemDropZone> SSkillExecutorTreeViewRow::HandleCanAcceptDrop(const FDragDropEvent& DragDropEvent,
	EItemDropZone InItemDropZone,TWeakObjectPtr<USkillExecutorConfig> SkillExecutorConfig)
{
	const TSharedPtr<FSkillTreeViewDragDrop> DragDropOperation=DragDropEvent.GetOperationAs<FSkillTreeViewDragDrop>();
	if (DragDropOperation.IsValid())
	{
		if (SkillTreeViewModel && SkillTreeViewModel->IsChildofSelection(SkillExecutorConfig.Get()))
		{
			return TOptional<EItemDropZone>();
		}
		return InItemDropZone;
	}
	return  TOptional<EItemDropZone>();
}

FReply SSkillExecutorTreeViewRow::HandleAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone,
	TWeakObjectPtr<USkillExecutorConfig> SkillExecutorConfig)
{
	const TSharedPtr<FSkillTreeViewDragDrop> DragDropOperation = DragDropEvent.GetOperationAs<FSkillTreeViewDragDrop>();
	if (DragDropOperation.IsValid())
	{
		if (SkillTreeViewModel)
		{
			if (InItemDropZone==EItemDropZone::AboveItem)
			{
				SkillTreeViewModel->MoveSelectedStatesBefore(SkillExecutorConfig.Get());
			}
			if (InItemDropZone==EItemDropZone::BelowItem)
			{
				SkillTreeViewModel->MoveSelectedStatesAfter(SkillExecutorConfig.Get());
			}
			else
			{
				SkillTreeViewModel->MoveSelectedStatesInto(SkillExecutorConfig.Get());
			}
			return  FReply::Handled();
		}
	}
	return FReply::Unhandled();
}
#undef LOCTEXT_NAMESPACE