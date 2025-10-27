// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ActionAbilityResourceFold/SkillClipTypeCheck.h"
#include "ActionAbilityResourceFold/SkillTypes/SkillTypes.h"
#include "SkillClipAbilityBase.generated.h"

class USkillExecutor;
/**
 * 作为Skill技能的基类需要配合SkillClip使用，不能单独使用。所有的ability类执行的都是攻击那一刻的内容这里不播放前摇后摇，作为对之前Ability的改进
 */
UCLASS(Abstract)
class ACTIONSKILLASSETRUNTIME_API USkillClipAbilityBase : public UGameplayAbility,public ISkillClipTypeCheck
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual  ESkillCLipType GetSkillClipType() override;
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void PreEntryAbility();
	virtual void PreEntryAbility_Implementation();
	//进入Ability时调用不使用原生的ActivateAbility
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void OnEntryAbility();
	virtual void OnEntryAbility_Implementation();
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void OnEndAbility();
	virtual void OnEndAbility_Implementation();
	virtual FEndAbilityEvent& GetAbilitySignal() override;
	UFUNCTION(BlueprintCallable,BlueprintPure)
	float GetHoldMinTime();
	UFUNCTION(BlueprintCallable,BlueprintPure)
	float GetCurrentHoldTime();
	UFUNCTION(BlueprintCallable,BlueprintPure)
	const  USkillExecutor * GetExecutor();
	UPROPERTY()
	FEndAbilityEvent AbilityEvent;
protected:
	UPROPERTY()
	const USkillExecutor * Executor;
	bool bIsExiting = false;
};
