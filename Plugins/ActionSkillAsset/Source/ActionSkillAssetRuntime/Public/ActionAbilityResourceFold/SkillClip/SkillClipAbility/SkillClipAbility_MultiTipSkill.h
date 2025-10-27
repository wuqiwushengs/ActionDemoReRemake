// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillClipAbilityBase.h"
#include "ActionAbilityResourceFold/SkillClip/SpecialAbilityInterface.h"
#include "SkillClipAbility_MultiTipSkill.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
/**
 * 多次连续点击技能。
 */
UCLASS(Abstract)
class ACTIONSKILLASSETRUNTIME_API USkillClipAbility_MultiTipSkill : public USkillClipAbilityBase,public ISpecialAbilityInterface
{
	GENERATED_BODY()
public:
	USkillClipAbility_MultiTipSkill();
	virtual void PreEntryAbility_Implementation() override;
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	UAnimMontage *	MultiTipMontagePlayedRule();
	virtual UAnimMontage * MultiTipMontagePlayedRule_Implementation();
	UFUNCTION(BlueprintCallable,BlueprintPure)
	virtual UAnimMontage * GetSelectedAnimMontage() override;
	UFUNCTION(BlueprintCallable,BlueprintPure)
	virtual int32 GetSelectedAnimMontageIndex() override;
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait * HoldMontagePlayTask;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FMultiAnimPlayed AnimPlayedRules;
	UPROPERTY()
	UAnimMontage * PlayMontage;
	//累计连击数
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent * MultiTipEvent;
	int32 TipCount;
	//这里指要点多少下
	UPROPERTY(EditDefaultsOnly)
	int32 MinTipIndex;
	UPROPERTY(EditDefaultsOnly)
	//0 =无限 最大连击次数
	int32 MaxPlayTime=0;
	int32 CurrenPlayTime=0;
	UFUNCTION()
	void AddTipCount(FGameplayEventData Payload);
	UFUNCTION()
	bool ChoosePlayAgain_orEndAbility();
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
};
