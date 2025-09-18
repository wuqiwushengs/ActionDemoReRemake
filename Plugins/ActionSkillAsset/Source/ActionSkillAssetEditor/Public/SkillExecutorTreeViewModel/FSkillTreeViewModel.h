#pragma once
#include "ActionSkillAssetRuntime/Public/ActionAbilityResourceFold/SkillExecutorFold/SkillExecutorDescriptor.h"

class USkillExecutorConfig;
class USkillExecutorEditorData;
enum class FSKillTreeViewModelInsert : uint8
{
	Before,
	After,
	Into,
};

class FSkillTreeViewModel:public FEditorUndoClient,public  TSharedFromThis<FSkillTreeViewModel>
{
public:
	DECLARE_MULTICAST_DELEGATE(FOnAssetChanged);
	//这里是不同的状态更新时需要调整父类或者在ui上添加子类
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatesChanged, const TSet<USkillExecutorConfig*>& /*AffectedStates*/, const FPropertyChangedEvent& /*这个变量用于记录变量的修改信息*/);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStateAdded, USkillExecutorConfig* /*ParentState*/, USkillExecutorConfig* /*NewState*/);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnStatesRemoved, const TSet<USkillExecutorConfig*>& /*AffectedParents*/);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatesMoved, const TSet<USkillExecutorConfig*>& /*AffectedParents*/, const TSet<USkillExecutorConfig*>& /*MovedStates*/);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSelectionChanged, const TArray<TWeakObjectPtr<USkillExecutorConfig>>& /*SelectedStates*/);
	FSkillTreeViewModel();
	virtual ~FSkillTreeViewModel() override;
	void HandleIdentifierChanged(const USkillExecutorDescriptorAsset& SkillExecutorDescriptorAsset);
	void Init(USkillExecutorEditorData * InTreeData);
	//返回整个技能树
	const USkillExecutorDescriptorAsset * GetSkillTree();
	//返回所有子树的信息。
	TArray<TObjectPtr<USkillExecutorConfig>>* GetSubTrees() const;
	int GetSubTreeCount() const;
	void GetSubTrees(TArray<TWeakObjectPtr<USkillExecutorConfig>> & OutSubTrees) const; 
	//FEditorUndo
	virtual void PostRedo(bool bSuccess) override;
	virtual void PostUndo(bool bSuccess) override;
	//SelectionFunction
	void ClearSelection();
	void SetSelection(USkillExecutorConfig *Selected);
	//用引用传递，避免不必要的拷贝
	void SetSelection(const TArray<TWeakObjectPtr<USkillExecutorConfig>> & InSelection);
	bool IsSelected(const USkillExecutorConfig * Config) const ;
	bool IsChildofSelection(const USkillExecutorConfig * Config);
	void GetSelectedStates(TArray<USkillExecutorConfig * >& OutSelectedSkillDescription);
	void GetSelectedStates(TArray<TWeakObjectPtr<USkillExecutorConfig>>& OutSelectedSkillDescription);
	bool HasSelected() const ;
	//用于在打开资产时获取数据并且将数据应用于ui展示当中。
	void SetPersistentExpandedStates(TSet<TWeakObjectPtr<USkillExecutorConfig>> &InExpandedStates);
	void GetPersistentExpandedStates(TSet<TWeakObjectPtr<USkillExecutorConfig>> &OutExpandedStates);
	void GetExpandedStatesRecursive(USkillExecutorConfig * Config,TSet<TWeakObjectPtr<USkillExecutorConfig>>& OutExpandedStates);

	//控制State的函数
	void AddState(USkillExecutorConfig * AfterState);
	void AddChildState(USkillExecutorConfig * ParentState);
	void RenameState(USkillExecutorConfig * State,FName NewName);
	void RemoveSelectedStates();
	void CopySelectedStates();
	bool CanPasteStatesFromClipBoard();
	void PasteStatesFromClipboard(USkillExecutorConfig * AfterConfig);
	void PasteStatesAsChildrenFromClipboard(USkillExecutorConfig * ParentState);
	void PasteStatesAsChildrenFromText(const FString & TextToImport,USkillExecutorConfig * ParentConfig,const int32 IndexToInsertAt);
	void DuplicateSelectedStates();
	void MoveSelectedStatesBefore(USkillExecutorConfig * TargetConfig);
	void MoveSelectedStatesAfter(USkillExecutorConfig * TargetConfig);
	void MoveSelectedStatesInto(USkillExecutorConfig * TargetConfig);
	
	
	//直接更新视口的函数
	void NotifyAssetChangedExternally() const ;
	void NotifyStatesChangedExternally(const TSet<USkillExecutorConfig*>& ChangedStates,const FPropertyChangedEvent & PropertyChangedEvent) const ;
	//委托
	FOnAssetChanged& GetOnAssetChanged() { return OnAssetChanged; }
	FOnStatesChanged& GetOnStatesChanged() { return OnStatesChanged; }
	FOnStateAdded& GetOnStateAdded() { return OnStateAdded; }
	FOnStatesRemoved& GetOnStatesRemoved() { return OnStatesRemoved; }
	FOnStatesMoved& GetOnStatesMoved() { return OnStatesMoved; }
	FOnSelectionChanged& GetOnSelectionChanged() { return OnSelectionChanged; }
protected:
	void MoveSelectedStates(USkillExecutorConfig * Config,FSKillTreeViewModelInsert Insert);
	TWeakObjectPtr<USkillExecutorEditorData> SkillTreeEditorDataWeak;
	TSet<TWeakObjectPtr<USkillExecutorConfig>> SelectedSkillExecutorConfigs;
	FOnAssetChanged OnAssetChanged;
	FOnStateAdded OnStateAdded;
	FOnStatesChanged OnStatesChanged;
	FOnStatesRemoved OnStatesRemoved;
	FOnStatesMoved OnStatesMoved;
	FOnSelectionChanged OnSelectionChanged;
	
};
