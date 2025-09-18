#include "FSkillTreeViewModel.h"

#include "Factories.h"
#include "SkillExecutorEditorData.h"
#include "UnrealExporter.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "Exporters/Exporter.h"

#define LOCTEXT_NAMESPACE "SkillTreeViewModel"

namespace SkillExecutor::Editor
{

	class FSkillExecutorConfigTextFactory:public FCustomizableTextObjectFactory
	{
	public:
		FSkillExecutorConfigTextFactory()
			:FCustomizableTextObjectFactory(GWarn)
		{}
		virtual bool CanCreateClass(UClass* InObjectClass, bool& bOmitSubObjs) const override
		{
			UE_LOG(LogTemp, Error, TEXT("*** CanCreateClass: %s"), *GetNameSafe(InObjectClass));
			return InObjectClass->IsChildOf(USkillExecutorConfig::StaticClass());
		};
		virtual void ProcessConstructedObject(UObject* CreatedObject) override
		{
			if (USkillExecutorConfig * Config=Cast<USkillExecutorConfig>(CreatedObject))
			{
				Configs.Add(Config);
			}
		};
		TArray<USkillExecutorConfig*> Configs;
	};
	bool IsChildof(const USkillExecutorConfig * Parent,const USkillExecutorConfig *state) 
	{
		for (const USkillExecutorConfig *Child:Parent->Children)
		{
			if (Child==state)
			{
				return true;
			}
			if (IsChildof(Child,state))
			{
				return true;
			}
		}
		return false;
	};
	void RemoveContainedChildren(TArray<USkillExecutorConfig*> Configs)
	{
		//这样做用来清理冗余的配置，保留顶层或者独立的配置，避免有些父亲在下面连接的子类也在里面。
		TSet<USkillExecutorConfig * >UniqueConfig;
		for (USkillExecutorConfig * Config:Configs)
		{
			UniqueConfig.Add(Config);
		}
		for (int32 i=0;i<UniqueConfig.Num();)
		{
			USkillExecutorConfig* Config=Configs[i];
			USkillExecutorConfig*ConfigParent=Config->Parent;
			bool bShouldRemove=false;
			while (ConfigParent)
			{
				if (UniqueConfig.Contains(ConfigParent))
				{
					bShouldRemove=true;
					break;
				}
				ConfigParent=ConfigParent->Parent;
			}
			if (bShouldRemove)
			{
				Configs.RemoveAt(i);	
			}
			else
			{
				i++;
			}
			
		}
	};
	FString ExportConfigToText(USkillExecutorEditorData * TreeData,const TArrayView<USkillExecutorConfig*> Configs)
	{
		if (Configs.IsEmpty())
		{
			return FString();
		}
		UnMarkAllObjects(EObjectMark(OBJECTMARK_TagExp|OBJECTMARK_TagImp));
		FStringOutputDevice Archive;
		const FExportObjectInnerContext Context;
		for (USkillExecutorConfig * Config:Configs)
		{
			UObject * ThisOuter=Config->GetOuter();
			UExporter::ExportToOutputDevice(&Context,Config,nullptr,Archive,TEXT("copy"), 0, PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited, false, ThisOuter);
			
		}
		return *Archive;
	};
	void FixSkillConfigAfterDuplication(USkillExecutorConfig * Config,USkillExecutorConfig * NewParentConfig,TArray<USkillExecutorConfig*>&NewConfigs)
	{
		Config->Modify();
		const FGuid NewStateID=FGuid::NewGuid();
		Config->ID=NewStateID;
		Config->Parent=NewParentConfig;
		NewConfigs.Add(Config);
		for (USkillExecutorConfig * Child:Config->Children)
		{
			FixSkillConfigAfterDuplication(Child,Config,NewConfigs);
		}
	};
}

FSkillTreeViewModel::FSkillTreeViewModel()
	:SkillTreeEditorDataWeak(nullptr)
{
}

FSkillTreeViewModel::~FSkillTreeViewModel()
{
	GEditor->UnregisterForUndo(this);
}



void FSkillTreeViewModel::Init(USkillExecutorEditorData* InTreeData)
{
	SkillTreeEditorDataWeak=InTreeData;
	GEditor->RegisterForUndo(this);
}
const USkillExecutorDescriptorAsset* FSkillTreeViewModel::GetSkillTree() 
{
	if (const USkillExecutorEditorData * EditorData=SkillTreeEditorDataWeak.Get())
	{
		return EditorData->GetTypedOuter<USkillExecutorDescriptorAsset>();
	}
	UE_LOG(LogTemp,Warning,TEXT("This EditorData don't have a  DescriptorAssetData"))
	return nullptr;
}
TArray<TObjectPtr<USkillExecutorConfig>>* FSkillTreeViewModel::GetSubTrees() const
{
	USkillExecutorEditorData * EditorData=SkillTreeEditorDataWeak.Get();
	return EditorData!=nullptr? &EditorData->SubTrees:nullptr;
}
int FSkillTreeViewModel::GetSubTreeCount() const
{
	USkillExecutorEditorData * EditorData=SkillTreeEditorDataWeak.Get();
	return EditorData!=nullptr?EditorData->SubTrees.Num():0;
	
}
void FSkillTreeViewModel::GetSubTrees(TArray<TWeakObjectPtr<USkillExecutorConfig>> &OutSubTrees) const
{
	OutSubTrees.Reset();
	if (USkillExecutorEditorData * TreeEditorData=SkillTreeEditorDataWeak.Get())
	{
		for (USkillExecutorConfig *SubTree:TreeEditorData->SubTrees)
		{
		   OutSubTrees.Add(SubTree);
		}
	}
}

void FSkillTreeViewModel::HandleIdentifierChanged(const USkillExecutorDescriptorAsset& SkillExecutorDescriptorAsset)
{
	if (GetSkillTree()==&SkillExecutorDescriptorAsset)
	{
		OnAssetChanged.Broadcast();	
	}
	
}

void FSkillTreeViewModel::PostRedo(bool bSuccess)
{
	OnAssetChanged.Broadcast();
}
void FSkillTreeViewModel::PostUndo(bool bSuccess)
{
	OnAssetChanged.Broadcast();
}

void FSkillTreeViewModel::ClearSelection()
{
	SelectedSkillExecutorConfigs.Reset();
	const TArray<TWeakObjectPtr<USkillExecutorConfig>> SelectSkillExecutorConfigs;
	OnSelectionChanged.Broadcast(SelectSkillExecutorConfigs);
}
void FSkillTreeViewModel::SetSelection(USkillExecutorConfig* Selected)
{
	SelectedSkillExecutorConfigs.Reset();
	SelectedSkillExecutorConfigs.Add(Selected);
	TArray<TWeakObjectPtr<USkillExecutorConfig>> SelectedSkillExecutorArr;
	SelectedSkillExecutorArr.Add(Selected);
	OnSelectionChanged.Broadcast(SelectedSkillExecutorArr);
}
void FSkillTreeViewModel::SetSelection(const TArray<TWeakObjectPtr<USkillExecutorConfig>>& InSelection)
{
	SelectedSkillExecutorConfigs.Reset();
	for (const TWeakObjectPtr<USkillExecutorConfig>& state:InSelection)
	{
		if (state.Get())
		{
			SelectedSkillExecutorConfigs.Add(state);
		}
	}
	OnSelectionChanged.Broadcast(InSelection);
}
bool FSkillTreeViewModel::IsSelected(const USkillExecutorConfig* Config) const
{
	const TWeakObjectPtr<USkillExecutorConfig> WeakConfig=const_cast<USkillExecutorConfig*>(Config);
	return SelectedSkillExecutorConfigs.Contains(WeakConfig);
}
bool FSkillTreeViewModel::IsChildofSelection(const USkillExecutorConfig * Config)
{
	for (const TWeakObjectPtr<USkillExecutorConfig>& WeakSelectedState : SelectedSkillExecutorConfigs)
	{
		if (const USkillExecutorConfig* SelectedExecutorConfig = Cast<USkillExecutorConfig>(WeakSelectedState.Get()))
		{
			if (SelectedExecutorConfig == Config)
			{
				return true;
			}
			
			if (SkillExecutor::Editor::IsChildof(SelectedExecutorConfig,Config))
			{
				return true;
			}
		}
	}
	return false;
}
void FSkillTreeViewModel::GetSelectedStates(TArray<USkillExecutorConfig*>& OutSelectedSkillDescription)
{
	OutSelectedSkillDescription.Reset();
	for (const TWeakObjectPtr<USkillExecutorConfig> & WeakConfig:SelectedSkillExecutorConfigs)
	{
		if (USkillExecutorConfig * SkillExecutorConfig=WeakConfig.Get())
		{
			OutSelectedSkillDescription.Add(SkillExecutorConfig);
		}
	}
}
void FSkillTreeViewModel::GetSelectedStates(TArray<TWeakObjectPtr<USkillExecutorConfig>>& OutSelectedSkillDescription)
{
	OutSelectedSkillDescription.Reset();
	for (const TWeakObjectPtr<USkillExecutorConfig> & WeakConfig:OutSelectedSkillDescription)
	{
		if (WeakConfig.Get())
		{
			OutSelectedSkillDescription.Add(WeakConfig);
		}
	}
}
bool FSkillTreeViewModel::HasSelected() const
{
	return SelectedSkillExecutorConfigs.Num()>0;
}


//用于向ui提供数据库
void FSkillTreeViewModel::SetPersistentExpandedStates(TSet<TWeakObjectPtr<USkillExecutorConfig>>& InExpandedStates)
{
	//检测是否为空，如果为空说明操作对象很可能已经没了所以无意义
	USkillExecutorEditorData * TreeData=SkillTreeEditorDataWeak.Get();
	if (TreeData==nullptr)
	{
		return;
	}
	TreeData->Modify();
	for(TWeakObjectPtr<USkillExecutorConfig> & WeakState:InExpandedStates)
	{
		if (USkillExecutorConfig * SkillConfig=WeakState.Get())
		{
			SkillConfig->bExpand=true;
		}
	}
}
void FSkillTreeViewModel::GetPersistentExpandedStates(TSet<TWeakObjectPtr<USkillExecutorConfig>>& OutExpandedStates)
{
	//获取所有Expanded是真的状态,即被展开的状态。
	OutExpandedStates.Reset();
	if (USkillExecutorEditorData * SkillTreeData=SkillTreeEditorDataWeak.Get())
	{
		for(USkillExecutorConfig * SubSkillTree:SkillTreeData->SubTrees)
		{
			GetExpandedStatesRecursive(SubSkillTree,OutExpandedStates);
		}
	}
}
void FSkillTreeViewModel::GetExpandedStatesRecursive(USkillExecutorConfig* Config,
                                                     TSet<TWeakObjectPtr<USkillExecutorConfig>>& OutExpandedStates)
{
	if (Config->bExpand)
	{
		OutExpandedStates.Add(Config);
	}
	for (USkillExecutorConfig * Child:Config->Children)
	{
		GetExpandedStatesRecursive(Child,OutExpandedStates);
	}
}

void FSkillTreeViewModel::AddState(USkillExecutorConfig* AfterState)
{
	USkillExecutorEditorData * EditorData=SkillTreeEditorDataWeak.Get();
	if (EditorData==nullptr)
	{
		return;
	}
	const FScopedTransaction Transaction(LOCTEXT("AddSkillStateTransaction","AddSkillConfig"));
	USkillExecutorConfig * NewConfig=NewObject<USkillExecutorConfig>(EditorData,FName(),RF_Transactional);
	USkillExecutorConfig * ParentSkillConfig=nullptr;
	if (AfterState==nullptr)
	{
		//我这里是只要找不到父类那么就往总的子树上面添加
		EditorData->Modify();
		EditorData->SubTrees.Add(NewConfig);
	}
	else
	{
		ParentSkillConfig=AfterState->Parent;
		if (ParentSkillConfig!=nullptr)
		{
			ParentSkillConfig->Modify();
		}
		else
		{
			EditorData->Modify();
		}
		TArray<TObjectPtr<USkillExecutorConfig>> &ParentArray=ParentSkillConfig?ParentSkillConfig->Children:EditorData->SubTrees;
		const int32 TargetIndex=ParentArray.Find(AfterState);
		if (TargetIndex !=INDEX_NONE)
		{
			ParentArray.Insert(NewConfig,TargetIndex+1);
			NewConfig->Parent=ParentSkillConfig;
		}
		else
		{
			ensureMsgf(false, TEXT("%s: Failed to find specified target state %s on state %s while adding new state."), *GetNameSafe(EditorData->GetOuter()), *GetNameSafe(AfterState), *GetNameSafe(ParentSkillConfig));
			ParentArray.Add(NewConfig);
			NewConfig->Parent=ParentSkillConfig;
		}
	}
	OnStateAdded.Broadcast(ParentSkillConfig,NewConfig);
}
void FSkillTreeViewModel::AddChildState(USkillExecutorConfig* ParentState)
{
	USkillExecutorEditorData * EditorData = SkillTreeEditorDataWeak.Get();
	if (EditorData == nullptr|| ParentState== nullptr)
	{
		return;
	}
	const FScopedTransaction Transaction(LOCTEXT("AddChildSkillConfig","AddChildConfig"));
	USkillExecutorConfig * NewConfig=NewObject<USkillExecutorConfig>(ParentState,FName(),RF_Transactional);
	ParentState->Modify();
	ParentState->Children.Add(NewConfig);
	NewConfig->Parent=ParentState;

	OnStateAdded.Broadcast(ParentState,NewConfig);
}
void FSkillTreeViewModel::RenameState(USkillExecutorConfig* State, FName NewName)
{
	if (State==nullptr)
	{
		return;
	}
	const FScopedTransaction Transaction(LOCTEXT("RenameTransaction","Rename"));
	State->Modify();
	State->SkillDescriptorName=NewName;

	TSet<USkillExecutorConfig*> AffectedStates;
	AffectedStates.Add(State);

	FProperty * NameProperty=FindFProperty<FProperty>(USkillExecutorConfig::StaticClass(),GET_MEMBER_NAME_CHECKED(USkillExecutorConfig,SkillDescriptorName));
	FPropertyChangedEvent PropertyChangedEvent(NameProperty,EPropertyChangeType::ValueSet);
	OnStatesChanged.Broadcast(AffectedStates,PropertyChangedEvent);
}
void FSkillTreeViewModel::RemoveSelectedStates()
{
	USkillExecutorEditorData * EditorData = SkillTreeEditorDataWeak.Get();
	if (EditorData == nullptr)
	{
		return;
	}
	TArray<USkillExecutorConfig *> States;
	GetSelectedStates(States);
	if (States.Num()>0)
	{
		const FScopedTransaction Transaction(LOCTEXT("DeleteStateTransaction","DeleteState"));
		TSet<USkillExecutorConfig*> AffectedParents;
		for (USkillExecutorConfig * SkillConfigNeedRemove:States)
		{
			if (SkillConfigNeedRemove)
			{
				SkillConfigNeedRemove->Modify();
				USkillExecutorConfig* Parent= SkillConfigNeedRemove->Parent;
				if (Parent)
				{
					AffectedParents.Add(Parent);
					Parent->Modify();
				}
				else
				{
					AffectedParents.Add(nullptr);
					EditorData->Modify();
				}
				TArray<TObjectPtr<USkillExecutorConfig>>& ArrayToRemoveFrom=Parent?Parent->Children:EditorData->SubTrees;
				const int32 ItemIndex=ArrayToRemoveFrom.Find(SkillConfigNeedRemove);
				if (ItemIndex !=INDEX_NONE)
				{
					ArrayToRemoveFrom.RemoveAt(ItemIndex);
					SkillConfigNeedRemove->Parent=nullptr;
				}
			}
		}
		OnStatesRemoved.Broadcast(AffectedParents);
	}
}
void FSkillTreeViewModel::CopySelectedStates()
{
	USkillExecutorEditorData * EditorData = SkillTreeEditorDataWeak.Get();
	if (EditorData == nullptr)
	{
		return;
	}
	TArray<USkillExecutorConfig *> SkillConfigs;
	GetSelectedStates(SkillConfigs);
	SkillExecutor::Editor::RemoveContainedChildren(SkillConfigs);
	FString ExportText=SkillExecutor::Editor::ExportConfigToText(EditorData,SkillConfigs);
	FPlatformApplicationMisc::ClipboardCopy(*ExportText);
}
bool FSkillTreeViewModel::CanPasteStatesFromClipBoard()
{
	FString TextToImport;
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);

	SkillExecutor::Editor::FSkillExecutorConfigTextFactory Factory;
	return Factory.CanCreateObjectsFromText(TextToImport);
}
void FSkillTreeViewModel::PasteStatesFromClipboard(USkillExecutorConfig * AfterConfig)
{
	USkillExecutorEditorData * EditorData=SkillTreeEditorDataWeak.Get();
	if (EditorData==nullptr)
	{
		return;
	}
	if(AfterConfig)
	{
		const int32 Index=AfterConfig->Parent?AfterConfig->Parent->Children.Find(AfterConfig):EditorData->SubTrees.Find(AfterConfig);
		if (Index!=INDEX_NONE)
		{
			FString TextToImport;
			FPlatformApplicationMisc::ClipboardPaste(TextToImport);
			const FScopedTransaction Transaction(LOCTEXT("PasteSkillConfigTransaction","Paste SkillConfig"));
			PasteStatesAsChildrenFromText(TextToImport, AfterConfig->Parent, Index + 1);
		}
	}
	
}
void FSkillTreeViewModel::PasteStatesAsChildrenFromClipboard(USkillExecutorConfig* ParentState)
{
	USkillExecutorEditorData * EditorData=SkillTreeEditorDataWeak.Get();
	if (EditorData==nullptr)
	{
		return;
	}
	CopySelectedStates();
	FString TextToImport;
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);
	const FScopedTransaction Transaction(LOCTEXT("PasteSkillConfigTransaction","Paste SkillConfig"));
	PasteStatesAsChildrenFromText(TextToImport, ParentState, INDEX_NONE);
}
void FSkillTreeViewModel::PasteStatesAsChildrenFromText(const FString& TextToImport, USkillExecutorConfig* ParentConfig,
	const int32 IndexToInsertAt)
{
	USkillExecutorEditorData * EditorData=SkillTreeEditorDataWeak.Get();
	if (EditorData==nullptr)
	{
		return;
	}
	UObject * Outer=ParentConfig?static_cast<UObject*>(ParentConfig):static_cast<UObject*>(EditorData);
	Outer->Modify();
	//反序列化获取文本当中的数据之后插入到技能树当中
	SkillExecutor::Editor::FSkillExecutorConfigTextFactory Factory;
	Factory.ProcessBuffer(Outer,RF_Transactional,TextToImport);
	TArray<TObjectPtr<USkillExecutorConfig>>& ParentArray = ParentConfig ? ParentConfig->Children : EditorData->SubTrees;
	const int32 TargetIndex = (IndexToInsertAt == INDEX_NONE) ? ParentArray.Num() : IndexToInsertAt;
	ParentArray.Insert(Factory.Configs, TargetIndex);

	//开始插入的操作
	TArray<USkillExecutorConfig * > NewSkillConfig;
	for (USkillExecutorConfig * Config:Factory.Configs)
	{
		SkillExecutor::Editor::FixSkillConfigAfterDuplication(Config,ParentConfig,NewSkillConfig);
	}
	for (USkillExecutorConfig *Config:NewSkillConfig)
	{
		OnStateAdded.Broadcast(Config->Parent,Config);
	}
}
void FSkillTreeViewModel::DuplicateSelectedStates()
{
	USkillExecutorEditorData * EditorData=SkillTreeEditorDataWeak.Get();
	if(EditorData==nullptr)
	{
		return;
	}
	//获取选中的状态并且移除里面涵盖的子类
	TArray<USkillExecutorConfig*>SkillExecutorConfigs;
	GetSelectedStates(SkillExecutorConfigs);
	SkillExecutor::Editor::RemoveContainedChildren(SkillExecutorConfigs);
	if (SkillExecutorConfigs.IsEmpty())
	{
		return;
	}
	//序列化乘string字符
	FString ExportedText=SkillExecutor::Editor::ExportConfigToText(EditorData,SkillExecutorConfigs);
	//将复制的所有内容放置到第一个config下,获取其在父类的子类数组的位置，复制所有内容到下一位
	USkillExecutorConfig * AfterConfig=SkillExecutorConfigs[0];
	const int32 index=AfterConfig->Parent?AfterConfig->Parent->Children.Find(AfterConfig):EditorData->SubTrees.Find(AfterConfig);
	if (index!=INDEX_NONE)
	{
		const FScopedTransaction Transaction(LOCTEXT("DuplicateStatesTransaction","Duplicate States"));
		PasteStatesAsChildrenFromText(ExportedText,AfterConfig->Parent,index+1);
	}
}

void FSkillTreeViewModel::MoveSelectedStatesBefore(USkillExecutorConfig* TargetConfig)
{
	MoveSelectedStates(TargetConfig,FSKillTreeViewModelInsert::Before);
}

void FSkillTreeViewModel::MoveSelectedStatesAfter(USkillExecutorConfig* TargetConfig)
{
	MoveSelectedStates(TargetConfig,FSKillTreeViewModelInsert::After);
}

void FSkillTreeViewModel::MoveSelectedStatesInto(USkillExecutorConfig* TargetConfig)
{
	MoveSelectedStates(TargetConfig,FSKillTreeViewModelInsert::After);
}

//控制State
void FSkillTreeViewModel::NotifyAssetChangedExternally() const
{
	OnAssetChanged.Broadcast();
}

void FSkillTreeViewModel::NotifyStatesChangedExternally(const TSet<USkillExecutorConfig*>& ChangedStates,
	const FPropertyChangedEvent& PropertyChangedEvent) const
{
	
	OnStatesChanged.Broadcast(ChangedStates,PropertyChangedEvent);
}

void FSkillTreeViewModel::MoveSelectedStates(USkillExecutorConfig* TargetConfig, FSKillTreeViewModelInsert Insert)
{
	USkillExecutorEditorData *EditorData=SkillTreeEditorDataWeak.Get();
	if (EditorData==nullptr||TargetConfig==nullptr)
	{
		return;
	}
	TArray<USkillExecutorConfig*> States;
	GetSelectedStates(States);
	SkillExecutor::Editor::RemoveContainedChildren(States);
	//不允许父类移动到子类
	States.RemoveAll([TargetConfig](const USkillExecutorConfig * Config)
	{
		return SkillExecutor::Editor::IsChildof(Config,TargetConfig);
	});
	if(States.Num()>0)
	{
		const FScopedTransaction Transaction(LOCTEXT("MoveTransaction","Move"));
		TSet<USkillExecutorConfig*> AffectedParents;
		TSet<USkillExecutorConfig*> AffectedStates;
		USkillExecutorConfig *TargetParent=TargetConfig->Parent;
		//这里设定插到前后都是同级，而插进去才是子集
		//这里是被插的父级添加影响
		if (Insert==FSKillTreeViewModelInsert::Into)
		{
			AffectedParents.Add(TargetConfig);
		}
		else
		{
			AffectedParents.Add(TargetParent);
		}
		//这里是被拆的父级也要添加影响
		for (int32 i=States.Num()-1;i>=0;i--)
		{
			if (USkillExecutorConfig *Config=States[i])
			{
				Config->Modify();
				if (Config->Parent)
				{
					AffectedParents.Add(Config->Parent);
				}
			}
		}
		for (USkillExecutorConfig * Parent:AffectedParents)
		{
			if (Parent)
			{
				Parent->Modify();
			}
			else
			{
				EditorData->Modify();
			}
		}
			// Add in reverse order to keep the original order.
		for (int32 i = States.Num() - 1; i >= 0; i--)
		{
			if (USkillExecutorConfig* SelectedState = States[i])
			{
				AffectedStates.Add(SelectedState);

				USkillExecutorConfig* SelectedParent = SelectedState->Parent;

				// Remove from current parent 从原来的父项移除
				TArray<TObjectPtr<USkillExecutorConfig>>& ArrayToRemoveFrom = SelectedParent ? SelectedParent->Children : EditorData->SubTrees;
				const int32 ItemIndex = ArrayToRemoveFrom.Find(SelectedState);
				if (ItemIndex != INDEX_NONE)
				{
					ArrayToRemoveFrom.RemoveAt(ItemIndex);
					SelectedState->Parent = nullptr;
				}

				// Insert to new parent 嵌入新的父项
				if (Insert == FSKillTreeViewModelInsert::Into)
				{
					// Into
					TargetConfig->Children.Insert(SelectedState, /*Index*/0);
					SelectedState->Parent = TargetConfig;
				}
				else
				{
					TArray<TObjectPtr<USkillExecutorConfig>>& ArrayToMoveTo = TargetParent ? TargetParent->Children : EditorData->SubTrees;
					const int32 TargetIndex = ArrayToMoveTo.Find(TargetConfig);
					if (TargetIndex != INDEX_NONE)
					{
						if (Insert == FSKillTreeViewModelInsert::Before)
						{
							// Before
							ArrayToMoveTo.Insert(SelectedState, TargetIndex);
							SelectedState->Parent = TargetParent;
						}
						else if (Insert == FSKillTreeViewModelInsert::After)
						{
							// After
							ArrayToMoveTo.Insert(SelectedState, TargetIndex + 1);
							SelectedState->Parent = TargetParent;
						}
					}
					else
					{
						// Fallback, should never happen.
						ensureMsgf(false, TEXT("%s: Failed to find specified target state %s on state %s while moving a state."), *GetNameSafe(EditorData->GetOuter()), *GetNameSafe(TargetConfig), *GetNameSafe(SelectedParent));
						ArrayToMoveTo.Add(SelectedState);
						SelectedState->Parent = TargetParent;
					}
				}
			}
		}

		OnStatesMoved.Broadcast(AffectedParents, AffectedStates);

		TArray<TWeakObjectPtr<USkillExecutorConfig>> WeakStates;
		for (USkillExecutorConfig* State : States)
		{
			WeakStates.Add(State);
		}

		SetSelection(WeakStates);
	}
}


#undef LOCTEXT_NAMESPACE
