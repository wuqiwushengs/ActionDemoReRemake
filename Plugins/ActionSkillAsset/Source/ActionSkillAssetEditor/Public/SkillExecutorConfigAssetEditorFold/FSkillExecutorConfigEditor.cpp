#include "FSkillExecutorConfigEditor.h"


#include "ActionSkillAssetRuntime/Public/ActionAbilityResourceFold/SkillExecutorFold/SkillExecutorDescriptor.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "SkillWidgetFolder/SkillTreeView/SkillTreeView.h"
#include "DetailsViewArgs.h"
#include "IDetailsView.h"
#include "SkillExecutorTreeViewModel/FSkillTreeViewModel.h"
#include "SkillExecutorTreeViewModel/SkillExecutorEditorData.h"
#include "UObject/ObjectSaveContext.h"


#define LOCTEXT_NAMESPACE "SkillExecutorConfigEditor"
struct FSkillExecutorAssetEditorTabs
{
	static const FName ViewportID;
	static const FName SkillConfigPropertyID;
    static 	const FName SkillTreeEditorAppName;
};
const FName FSkillExecutorAssetEditorTabs::SkillTreeEditorAppName(TEXT("SkillTreeEditorApp"));
const FName FSkillExecutorAssetEditorTabs::ViewportID(TEXT("Viewport"));
const FName FSkillExecutorAssetEditorTabs::SkillConfigPropertyID(TEXT("SkillConfigProperty"));
FSkillExecutorConfigEditor::FSkillExecutorConfigEditor()
	:TreeViewCommandList(new FUICommandList)
{
	if (UEditorEngine * EditorEngine=Cast<UEditorEngine>(GEngine))
	{
		EditorEngine->RegisterForUndo(this);
	}
	OnPackageSavedDelegateHandle=UPackage::PackageSavedWithContextEvent.AddRaw(this,&FSkillExecutorConfigEditor::OnPackageSaved);
}
void FSkillExecutorConfigEditor::OnPackageSaved(const FString& String, UPackage* Package,
	FObjectPostSaveContext ObjectPostSaveContext)
{
	//保存内容
	UpdateAsset();
}

FSkillExecutorConfigEditor::~FSkillExecutorConfigEditor()
{
	if (UEditorEngine * EditorEngine=Cast<UEditorEngine>(GEngine))
	{
	EditorEngine->UnregisterForUndo(this);
	}
	UPackage::PackageSavedWithContextEvent.Remove(OnPackageSavedDelegateHandle);
}

void FSkillExecutorConfigEditor::InitSkillExecutorConfigEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, USkillExecutorDescriptorAsset* ExecutorConfig)
{
	EditingSkillExecutorDescriptorAsset=ExecutorConfig;
	
	USkillExecutorEditorData * EditorData=Cast<USkillExecutorEditorData>(EditingSkillExecutorDescriptorAsset->EditorData);
	if (EditorData==nullptr)
	{
		EditorData=NewObject<USkillExecutorEditorData>(EditingSkillExecutorDescriptorAsset,FName(),RF_Transactional);
		EditorData->AddSubTree(FName("Root"));
		EditingSkillExecutorDescriptorAsset->EditorData=EditorData;
	}
	//初始化所有的子树，用来支持撤销和重做功能
	for (USkillExecutorConfig* SubTree : EditorData->SubTrees)
	{
		TArray<USkillExecutorConfig*> Stack;

		Stack.Add(SubTree);
		while (!Stack.IsEmpty())
		{
			if (USkillExecutorConfig* State = Stack.Pop())
			{
				State->SetFlags(RF_Transactional);
				
				for (USkillExecutorConfig* ChildState : State->Children)
				{
					Stack.Add(ChildState);
				}
			}
		}
	}
	SkillTreeViewModel=MakeShareable(new FSkillTreeViewModel());
	SkillTreeViewModel->Init(EditorData);

	SkillTreeViewModel->GetOnAssetChanged().AddSP(this, &FSkillExecutorConfigEditor::HandleModelAssetChanged);
	SkillTreeViewModel->GetOnStateAdded().AddSPLambda(this, [this](USkillExecutorConfig* , USkillExecutorConfig*){ UpdateAsset(); });
	SkillTreeViewModel->GetOnStatesRemoved().AddSPLambda(this, [this](const TSet<USkillExecutorConfig*>&){ UpdateAsset(); });
	SkillTreeViewModel->GetOnStatesMoved().AddSPLambda(this, [this](const TSet<USkillExecutorConfig*>&, const TSet<USkillExecutorConfig*>&){ UpdateAsset(); });
	SkillTreeViewModel->GetOnSelectionChanged().AddSP(this, &FSkillExecutorConfigEditor::HandleModelSelectionChanged);

	TSharedRef<FTabManager::FLayout> StandAloneDefaultLayout=FTabManager::NewLayout("Standalone_SkillExecutorTree_Layout")
	->AddArea(
	FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
							 ->Split
							 (
								 FTabManager::NewSplitter()
								 ->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
								 ->Split
								 (
									 FTabManager::NewStack()
									 ->SetSizeCoefficient(0.65f)
									 ->AddTab(FSkillExecutorAssetEditorTabs::ViewportID,
											  ETabState::OpenedTab)->
									 SetHideTabWell(true)
								 )
								 ->Split
								 (
									 FTabManager::NewSplitter()
									 ->SetOrientation(Orient_Vertical)
									 ->Split
									 (
									 FTabManager::NewStack()
									 ->SetSizeCoefficient(0.7f)
									 ->AddTab(
									 FSkillExecutorAssetEditorTabs::SkillConfigPropertyID,
									 ETabState::OpenedTab)
									 ->SetHideTabWell(true)
									 )
								 )
							 )
		);
	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, FSkillExecutorAssetEditorTabs::SkillTreeEditorAppName,StandAloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar,EditingSkillExecutorDescriptorAsset);
	RegenerateMenusAndToolbars();
	
	}

void FSkillExecutorConfigEditor::RebuildSkillConfig()
{
	UpdateAsset();
}
#pragma region ToolKitImplement
FName FSkillExecutorConfigEditor::GetToolkitFName() const
{
	return FName("SkillExecutorConfig");
}

FText FSkillExecutorConfigEditor::GetBaseToolkitName() const
{
	return LOCTEXT("SKillExecutorEditor","SkillExecutorConfig");
}

FString FSkillExecutorConfigEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("SkillExecutorEditor","SkillExecutorConfig").ToString();
}

FLinearColor FSkillExecutorConfigEditor::GetWorldCentricTabColorScale() const
{
	return FColor(0.4,0.4,0.4,1.0f);
}
#pragma endregion
#pragma region GCImplement
void FSkillExecutorConfigEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditingSkillExecutorDescriptorAsset);
}

void FSkillExecutorConfigEditor::SaveAsset_Execute()
{
	if (EditingSkillExecutorDescriptorAsset)
	{
		RebuildSkillConfig();
	}
	FAssetEditorToolkit::SaveAsset_Execute();
}

FString FSkillExecutorConfigEditor::GetReferencerName() const
{
	return LOCTEXT("SKillExecutorEditor","FSkillExecutorConfigEditor").ToString();
}

void FSkillExecutorConfigEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(
	LOCTEXT("WorkspaceMenu_SkillConfigEditor", "ConfigEditor"));
	const auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FSkillExecutorAssetEditorTabs::ViewportID,
									 FOnSpawnTab::CreateSP(this, &FSkillExecutorConfigEditor::SpawnTab_Viewport))
				.SetDisplayName(LOCTEXT("GraphCanvasTab", "Viewport"))
				.SetGroup(WorkspaceMenuCategoryRef)
				.SetIcon(FSlateIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Outliner")));

	InTabManager->RegisterTabSpawner(FSkillExecutorAssetEditorTabs::SkillConfigPropertyID,
									 FOnSpawnTab::CreateSP(this, &FSkillExecutorConfigEditor::SpawnTab_Details))
				.SetDisplayName(LOCTEXT("DetailsTab", "Property"))
				.SetGroup(WorkspaceMenuCategoryRef)
				.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FSkillExecutorConfigEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner(FSkillExecutorAssetEditorTabs::ViewportID);
	InTabManager->UnregisterTabSpawner(FSkillExecutorAssetEditorTabs::SkillConfigPropertyID);
}

TSharedRef<SDockTab> FSkillExecutorConfigEditor::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FSkillExecutorAssetEditorTabs::ViewportID);
	

	return SNew(SDockTab)
		.Label(NSLOCTEXT("SkillTreeEditor", "SkillTreeViewTab", "SkillExecutorTree"))
		.TabColorScale(GetTabColorScale())
		[
			SAssignNew(SkillTreeView, SSkillTreeView, SkillTreeViewModel.ToSharedRef(), TreeViewCommandList)
		];;
}

void FSkillExecutorConfigEditor::OnAssetFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
		if (SkillTreeViewModel)
    	{
    		SkillTreeViewModel->NotifyAssetChangedExternally();
    	}
}

TSharedRef<SDockTab> FSkillExecutorConfigEditor::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FSkillExecutorAssetEditorTabs::SkillConfigPropertyID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("DetailTab_Title", "Detail"));
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	SelectionDetailsView= PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	SelectionDetailsView->SetObject( nullptr);
	SelectionDetailsView->OnFinishedChangingProperties().AddSP(this, &FSkillExecutorConfigEditor::OnAssetFinishedChangingProperties);
	
	if (SelectionDetailsView.IsValid())
	{
		SpawnedTab->SetContent(SelectionDetailsView.ToSharedRef());
	}

	return SpawnedTab;
}

void FSkillExecutorConfigEditor::HandleModelAssetChanged()
{
	UpdateAsset();
}

void FSkillExecutorConfigEditor::HandleModelSelectionChanged(
	const TArray<TWeakObjectPtr<USkillExecutorConfig>>& SelectedStates)
{
	if (SelectionDetailsView)
	{
		TArray<UObject*> Selected;
		for (const TWeakObjectPtr<USkillExecutorConfig>& WeakState : SelectedStates)
		{
			if (USkillExecutorConfig* State = WeakState.Get())
			{
				Selected.Add(State);
			}
		}
		SelectionDetailsView->SetObjects(Selected);
	}
}

void FSkillExecutorConfigEditor::UpdateAsset()
{
	if (!EditingSkillExecutorDescriptorAsset||!EditingSkillExecutorDescriptorAsset->EditorData)
	{
		return;
	}
	//直接复制过来。
	USkillExecutorEditorData * EditorData=Cast<USkillExecutorEditorData>(EditingSkillExecutorDescriptorAsset->EditorData);
	EditingSkillExecutorDescriptorAsset->ExecutorConfigs=EditorData->SubTrees;
}
#pragma endregion 


#undef LOCTEXT_NAMESPACE