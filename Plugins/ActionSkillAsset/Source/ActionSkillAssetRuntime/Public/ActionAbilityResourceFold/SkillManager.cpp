// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillManager.h"
#include "AbilitySystemFold/AbilitySystemCompoentRef/ActionAbilitySystemComponent.h"
#include "SkillExecutorFold/ExecutorCondition.h"

UActionAbilitySystemComponent * USkillManager::GetOwnerAbilitySystemComponent() const 
{
	return  AbilitySystemComponent;
}
void USkillManager::Initialize(UActionAbilitySystemComponent * AbilitySystem)
{
	this->AbilitySystemComponent=AbilitySystem;
	GetOwnerAbilitySystemComponent()->OnStateTagChanged.AddUObject(this,&USkillManager::OnStateTagChanged);
	GetOwnerAbilitySystemComponent()->OnInputStateChanged.AddUObject(this,&USkillManager::OnInputStateChanged);
	EndSkillDelegate.BindUObject(this,&USkillManager::ClearSelectedSSkillExecutorConfig);
	SelectedSkillExecutorConfig=nullptr;
	AutoSkillCheck.Empty();
	
	BindAbility();
	if (bCheckAutoChildrenSkill=CheckSkillExecutorHaveAutoChildSkillFromRoot();bCheckAutoChildrenSkill)
	{
		FindCorrectSkillConfigsFromRoot(AutoSkillCheck,ESkillReleaseType::Auto);
	}
}

void USkillManager::BindAbility()
{
	for (TPair<FGameplayTag,USkillExecutorDescriptorAsset *> Asset: SkillAssetSum->SKillExecutorAssets)
	{
		for(USkillExecutorConfig * Config :Asset.Value->ExecutorConfigs)
		{
			TraverseSkillExecutorConfig(Config);
		}
	}
}
void USkillManager::TraverseSkillExecutorConfig(USkillExecutorConfig* Config)
{
	if (!Config)
	{
		return;
	}
	// 添加当前节点到结果数组
	Config->ExecutorDescriptor.Executor->InitializeAbility(GetOwnerAbilitySystemComponent());
	// 遍历所有子节点
	for (TObjectPtr<USkillExecutorConfig> Child : Config->Children)
	{
		TraverseSkillExecutorConfig(Child);
	}
	
}
void USkillManager::Tick(float DeltaTime)
{
	if (bCheckAutoChildrenSkill)
	{
		UpdateAutoSkillCheck();
	}
}

bool USkillManager::IsTickable() const
{
	return true;
}

bool USkillManager::IsTickableInEditor() const
{
	return false;
}

bool USkillManager::IsTickableWhenPaused() const
{
	return false;
}

TStatId USkillManager::GetStatId() const
{
	return TStatId();
}

//寻找到可以执行的技能并且将自动技能检测改为false然后开启执行这个技能
void USkillManager::UpdateAutoSkillCheck()
{
	if (AutoSkillCheck.Num() > 0)
	{
		USkillExecutorConfig *	FinalConfig=nullptr;
		for (auto Config : AutoSkillCheck)
		{
			if (Config->ExecutorDescriptor.CanExecute(GetAbilityOwnedTag()))
			{
				if(Config->ExecutorDescriptor.Condition)
				{
					if(!Config->ExecutorDescriptor.Condition->CanTransition(GetOwnerAbilitySystemComponent()->GetActionPlayerCharacter(),GetOwnerAbilitySystemComponent(),this)) continue;
				}
				if(!FinalConfig)
				{FinalConfig=Config;}
				else
				{
					FinalConfig=FinalConfig->ExecutorDescriptor.OwnerRequestTag.Num()<Config->ExecutorDescriptor.OwnerRequestTag.Num()? Config:FinalConfig;
				}
			}
		}
		if(FinalConfig)
		{
			bCheckAutoChildrenSkill=false;	
		   StartSelectedSkillConfig(FinalConfig,FSkillTitle());
			UE_LOG(LogTemp,Warning,TEXT("Execute Skill"))
		}
		return;
	}
	bCheckAutoChildrenSkill=false;
}

void USkillManager::ClearSelectedSSkillExecutorConfig(FGameplayTag SkillTag)
{
	//在结束的时候重新检测
	if (bCheckAutoChildrenSkill=CheckSkillExecutorHaveAutoChildSkillFromRoot();bCheckAutoChildrenSkill)
	{
		FindCorrectSkillConfigsFromRoot(AutoSkillCheck,ESkillReleaseType::Auto);
	}
}
FGameplayTag USkillManager::GetCurrentStateTag() const 
{
	
	return  GetOwnerAbilitySystemComponent()?GetOwnerAbilitySystemComponent()->CurrentStateTag:FGameplayTag();
}
FGameplayTagContainer USkillManager::GetAbilityOwnedTag() const 
{
   return GetOwnerAbilitySystemComponent()?GetOwnerAbilitySystemComponent()->GetOwnedGameplayTags():FGameplayTagContainer();
}

bool  USkillManager::TurnToNextSkillExecutor(ESkillReleaseType SkillTriggerType,const FSkillTitle & InputInfo)
{	
	//执行对应开始的函数
		if (USkillExecutorConfig * SelectedSkill=FindNextSkillExecutor(SkillTriggerType,InputInfo);SelectedSkill)
		{
			StartSelectedSkillConfig(SelectedSkill,InputInfo);
			return	true;
		}
		else
		{
			//如果尝试的时候为空的情况
			UE_LOG(LogTemp, Warning, TEXT("Can't find Correct Skill"));
			//进行自动技能的追踪
			SelectedSkillExecutorConfig=nullptr;
			if (bCheckAutoChildrenSkill=CheckSkillExecutorHaveAutoChildSkillFromRoot();bCheckAutoChildrenSkill)
			{
				FindCorrectSkillConfigsFromRoot(AutoSkillCheck,ESkillReleaseType::Auto);
			}
		};
	return false;
}
//寻找的都是可以执行的而不是仅仅符合技能释放条件的
USkillExecutorConfig* USkillManager::FindNextSkillExecutor(ESkillReleaseType SkillTriggerType,const FSkillTitle & InputInfo) const 
{
	USkillExecutorConfig * SelectedSkill=nullptr;
	if (SelectedSkillExecutorConfig)
	{	//先寻找子类
		SelectedSkill=FindCanExecuteSkillExecutorFromChildren(SkillTriggerType,SelectedSkillExecutorConfig,InputInfo);
	}
	if (!SelectedSkill)
	{	//再寻找Root
		FindCanExecuteSkillConfigFromRoot(SelectedSkill,SkillTriggerType,InputInfo);
	}
	return SelectedSkill;
}
USkillExecutorConfig * USkillManager::FindCanExecuteSkillExecutorFromChildren(ESkillReleaseType SkillTriggerType, USkillExecutorConfig* ParentConfig,const FSkillTitle & InputInfo) const
{
	USkillExecutorConfig * NeededConfig=nullptr;
	//检查当前技能是不是当前这个技能资产里面的
	USkillExecutorConfig * RootConfig=ParentConfig->FindRootSkillExecutorConfig(ParentConfig);
	UActionAbilitySystemComponent * ActionAbilitySystemCompoent=GetOwnerAbilitySystemComponent();
	USkillExecutorDescriptorAsset * Asset=*SkillAssetSum->SKillExecutorAssets.Find(ActionAbilitySystemCompoent->CurrentStateTag);
	bool bFind=Asset->ExecutorConfigs.Contains(RootConfig);
	if(!bFind) return NeededConfig;
	//检查其下面有咩有能够执行的
	if (ParentConfig && ParentConfig->Children.Num() > 0)
	{
		for (auto SkillConfig:ParentConfig->Children)
		{
			if(SkillConfig->ExecutorDescriptor.Condition)
			{
				if(!SkillConfig->ExecutorDescriptor.Condition->CanTransition(GetOwnerAbilitySystemComponent()->GetActionPlayerCharacter(),GetOwnerAbilitySystemComponent(),this)) continue;
			}
			if (SkillConfig->ExecutorDescriptor.ReleaseType==SkillTriggerType && SkillConfig->ExecutorDescriptor.CanExecute(GetAbilityOwnedTag()) && SkillConfig->ExecutorDescriptor.TriggeredTag==InputInfo.InputTag)
			{
					if (!NeededConfig || NeededConfig->ExecutorDescriptor.SkillWeight>SkillConfig->ExecutorDescriptor.SkillWeight )
					{
						NeededConfig=SkillConfig;
					}
			}
		}
	}
	return NeededConfig;
}
void USkillManager::FindCanExecuteSkillConfigFromRoot(USkillExecutorConfig*& NeededConfig,ESkillReleaseType TriggerType,const FSkillTitle & InputInfo) const 
{
	NeededConfig=nullptr;
	UActionAbilitySystemComponent * ActionAbilitySystemCompoent=GetOwnerAbilitySystemComponent();
	USkillExecutorDescriptorAsset * Asset=*SkillAssetSum->SKillExecutorAssets.Find(ActionAbilitySystemCompoent->CurrentStateTag);
	if (!Asset) return;
	//查找正确的状态并返回
	for (USkillExecutorConfig * ExecutorConfig : Asset->ExecutorConfigs)
	{   if(ExecutorConfig->ExecutorDescriptor.Condition)
	{
		if(!ExecutorConfig->ExecutorDescriptor.Condition->CanTransition(GetOwnerAbilitySystemComponent()->GetActionPlayerCharacter(),GetOwnerAbilitySystemComponent(),this)) continue;
	}
		if(TriggerType==ExecutorConfig ->ExecutorDescriptor.ReleaseType&&ExecutorConfig ->ExecutorDescriptor.CanExecute(GetAbilityOwnedTag())&& ExecutorConfig ->ExecutorDescriptor.TriggeredTag==InputInfo.InputTag)
		{
			NeededConfig=ExecutorConfig;
			break;
		}
	}
}
//寻找的是只要符合技能释放条件的 现在改成专门用在Auto的
void USkillManager::FindCorrectSkillConfigsFromRoot(TArray<USkillExecutorConfig*>& NeededConfig, ESkillReleaseType TriggerType) const 
{
	NeededConfig.Empty();
	if (!SkillAssetSum) return;
	USkillExecutorDescriptorAsset * Assets=*SkillAssetSum->SKillExecutorAssets.Find(GetOwnerAbilitySystemComponent()->CurrentStateTag);
	if (Assets->ExecutorConfigs.Num()<=0) return;
	for (USkillExecutorConfig * Config:Assets->ExecutorConfigs)
	{
		if (Config->ExecutorDescriptor.ReleaseType==TriggerType)
		{
			//不会在执行某个自动时再执行自身
			if(SelectedSkillExecutorConfig &&SelectedSkillExecutorConfig->ExecutorDescriptor.Executor->IsActive && Config==SelectedSkillExecutorConfig)  continue;
			NeededConfig.Add(Config);
		}
	}

}
void USkillManager::FindCorrectSkillConfigFromChildren(USkillExecutorConfig* Parent, TArray<USkillExecutorConfig*>& Skills,ESkillReleaseType TriggerType)
{
	Skills.Empty();
	if (!Parent) return;
	for (auto Child: Parent->Children)
	{
		if (Child->ExecutorDescriptor.ReleaseType==TriggerType && Child !=SelectedSkillExecutorConfig)
		{
			Skills.Add(Child);
		}
	}
}
void USkillManager::FindCorrectSkillConfigsFromRootAndParent(TArray<USkillExecutorConfig*>& NeededConfig,USkillExecutorConfig* ParentConfig, ESkillReleaseType TriggerType)
{
	TArray<USkillExecutorConfig *> RootNeededConfig;
	FindCorrectSkillConfigsFromRoot(RootNeededConfig,TriggerType);
	int32 index=RootNeededConfig.Find(ParentConfig->FindRootSkillExecutorConfig(ParentConfig));
	if (index!=INDEX_NONE)
	{
		RootNeededConfig.RemoveAt(index);
	}
	TArray<USkillExecutorConfig *> ChildNeededConfig;
	FindCorrectSkillConfigFromChildren(ParentConfig,ChildNeededConfig,ESkillReleaseType::Auto);
	NeededConfig.Append(RootNeededConfig);
	NeededConfig.Append(ChildNeededConfig);
}

//开始技能
void USkillManager::StartSelectedSkillConfig(USkillExecutorConfig* SelectedConfig,const FSkillTitle & InputInfo)
{
	check(SelectedConfig);
	ShutdownCurrentSkillExecutor();
	SelectedSkillExecutorConfig=SelectedConfig;
	//设置自动技能检测
	TArray<USkillExecutorConfig *> AutoSkillConfigs;
	FindCorrectSkillConfigsFromRootAndParent(AutoSkillConfigs,SelectedConfig,ESkillReleaseType::Auto);
	if (bCheckAutoChildrenSkill=AutoSkillConfigs.Num()>0;bCheckAutoChildrenSkill)
	{
		AutoSkillCheck=AutoSkillConfigs;
	}
	//执行前面所选的技能
	SelectedConfig->ExecutorDescriptor.Executor->InitializeSkill(InputInfo,this);
}
void USkillManager::ShutdownCurrentSkillExecutor()
{
	if (SelectedSkillExecutorConfig)
	{
		SelectedSkillExecutorConfig->ExecutorDescriptor.Executor->InterruptExecution();
		//执行时停止避免同时起效
		bCheckAutoChildrenSkill=false;
		AutoSkillCheck.Empty();
		SelectedSkillExecutorConfig=nullptr;
	}
}
bool USkillManager:: CheckExecutorHaveAutoChildSkillFromSelectedSkill() const 
{
	//检查子分支有没有，如果没有的话那就检查一下Root
	if (SelectedSkillExecutorConfig && SelectedSkillExecutorConfig->Children.Num()>0)
	{
		TObjectPtr<USkillExecutorConfig>SkillExecutorConfig=*SelectedSkillExecutorConfig->Children.FindByPredicate([](const USkillExecutorConfig * ChildrenConfig)
		{
				return ChildrenConfig->ExecutorDescriptor.ReleaseType==ESkillReleaseType::Auto;
		});
		return SkillExecutorConfig?true:false;
	}
	TArray<USkillExecutorConfig *> SkillExecutorConfig;
	FindCorrectSkillConfigsFromRoot(SkillExecutorConfig,ESkillReleaseType::Auto);
	return SkillExecutorConfig.Num()>0;

}

bool USkillManager::CheckSkillExecutorHaveAutoChildSkillFromRoot() const 
{
	USkillExecutorDescriptorAsset * Assets=*SkillAssetSum->SKillExecutorAssets.Find(GetOwnerAbilitySystemComponent()->CurrentStateTag);
	if (!Assets) return false;
	for (USkillExecutorConfig * Config:Assets->ExecutorConfigs)
	{
		if (Config->ExecutorDescriptor.ReleaseType==ESkillReleaseType::Auto)
		{
			return true;
		}
	}
	return false;
}
void USkillManager::StopAutoCheckChildrenSkillDirectly()
{
	bCheckAutoChildrenSkill=false;
}
//当状态切换时切换能够进行过度的技能
void USkillManager::OnStateTagChanged(FGameplayTag LastGameplayTag, FGameplayTag NewGameplayTag)
{
	bCheckAutoChildrenSkill=CheckSkillExecutorHaveAutoChildSkillFromRoot();
	SelectedSkillExecutorConfig=nullptr;
	if (bCheckAutoChildrenSkill)
	{
		FindCorrectSkillConfigsFromRoot(AutoSkillCheck,ESkillReleaseType::Auto);
	}
	AutoSkillCheck.Empty();
}

void USkillManager::OnInputStateChanged(EInputState OldState, EInputState NewState)
{
	if (NewState!=EInputState::NormalInputState) return;
	bCheckAutoChildrenSkill=CheckSkillExecutorHaveAutoChildSkillFromRoot();
	SelectedSkillExecutorConfig=nullptr;
	if (bCheckAutoChildrenSkill)
	{
		FindCorrectSkillConfigsFromRoot(AutoSkillCheck,ESkillReleaseType::Auto);
	}
	AutoSkillCheck.Empty();
}
