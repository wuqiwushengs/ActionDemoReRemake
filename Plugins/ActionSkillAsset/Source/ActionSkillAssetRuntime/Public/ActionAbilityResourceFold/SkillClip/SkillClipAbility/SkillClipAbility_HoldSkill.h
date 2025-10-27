// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillClipAbilityBase.h"
#include "ActionAbilityResourceFold/SkillClip/SpecialAbilityInterface.h"
#include "SkillClipAbility_HoldSkill.generated.h"

class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
//长按的内容：
UCLASS(Abstract)
class ACTIONSKILLASSETRUNTIME_API USkillClipAbility_HoldSkill : public USkillClipAbilityBase,public ISpecialAbilityInterface
{
	GENERATED_BODY()
public:
	
	USkillClipAbility_HoldSkill();
	//需要自己重写 
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	UAnimMontage * HoldMontagePlayedRule();
	virtual UAnimMontage * HoldMontagePlayedRule_Implementation();
	virtual void PreEntryAbility_Implementation() override;
	UFUNCTION(BlueprintCallable,BlueprintPure)
	virtual UAnimMontage * GetSelectedAnimMontage() override;
	UFUNCTION(BlueprintCallable,BlueprintPure)
	virtual int32 GetSelectedAnimMontageIndex() override;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FMultiAnimPlayed AnimPlayedRules;
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait * HoldMontagePlayTask;
	float HoldTime;
	UPROPERTY()
	UAnimMontage * PlayMontage;
};
