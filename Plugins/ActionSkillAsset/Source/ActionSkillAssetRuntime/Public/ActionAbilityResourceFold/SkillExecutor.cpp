// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillExecutor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "SkillManager.h"
#include "AbilitySystemFold/AbilitySystemCompoentRef/ActionAbilitySystemComponent.h"
#include "GamePlayTag/GamePlayTags.h"
#include "SkillClip/SkillClipBase.h"
#include "SkillClip/SkillClip_PlayMontage.h"
#include "SkillClip/SkillClipAbility/SkillClipAbilityBase.h"

UActionAbilitySystemComponent *USkillExecutor::GetOwnerAbilitySystemComponent()
{
	if (SkillManager.IsValid())
	{
		return SkillManager->GetOwnerAbilitySystemComponent();
	}
	return nullptr;
}
void USkillExecutor::SkillExecutorTickFunc(float DeltaTime)
{
	
}
TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>>  USkillExecutor::MakeClip(UClass* ClipClass)
{
	return  FSkillContainer::MakeVariantSkill(ClipClass);
}
void USkillExecutor::InitializeSkill(FSkillTitle InputSkill,USkillManager * inSkillManager)
{
	InitialSkillState(InputSkill,inSkillManager);
	if(SkillContext.bNeedHoldSkill && InputSkill.InputType==EInputType::Release)
	{
		HandleHoldSkillRelease();
		return;
	}
	//有前摇就开始播放前摇
	HandlePreTipSkill();
	HandleSkillTypes();
	PlayNextClipInCache();
	if(SkillManager->StartSkillDelegate.IsBound())
	{
		SkillManager->StartSkillDelegate.Broadcast(InputSkill.InputTag);
	}
}
void USkillExecutor::InitialSkillState(FSkillTitle InputSkill, USkillManager* inSkillManager)
{
	//记录当前的游戏时间和按键映射
	CurrentTime=InputSkill.InputTime;
	SkillContext.InitialSkillContext();
	InitalPressInfo=InputSkill;
	SkillManager=inSkillManager;
	IsActive=true;
}
void USkillExecutor::HandlePreTipSkill()
{
	LoadClipToQueue(SkillContext.GetPreSkill());
}
void USkillExecutor::HandleHoldSkillRelease()
{
	float IntervalTime=GWorld->GetTimeSeconds()-CurrentTime;
	SkillContext.HoldSkill.HoldSkillInfo.CurrentHoldTime=IntervalTime;
	switch (SkillContext.HoldSkill.HoldType)
	{
	case EPreHoldTimeType::Once:
		LoadClipToQueue( SkillContext.GetProcessSkill(HoldSkill),SkillContext.GetPostSkill(HoldSkill));
		break;
	case EPreHoldTimeType::Loop:
		if(SkillContext.HoldSkill.SkipHoldExecuteAfterRelease)
		{
			LodeSkillClip.Empty();
			LoadClipToQueue(SkillContext.GetPostSkill(HoldSkill));
		}
		break;
	}
	PlayNextClipInCache();
}
void USkillExecutor::HandleSkillTypes()
{
	//载入片段
	if(SkillContext.bShouldLoadTipSkillInitial())
	{
		//添加单击切片
		FinalSelectedSkillType=ESkillType::TipSkill;
		LoadClipToQueue(SkillContext.GetProcessSkill(TipSkill),SkillContext.GetPostSkill(TipSkill));
	}
	else if(SkillContext.bShouldLoadMultiTipSkill())
	{
		FinalSelectedSkillType=ESkillType::MultiTipSkill;
		LoadClipToQueue(SkillContext.GetProcessSkill(MultiTipSkill),SkillContext.GetPostSkill(MultiTipSkill));
	}
	else if(SkillContext.bShouldLoadHoldSkillInitial())
	{
		FinalSelectedSkillType=ESkillType::HoldSkill;
		LoadClipToQueue(SkillContext.GetProcessSkill(HoldSkill),SkillContext.GetPostSkill(HoldSkill));
	}
	
}
void USkillExecutor::InitializeAbility(UActionAbilitySystemComponent * OwnerAbilitySystemComponent)
{
	SkillContext.TryGiveAllAbility(OwnerAbilitySystemComponent);
}
//这个用来在各种输入的时候触发
void USkillExecutor::OnSkillTrigger(FSkillTitle TriggerInput)
{
	CurrentPressInfo=TriggerInput;
	//这里判断的是蓄力和连击存在的情况,当松开时如果时间太小那么就直接走连击如果时间大于最小时间那么就走蓄力
	//TODO::设置每个技能容器设置一个判断，用户只需要调用尝试更改技能即可
	if (CanProcessHoldSkillTrigger())
	{
		SkillContext.SetCurrentHoldTime(GWorld->GetTimeSeconds()-CurrentTime);
		//循环动画都不行并且有连击内容时走连击 不允许在once那里使用
		//这里为了方便后面理解因此不做优化，这里是判断是否能够结束蓄力直接到蓄力内容
		if(SkillContext.ShouldStopCurrentPreHoldSkillOnTrigger())
		{
			switch (SkillContext.HoldSkill.HoldType)
			{
			case EPreHoldTimeType::Once:
				StopCurrentSkill();
				UE_LOG(LogTemp,Warning,TEXT("StopSkill"));
				return;
			case EPreHoldTimeType::Loop:
				if(SkillContext.HoldSkill.SkipHoldExecuteAfterRelease)
				{
					LodeSkillClip.Empty();
					LoadClipToQueue(SkillContext.GetPostSkill(HoldSkill));
				}
				StopCurrentSkill();
				return;
			}
		}
		//如果蓄力内容失败之后再来尝试连打的
		if(CanProcessMultiSkillTrigger())
		{
			FinalSelectedSkillType=ESkillType::MultiTipSkill;
			LodeSkillClip.Empty();
			LoadClipToQueue(SkillContext.GetProcessSkill(MultiTipSkill),SkillContext.GetPostSkill(MultiTipSkill));
			StopCurrentSkill();
			return;
		}
	}
	//当连击情况按压时,激活连击的Tag
	if (CanProcessMultiSkillTapTrigger())
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwnerAbilitySystemComponent()->GetOwnerActor(),GamePlayTags::MultiTipSign,FGameplayEventData());
		return;
	}
}


bool USkillExecutor::CanProcessHoldSkillTrigger()
{
	return  CurrentPressInfo==InitalPressInfo&& SkillContext.bNeedHoldSkill&&!SkillContext.DoOnceAttackRelease &&CurrentPressInfo.InputType==EInputType::Release &&FinalSelectedSkillType==ESkillType::HoldSkill;
}
bool USkillExecutor::CanProcessMultiSkillTrigger()
{
	return  SkillContext.bNeedMultiTipSkill &&SkillContext.HoldSkill.HoldType!=EPreHoldTimeType::Once;
}
bool USkillExecutor::CanProcessMultiSkillTapTrigger()
{
	return  CurrentPressInfo.InputTag==InitalPressInfo.InputTag &&SkillContext.bNeedMultiTipSkill && CurrentPressInfo.InputType==EInputType::Press && FinalSelectedSkillType==ESkillType::MultiTipSkill;
}

void USkillExecutor::InterruptExecution()
{
	EndSkill();
}
void USkillExecutor::OnClipEnd()
{
	SkillContext.DoOnceAttackRelease=true;
	if(!PlayNextClipInCache())
	{
		EndSkill();
	}
}

void USkillExecutor::PlayClip(TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> &&Clip,FName BindFunction)
{
	
	if (Clip.IsType<TSubclassOf<USkillClip_PlayMontage>>())
	{
		PlayedMontage=nullptr;
		if(Clip.TryGet<TSubclassOf<USkillClip_PlayMontage>>()) return;
		PlayedMontage=NewObject<USkillClip_PlayMontage>(this, Clip.Get<TSubclassOf<USkillClip_PlayMontage>>());
		PlayedMontage->Initialize(GetOwnerAbilitySystemComponent(),this);
		if (PlayedMontage)
		{
			PlayedMontage->AbilityEvent.BindUFunction(this,BindFunction);
			PlayedMontage->OnEnterThisClip();
			LastExeClip=PlayedMontage;
		}
		PlayedAbilitySpecHandle=FGameplayAbilitySpecHandle();
		LastExeAbilitySpecHandle=FGameplayAbilitySpecHandle();
	}
	if (Clip.IsType<TSubclassOf<USkillClipAbilityBase>>())
	{
		FGameplayAbilitySpec * Spec=GetOwnerAbilitySystemComponent()->FindAbilitySpecFromClass(Clip.Get<TSubclassOf<USkillClipAbilityBase>>());
		if (Spec)
		{
			GetOwnerAbilitySystemComponent()->TryActivateAbilityByClass(Clip.Get<TSubclassOf<USkillClipAbilityBase>>());
			PlayedAbilitySpecHandle=Spec->Handle;
			LastExeAbilitySpecHandle=Spec->Handle;
		}
		PlayedMontage=nullptr;
		LastExeClip=nullptr;
	}
}

void USkillExecutor::EndSkill()
{
	LodeSkillClip.Empty();
	InitalPressInfo=FSkillTitle();
	CurrentPressInfo=FSkillTitle();
	FinalSelectedSkillType=ESkillType::None;
	CurrentTime=0.0;
	SkillContext.InitialSkillContext();
	LastExeClip=nullptr;
	LastExeAbilitySpecHandle=FGameplayAbilitySpecHandle();
	if (PlayedMontage)
	{
		PlayedMontage->OnExitThisClip();
		PlayedMontage=nullptr;
	}
	if (PlayedAbilitySpecHandle.IsValid())
	{
		GetOwnerAbilitySystemComponent()->CancelAbilityHandle(PlayedAbilitySpecHandle);
		PlayedAbilitySpecHandle=FGameplayAbilitySpecHandle();
	}
	IsActive=false;
	if(SkillManager->EndSkillDelegate.IsBound())
	{
		SkillManager->EndSkillDelegate.Broadcast(CurrentPressInfo.InputTag);
	}
}
void USkillExecutor::StopCurrentSkill()
{
	if (PlayedMontage)
	{
		PlayedMontage->OnExitThisClip_Implementation();
	}
	else
	{
		GetOwnerAbilitySystemComponent()->CancelAbilityHandle(PlayedAbilitySpecHandle);
	}
}
USkillManager* USkillExecutor::GetSkillManager()
{
	if(SkillManager.Get())
	{
		return SkillManager.Get();
	}
	return nullptr;
}


