#pragma once
#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"
class SSkillTreeView;
class FSkillTreeViewModel;
class USkillExecutorDescriptorAsset;
struct FSkillExecutorDescriptor;
class USkillExecutorConfig;
class FSkillTreeViewDragDrop:public FDragDropOperation
{
public:
	DRAG_DROP_OPERATOR_TYPE(FSkillTreeViewDragDrop,FDragDropOperation)
	static TSharedRef<FSkillTreeViewDragDrop> New(const USkillExecutorConfig* InSkillConfig)
	{
		return MakeShareable(new FSkillTreeViewDragDrop(InSkillConfig));
	}
	const  USkillExecutorConfig* GetDraggedDescriptor() const { return SkillDescriptor; }
private:
	FSkillTreeViewDragDrop(const USkillExecutorConfig* InSkillConfig)
		:SkillDescriptor(InSkillConfig)
	{
	}
	const USkillExecutorConfig*  SkillDescriptor;
	
};
class ACTIONSKILLASSETEDITOR_API FSkillExecutorConfigEditor:public FAssetEditorToolkit,public FGCObject,public FEditorUndoClient
{
public:
	void OnPackageSaved(const FString& String, UPackage* Package, FObjectPostSaveContext ObjectPostSaveContext);
	
	FSkillExecutorConfigEditor();
	virtual ~FSkillExecutorConfigEditor() override;
	void InitSkillExecutorConfigEditor(const EToolkitMode::Type Mode,const TSharedPtr<IToolkitHost>&InitToolkitHost,USkillExecutorDescriptorAsset* ExecutorConfig);
	void RebuildSkillConfig() ;
	//ToolKit Implement
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	//GC
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual void SaveAsset_Execute() override;
	virtual FString GetReferencerName() const override;
	//TabRegister
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>&InTabManager) override;
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	void OnAssetFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args) ;
	
private:
	void HandleModelAssetChanged();
	void HandleModelSelectionChanged(const TArray<TWeakObjectPtr<USkillExecutorConfig>>& SelectedStates);
	void UpdateAsset();
	TSharedPtr<IDetailsView> SelectionDetailsView;
	FDelegateHandle OnPackageSavedDelegateHandle;
	TObjectPtr<USkillExecutorDescriptorAsset> EditingSkillExecutorDescriptorAsset;
	TSharedPtr<FSkillTreeViewModel> SkillTreeViewModel;
	TSharedRef<FUICommandList> TreeViewCommandList;
	TSharedPtr<SSkillTreeView> SkillTreeView;
};
