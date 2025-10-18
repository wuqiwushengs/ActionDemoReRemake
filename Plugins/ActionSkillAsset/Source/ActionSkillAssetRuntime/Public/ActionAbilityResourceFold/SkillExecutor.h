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
public:
	UFUNCTION()
	UActionAbilitySystemComponent *GetOwnerAbilitySystemComponent();
	
	void SkillExecutorTickFunc(float DeltaTime);
	UFUNCTION(BlueprintCallable)
	void InitializeSkill(FSkillTitle InputSkill,USkillManager * inSkillManager);
	void InitializeAbility(UActionAbilitySystemComponent * OwnerAbilitySystemComponent);
	void OnSkillTrigger(FSkillTitle  TriggerInput);
	//外部调用
	UFUNCTION(BlueprintCallable)
	void InterruptExecution();
	UFUNCTION()
	void OnPreClipEnd();
	UFUNCTION()
	void OnClipEnd();
	
private:
	void PlayClip(TVariant<TSubclassOf<USkillClip_PlayMontage>,TSubclassOf<USkillClipAbilityBase>> Clip,FName BindFunction);
	void ResetSkill();
	void StopCurrentSkill(TVariant<TSubclassOf<USkillClip_PlayMontage>,TSubclassOf<USkillClipAbilityBase>> Clip);
	UPROPERTY()
	TWeakObjectPtr<USkillManager> SkillManager;
	UFUNCTION(BlueprintCallable,BlueprintPure)
	USkillManager *GetSkillManager();
public:
	#pragma region Skill
	UPROPERTY(EditAnywhere)
	bool  bCanSavetoOffset=true;
	UPROPERTY(EditAnywhere)
	bool bNeedTipSkill=false;
	//不需要所有都加内容，只需要加所需要的即可，单击和其他技能相互冲突，不需要一起添加
	UPROPERTY(EditAnywhere)
	FSkillContainer PreTipContent;
	UPROPERTY(EditAnywhere,meta=(EditCondition="bNeedTipSkill"))
	FTipSkill TipSkill;
	UPROPERTY(EditAnywhere)
	bool bNeedHoldSkill=false;
	UPROPERTY(EditAnywhere,meta=(EditCondition="bNeedHoldSkill"))
	FHoldSkill HoldSkill;
	UPROPERTY(EditAnywhere)
	bool bNeedMultiTipSkill=false;
	UPROPERTY(EditAnywhere,meta=(EditCondition="bNeedMultiTipSkill"))
	FMultiTipSkill MultiTipSkill;
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

	//前一个切片或者是Ability
	UPROPERTY(BlueprintReadOnly)
	USkillClip_PlayMontage*  LastExeClip;
	UPROPERTY(BlueprintReadOnly)
	FGameplayAbilitySpecHandle LastExeAbilitySpecHandle;
	float CurrentTime;
	bool IsActive=false;
	
};
