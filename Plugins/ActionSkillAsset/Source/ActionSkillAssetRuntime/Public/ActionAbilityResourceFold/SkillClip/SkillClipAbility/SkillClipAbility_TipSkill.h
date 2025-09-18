// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillClipAbilityBase.h"
#include "SkillClipAbility_TipSkill.generated.h"

class UAbilityTask_PlayMontageAndWait;
/**
 * 这个类中在preAbility时即开始播放单击的动画并且将所有的委托都绑定到了结束技能，如果后续需要做一些额外任务请停止他的委托
 */
UCLASS()
class ACTIONSKILLASSETRUNTIME_API USkillClipAbility_TipSkill : public USkillClipAbilityBase
{
	GENERATED_BODY()
public:
	//当发起攻击时开始
	virtual void PreEntryAbility_Implementation() override;
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait * TipMontage;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UAnimMontage * TipAnimMontage;
	
};
