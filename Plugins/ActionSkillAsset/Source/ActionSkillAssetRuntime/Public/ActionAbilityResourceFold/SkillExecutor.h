// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "SkillTypes/SkillTypes.h"
#include "UObject/Object.h"
#include "SkillExecutor.generated.h"

class USkillManager;
class USkillClipAbilityBase;
class USkillClipBase;
struct FTipSkill;
class UActionAbilitySystemComponent;
/**
 * 
 */
UCLASS(Blueprintable,EditInlineNew)
class ACTIONSKILLASSETRUNTIME_API USkillExecutor : public UObject
{
	GENERATED_BODY()
	friend class USkillManager;
public:
	UFUNCTION()
	UActionAbilitySystemComponent *GetOwnerAbilitySystemComponent();
	
	
	UFUNCTION(BlueprintCallable)
	void InitializeSkill(FSkillTitle InputSkill,USkillManager * inSkillManager);
	void InitialSkillState(FSkillTitle InputSkill,USkillManager * inSkillManager);
	void HandlePreTipSkill();
	void HandleHoldSkillRelease();
	void HandleSkillTypes();
	void InitializeAbility(UActionAbilitySystemComponent * OwnerAbilitySystemComponent);
	void OnSkillTrigger(FSkillTitle  TriggerInput);
	//这两个都是在Hold阶段判断是否能够处理这两个技能
	bool CanProcessHoldSkillTrigger();
	bool CanProcessMultiSkillTrigger();
	//当开始连击时的判断
	bool CanProcessMultiSkillTapTrigger();
	//外部调用
	UFUNCTION(BlueprintCallable)
	void InterruptExecution();
	UFUNCTION()
	void OnClipEnd();
	bool CanSavetoOffset()
	{
		  return SkillContext.bCanSavetoOffset;
	};
	static TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> MakeClip(UClass * ClipClass);
private:
	void SkillExecutorTickFunc(float DeltaTime);
	void PlayClip(TVariant<TSubclassOf<USkillClip_PlayMontage>,TSubclassOf<USkillClipAbilityBase>> &&Clip,FName BindFunction);
	void EndSkill();
	void StopCurrentSkill();
	UPROPERTY()
	TWeakObjectPtr<USkillManager> SkillManager;
	UFUNCTION(BlueprintCallable,BlueprintPure)
	USkillManager *GetSkillManager();
public:
	#pragma region Skill
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FSkillContext SkillContext;
#pragma endregion
public:
	//用来记录蓄力的按键信息,应用在蓄力时
	UPROPERTY()
	FSkillTitle InitalPressInfo;
	//随时按下时的按键信息
	UPROPERTY()
	FSkillTitle CurrentPressInfo;
	//用来记录最终所选择的技能
	ESkillType FinalSelectedSkillType=ESkillType::None;
	//初始化技能
	UPROPERTY()
	USkillClip_PlayMontage* PlayedMontage;
	UPROPERTY()
	FGameplayAbilitySpecHandle PlayedAbilitySpecHandle;
	TQueue<TVariant<TSubclassOf<USkillClip_PlayMontage>,TSubclassOf<USkillClipAbilityBase>>> LodeSkillClip;
	template<typename  T>
	void LoadClipToQueue(T arg);
	template<typename T,typename... Args>
	void LoadClipToQueue(T first,Args... rest);
	bool  PlayNextClipInCache()
	{
		TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Out;
		if (LodeSkillClip.Dequeue(Out))
		{
			PlayClip(MoveTemp(Out),TEXT("OnClipEnd"));
			return true;
		}
		return false;
	}
	//前一个切片或者是Ability
	UPROPERTY(BlueprintReadOnly)
	USkillClip_PlayMontage*  LastExeClip;
	UPROPERTY(BlueprintReadOnly)
	FGameplayAbilitySpecHandle LastExeAbilitySpecHandle;
	float CurrentTime;
	bool IsActive=false;
	
};


template <typename T>
void USkillExecutor::LoadClipToQueue(T arg)
{
	if(arg)
	{	auto Clip=MakeClip(arg);
		LodeSkillClip.Enqueue(Clip);
	}
}
template <typename T, typename ... Args>
void USkillExecutor::LoadClipToQueue(T first, Args... rest)
{
	static_assert(std::is_same_v<T,UClass*>);
	LoadClipToQueue(first);
	LoadClipToQueue(rest...);
}
