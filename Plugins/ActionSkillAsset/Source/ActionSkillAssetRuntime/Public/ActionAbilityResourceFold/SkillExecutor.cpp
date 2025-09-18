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

void USkillExecutor::SkillExecutorTickFunc()
{
	//目前只需要处理一个蓄力时间的问题。
	
	
}

void USkillExecutor::InitializeSkill(FSkillTitle InputSkill,USkillManager * inSkillManager)
{
	UE_LOG(LogTemp,Warning,TEXT("%s StartInitial Skill"),*GetName());
	//记录当前的游戏时间和按键映射
	CurrentTime=InputSkill.InputTime;
	InitalPressInfo=InputSkill;
	SkillManager=inSkillManager;
	IsActive=true;
	if(bNeedHoldSkill && InputSkill.InputType==EInputType::Release)
	{
		TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Clip;
		Clip=FSkillContainer::MakeVariantSkill(HoldSkill.HoldSkillClass.GetCurrentSkill());
		PlayClip(Clip,TEXT("OnClipEnd"));
		if (HoldSkill.PostHoldContent.IsValid())
		{
			TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Post;
			Post=FSkillContainer::MakeVariantSkill(HoldSkill.PostHoldContent.GetCurrentSkill());
			LodeSkillClip.Enqueue(Post);
		}
		return;
	}
	if (PreTipContent.IsValid())
	{
		//有前摇就开始播放前摇
		TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Clip;
		Clip=FSkillContainer::MakeVariantSkill(PreTipContent.GetCurrentSkill());
		PlayClip(Clip,TEXT("OnPreClipEnd"));
		//载入片段
		if (!bNeedHoldSkill)
		{	//如果需要连击
			if (bNeedMultiTipSkill)
			{
				//添加单击切片
				FinalSelectedSkillType=ESkillType::MultiTipSkill;
				TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Multi;
				Multi=FSkillContainer::MakeVariantSkill(MultiTipSkill.MultiTipSkillClass.GetCurrentSkill());
				LodeSkillClip.Enqueue(Multi);
				if (MultiTipSkill.PostMultiTipContent.IsValid())
				{
					TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Post;
					Post=FSkillContainer::MakeVariantSkill(MultiTipSkill.PostMultiTipContent.GetCurrentSkill());
					LodeSkillClip.Enqueue(Post);
				}
			}
			//如果只有单击
			else if(bNeedTipSkill)
			{
				//添加单击切片
				FinalSelectedSkillType=ESkillType::TipSkill;
				TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Tip;
				Tip=FSkillContainer::MakeVariantSkill(TipSkill.TipSkillClass.GetCurrentSkill());
				LodeSkillClip.Enqueue(Tip);
				if (TipSkill.PostTipContent.IsValid())
				{
					TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Post;
					Post=FSkillContainer::MakeVariantSkill(TipSkill.PostTipContent.GetCurrentSkill());
					LodeSkillClip.Enqueue(Post);
				}
				
			}
		}
		else
		{
			FinalSelectedSkillType=ESkillType::HoldSkill;
			//如果有蓄力并且当前技能时单次时
			if (HoldSkill.HoldType==EPreHoldTimeType::Once)
			{
				TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Hold;
				Hold=FSkillContainer::MakeVariantSkill(HoldSkill.HoldSkillClass.GetCurrentSkill());
				LodeSkillClip.Enqueue(Hold);
				if (HoldSkill.PostHoldContent.IsValid())
				{
					TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Post;
					Post=FSkillContainer::MakeVariantSkill(HoldSkill.PostHoldContent.GetCurrentSkill());
					LodeSkillClip.Enqueue(Post);
				}
			}
		}
		
	}
	//没有前摇的情况只有在单击时
	else if(bNeedTipSkill)
	{
		FinalSelectedSkillType=ESkillType::TipSkill;
		TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Tip;
		Tip=FSkillContainer::MakeVariantSkill(TipSkill.TipSkillClass.GetCurrentSkill());
		PlayClip(Tip,TEXT("OnClipEnd"));
		if (TipSkill.PostTipContent.IsValid())
		{
			TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Post;
			Post=FSkillContainer::MakeVariantSkill(TipSkill.PostTipContent.GetCurrentSkill());
			LodeSkillClip.Enqueue(Post);
		}
		
	}
	SkillManager->StartSkillDelegate.ExecuteIfBound(InputSkill.InputTag);
	
}

void USkillExecutor::InitializeAbility(UActionAbilitySystemComponent * OwnerAbilitySystemComponent)
{
	if(PreTipContent.IsValid()&& PreTipContent.SelectedSkill==ESkillCLipType::Ability)
	{
		FGameplayAbilitySpec Spec(PreTipContent.AbilitySkill);
		OwnerAbilitySystemComponent->GiveAbility(Spec);
	}
	if(bNeedTipSkill )
	{
		if(	TipSkill.PostTipContent.IsValid()&& TipSkill.PostTipContent.SelectedSkill==ESkillCLipType::Ability)
	{
		FGameplayAbilitySpec Spec(TipSkill.PostTipContent.AbilitySkill);
		OwnerAbilitySystemComponent->GiveAbility(Spec);
	}
		if(	TipSkill.TipSkillClass.IsValid()&& TipSkill.TipSkillClass.SelectedSkill==ESkillCLipType::Ability)
		{
			FGameplayAbilitySpec Spec(TipSkill.TipSkillClass.AbilitySkill);
			OwnerAbilitySystemComponent->GiveAbility(Spec);
		}
	}
	if(bNeedHoldSkill)
	{
		if(	HoldSkill.HoldSkillClass.IsValid()&& HoldSkill.HoldSkillClass.SelectedSkill==ESkillCLipType::Ability)
		{
			FGameplayAbilitySpec Spec(HoldSkill.HoldSkillClass.AbilitySkill);
			OwnerAbilitySystemComponent->GiveAbility(Spec);
		}
		if(	HoldSkill.PostHoldContent.IsValid()&& HoldSkill.PostHoldContent.SelectedSkill==ESkillCLipType::Ability)
		{
			FGameplayAbilitySpec Spec(HoldSkill.PostHoldContent.AbilitySkill);
			OwnerAbilitySystemComponent->GiveAbility(Spec);
		}
	}
	if(bNeedMultiTipSkill)
	{
		if(	MultiTipSkill.MultiTipSkillClass.IsValid()&& MultiTipSkill.MultiTipSkillClass.SelectedSkill==ESkillCLipType::Ability)
		{
			FGameplayAbilitySpec Spec(MultiTipSkill.MultiTipSkillClass.AbilitySkill);
		OwnerAbilitySystemComponent->GiveAbility(Spec);
		}
		if(	MultiTipSkill.PostMultiTipContent.IsValid()&& MultiTipSkill.PostMultiTipContent.SelectedSkill==ESkillCLipType::Ability)
		{
			FGameplayAbilitySpec Spec(MultiTipSkill.PostMultiTipContent.AbilitySkill);
			OwnerAbilitySystemComponent->GiveAbility(Spec);
		}
	}
}

//这个输入
void USkillExecutor::OnSkillTrigger(FSkillTitle TriggerInput)
{
	
	CurrentPressInfo=TriggerInput;
	UE_LOG(LogTemp,Warning,TEXT(" initial %s post %s"),*InitalPressInfo.SkillInputId.ToString(),*TriggerInput.SkillInputId.ToString())
	//这里判断的是蓄力和连击存在的情况,当松开时如果时间太小那么就直接走连击如果时间大于最小时间那么就走蓄力
	if (CurrentPressInfo==InitalPressInfo&& bNeedHoldSkill &&CurrentPressInfo.InputType==EInputType::Release &&FinalSelectedSkillType==ESkillType::HoldSkill)
	{	float IntervalTime=GWorld->GetTimeSeconds()-CurrentTime;
		HoldSkill.HoldSkillInfo.CurrentHoldTime=IntervalTime;
		//不根据这个时间来判断,判断部分写在动画播放哪里
		if ((!bNeedMultiTipSkill ||(bNeedMultiTipSkill && IntervalTime>=HoldSkill.HoldSkillInfo.MinHoldTime) )&& HoldSkill.HoldType==EPreHoldTimeType::Loop)
		{
			UE_LOG(LogTemp,Warning,TEXT("%s OnTriggeredSkill To Hold Loop"),*GetName());
			TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> HoldAbility;
			HoldAbility=FSkillContainer::MakeVariantSkill(HoldSkill.HoldSkillClass.GetCurrentSkill());
			LodeSkillClip.Enqueue(HoldAbility);
			if (HoldSkill.PostHoldContent.IsValid())
			{
				TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Post;
				Post=FSkillContainer::MakeVariantSkill(HoldSkill.PostHoldContent.GetCurrentSkill());
				LodeSkillClip.Enqueue(Post);
			}
			StopCurrentSkill(FSkillContainer::MakeVariantSkill(PreTipContent.GetCurrentSkill()));
			return;
		}
		//当为不为循环动画时根据当前的情况释放，要保证这个最小释放的时间要比蓄力阶段动画时间短。
		if (!bNeedMultiTipSkill && HoldSkill.HoldType==EPreHoldTimeType::Once && IntervalTime<=HoldSkill.HoldSkillInfo.MinHoldTime)
		{
			StopCurrentSkill(FSkillContainer::MakeVariantSkill(PreTipContent.GetCurrentSkill()));
			return;
		}
		//循环动画都不行并且有连击内容时走连击 不允许在once那里使用
		if(bNeedMultiTipSkill &&HoldSkill.HoldType!=EPreHoldTimeType::Once)
		{
			TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> MulityAbility;
			MulityAbility=FSkillContainer::MakeVariantSkill(MultiTipSkill.MultiTipSkillClass.GetCurrentSkill());
			LodeSkillClip.Enqueue(MulityAbility);
			if (HoldSkill.PostHoldContent.IsValid())
			{
				TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Post;
				Post=FSkillContainer::MakeVariantSkill(MultiTipSkill.PostMultiTipContent.GetCurrentSkill());
				LodeSkillClip.Enqueue(Post);
			}
			FinalSelectedSkillType=ESkillType::MultiTipSkill;
			StopCurrentSkill(FSkillContainer::MakeVariantSkill(PreTipContent.GetCurrentSkill()));
			return;
		}
	}
	//当连击情况按压时,激活连击的Tag
	else if (CurrentPressInfo.InputTag==InitalPressInfo.InputTag && bNeedMultiTipSkill && CurrentPressInfo.InputType==EInputType::Press && FinalSelectedSkillType==ESkillType::MultiTipSkill)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwnerAbilitySystemComponent()->GetOwnerActor(),GamePlayTags::MultiTipSign,FGameplayEventData());
		GEngine->AddOnScreenDebugMessage(-1,1,FColor::Black,TEXT("Mulit Tag Press"));
		return;
	}
}

void USkillExecutor::InterruptExecution()
{
	ResetSkill();
	UE_LOG(LogTemp,Warning,TEXT("%s be Interupt"),*GetName());
}

void USkillExecutor::OnPreClipEnd()
{
	TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Out;
	bool SuccessGet=LodeSkillClip.Dequeue(Out);
	if (SuccessGet)
	{
		PlayClip(Out,TEXT("OnClipEnd"));
	}
}
void USkillExecutor::OnClipEnd()
{
	TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Out;
	if (LodeSkillClip.Dequeue(Out))
	{
		PlayClip(Out,TEXT("OnClipEnd"));
	}
	else
	{
		ResetSkill();
	}
}
void USkillExecutor::PlayClip(TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Clip,FName BindFunction)
{
	if (Clip.IsType<TSubclassOf<USkillClip_PlayMontage>>())
	{
		PlayedMontage=nullptr;
		PlayedMontage=NewObject<USkillClip_PlayMontage>(this,Clip.Get<TSubclassOf<USkillClip_PlayMontage>>());
		PlayedMontage->Initialize(GetOwnerAbilitySystemComponent(),this);
		if (PlayedMontage)
		{
			PlayedMontage->AbilityEvent.BindUFunction(this,BindFunction);
			PlayedMontage->OnEnterThisClip();
		}
		PlayedAbilitySpecHandle=FGameplayAbilitySpecHandle();
	}
	if (Clip.IsType<TSubclassOf<USkillClipAbilityBase>>())
	{
		FGameplayAbilitySpec * Spec=GetOwnerAbilitySystemComponent()->FindAbilitySpecFromClass(Clip.Get<TSubclassOf<USkillClipAbilityBase>>());
		if (Spec)
		{
			GetOwnerAbilitySystemComponent()->TryActivateAbilityByClass(Clip.Get<TSubclassOf<USkillClipAbilityBase>>());
			PlayedAbilitySpecHandle=Spec->Handle;
		}
		PlayedMontage=nullptr;
	}
}

void USkillExecutor::ResetSkill()
{
	LodeSkillClip.Empty();
	InitalPressInfo=FSkillTitle();
	CurrentPressInfo=FSkillTitle();
	FinalSelectedSkillType=ESkillType::None;
	CurrentTime=0.0;
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
	SkillManager->EndSkillDelegate.ExecuteIfBound(CurrentPressInfo.InputTag);
}

void USkillExecutor::StopCurrentSkill(
	TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Clip)
{
	if (Clip.IsType<TSubclassOf<USkillClip_PlayMontage>>())
	{
		PlayedMontage->OnExitThisClip_Implementation();
	}
	if (Clip.IsType<TSubclassOf<USkillClipAbilityBase>>())
	{
		GetOwnerAbilitySystemComponent()->CancelAbilityHandle(PlayedAbilitySpecHandle);
	}
}


