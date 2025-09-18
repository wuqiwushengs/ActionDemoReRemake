// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionAbilityResourceFold/SkillClipTypeCheck.h"
#include "ActionAbilityResourceFold/SkillTypes/SkillTypes.h"
#include "UObject/Object.h"
#include "SkillClipBase.generated.h"

class USkillExecutor;
class UActionAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class ACTIONSKILLASSETRUNTIME_API USkillClipBase : public UObject,public ISkillClipTypeCheck
{
	GENERATED_BODY()
public:
	USkillClipBase();
	void Initialize(UActionAbilitySystemComponent * InAbilitySystemComponent,USkillExecutor * InExecutor);
	UActionAbilitySystemComponent *GetAbilitySystemComponent();
	virtual ESkillCLipType GetSkillClipType() override;
	virtual FEndAbilityEvent & GetAbilitySignal() override;
	//Executor调用这个
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void OnEnterThisClip();
	virtual void OnEnterThisClip_Implementation();
	//通过这个返回到Executor的执行结果
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void OnExitThisClip();
	virtual void OnExitThisClip_Implementation();
	UPROPERTY()
	FEndAbilityEvent AbilityEvent;
	bool bIsExiting = false;
private:
	UPROPERTY()
	UActionAbilitySystemComponent * AbilitySystemComponent;
	UPROPERTY()
	const USkillExecutor * Executor;
	
};
