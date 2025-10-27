// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "SkillExecutor.h"
#include "InputFold/InputType.h"
#include "SkillExecutorFold/SkillExecutorDescriptor.h"
#include "UObject/Object.h"
#include "SkillManager.generated.h"
class USkillExecutorDescriptorAssetSum;
class UActionAbilitySystemComponent;
/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FSkillStateDelegate,FGameplayTag)

UCLASS()
class ACTIONSKILLASSETRUNTIME_API USkillManager : public UObject ,public FTickableGameObject
{
	GENERATED_BODY()
public:
	//Tick
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;
	UFUNCTION()
	UActionAbilitySystemComponent* GetOwnerAbilitySystemComponent()  const ;
	void OnStateTagChanged(FGameplayTag  LastGameplayTag, FGameplayTag NewGameplayTag);
	void OnInputStateChanged(EInputState OldState, EInputState  NewState);
	void Initialize(UActionAbilitySystemComponent * AbilitySystem);
	void BindAbility();
	void TraverseSkillExecutorConfig(USkillExecutorConfig* Config);
	//用于保存连段
	UPROPERTY(BlueprintReadWrite)
	mutable  bool bUsePreSelectedSkill=false;
	UPROPERTY()
	UActionAbilitySystemComponent * AbilitySystemComponent;
	FSkillStateDelegate  StartSkillDelegate;
	FSkillStateDelegate	EndSkillDelegate;
	//被选择的Executor
	UPROPERTY(Transient,BlueprintReadOnly)
	USkillExecutorConfig * SelectedSkillExecutorConfig;
	UPROPERTY(Transient,BlueprintReadOnly)
	USkillExecutorConfig * LastSelectedSkillExecutorConfig;
	void ClearSelectedSSkillExecutorConfig(FGameplayTag SkillTag);
	//获取当前的状态Tag,即倒地，在地面，在空中这种能过被动出现的，不要把防御，攻击这种状态放在这里，这是为了筛选使用哪一个技能树。
	UFUNCTION()
	FGameplayTag GetCurrentStateTag() const ;
	UFUNCTION()
	FGameplayTagContainer GetAbilityOwnedTag() const ;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkillExecutorDescriptorAssetSumLink> SkillAssetSumLink;
	UPROPERTY(Transient)
	TObjectPtr<USkillExecutorDescriptorAssetSum> SelectedSkillAssetSum;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FGameplayTag SkillAssetLinkTag;
	//链接的SkillAsset改变的Delegate
	FSkillStateDelegate SkillAssetChangeDelegate;
	UFUNCTION(BlueprintCallable)
	void ChangeSkillAssetLinkTag(FGameplayTag SkillAssetTag);
	//直接停止自动查询auto类型的技能
	void  StopAutoCheckChildrenSkillDirectly();
	//技能能够向下搜寻
	bool TurnToNextSkillExecutor(ESkillReleaseType SkillTriggerType,const FSkillTitle  & InputInfo);
	void UpdateAutoSkillCheck();
	UPROPERTY(Transient)
	TArray<USkillExecutorConfig * > AutoSkillCheck;
	private:
	
#pragma  region SkillFindAndExecute
	//寻找下一个技能
	USkillExecutorConfig *  FindNextSkillExecutor(ESkillReleaseType SkillTriggerType,const FSkillTitle & InputInfo) const ;
	//从开头寻找对应的技能
	//查询是否在被选择的技能中有自动技能子类 如果找不到就从Root去找
	bool CheckExecutorHaveAutoChildSkillFromSelectedSkill() const ;
	//查询在Root中是否有符合的自动技能子类
	bool CheckSkillExecutorHaveAutoChildSkillFromRoot() const;
	//寻找对应的可以直接执行的子类技能
	USkillExecutorConfig * FindCanExecuteSkillExecutorFromChildren(ESkillReleaseType SkillTriggerType,USkillExecutorConfig * ParentConfig,const FSkillTitle & InputInfo) const;
	//查找可以执行的技能
	void FindCanExecuteSkillConfigFromRoot(USkillExecutorConfig * & NeededConfig,ESkillReleaseType TriggerType,const FSkillTitle & InputInfo) const ;
	//查找符合释放条件的技能 但是这三个目前内部编写原因最实用于Auto  如果要寻找可以执行的技能请用上面两个
	void FindCorrectSkillConfigsFromRoot(TArray<USkillExecutorConfig *> & NeededConfig,ESkillReleaseType TriggerType) const ;
	void FindCorrectSkillConfigFromChildren(USkillExecutorConfig * Parent,TArray<USkillExecutorConfig*>& Skills,ESkillReleaseType TriggerType);
	void FindCorrectSkillConfigsFromRootAndParent(TArray<USkillExecutorConfig *>& NeededConfig,USkillExecutorConfig * ParentConfig,ESkillReleaseType TriggerType);
	//开始执行某个技能
	void StartSelectedSkillConfig(USkillExecutorConfig * SelectedConfig,const FSkillTitle &InputInfo);
	void SetSelectedSkillConfig(USkillExecutorConfig *	InSkillConfig);
	UFUNCTION(BlueprintCallable)
	void ShutdownCurrentSkillExecutor();
	bool bCheckAutoChildrenSkill=false;
#pragma  endregion
};