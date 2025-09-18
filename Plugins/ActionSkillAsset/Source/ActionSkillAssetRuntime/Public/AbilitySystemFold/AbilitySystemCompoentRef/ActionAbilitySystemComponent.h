// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "InputAction.h"
#include "ActionAbilityResourceFold/SkillTypes/SkillTypes.h"
#include "InputFold/InputType.h"
#include "ActionAbilitySystemComponent.generated.h"


class UInputDataAsset;
class USkillManager;
struct FInputActionInstance;
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStateTagChanged, FGameplayTag/* Old Tag*/, FGameplayTag/*NewTag*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInputStateChanged, EInputState/*Old State*/,EInputState /*New State*/);

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FTimeLockDelegate,const FInputActionInstance&,InpuActiondata, UInputDataAsset *,InputDataAsset,FGameplayTag, InputTag);
DECLARE_DYNAMIC_DELEGATE(FTimerUnlockDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FInputExecuteDelegate,const FAbilityInputInfo &,Inputinfo);
UCLASS(ClassGroup=(Action))
class ACTIONSKILLASSETRUNTIME_API UActionAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UActionAbilitySystemComponent();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	UInputDataAsset * GetInputDataAsset();
	void  AbilityInputDataLocalProcessing(const FInputActionInstance& InputInfo ,FGameplayTag InputData, UInputDataAsset * InputDataAsset,ETriggerEvent TriggerEvent);
	void OnAbilityInputStart(const FInputActionInstance& InputInfo,FGameplayTag InputTag, UInputDataAsset * InputDataAsset);
	void OnAbilityInputEnd(const FInputActionInstance& InputInfo,FGameplayTag InputTag, UInputDataAsset * InputDataAsset);
	void OnTurnNormalInputToCheckStillHasPressSkill(EInputState OldState ,EInputState NewState);
	UFUNCTION(BlueprintCallable,BlueprintPure)
	USkillManager * GetSkillManager();
#pragma  region State
	UPROPERTY()
	FGameplayTag CurrentStateTag;
	UFUNCTION(Blueprintable)
	void SetCurrentStateTag(FGameplayTag NewStateTag);
	FOnStateTagChanged OnStateTagChanged;
#pragma  endregion
#pragma region Input
	/*设置获取当前的输入状态  输入状态分类 预输入 普通输入 禁止输入*/
	UFUNCTION(BlueprintCallable,BlueprintPure)
	EInputState GetCurrentInputState();
	void  SetCurrentInputState(EInputState NewInputState);
	FOnInputStateChanged OnInputStateChanged;
	/*检查输入时间是否超过缓冲时间由此决定是否继续向缓冲区添加*/
	bool CheckInputLengthToSetInputLock(float InputLength);
	void SetInputLock(const FInputActionInstance &ActionInstance,UInputDataAsset * InputDataAsset,TArray<FAbilityInputInfo> & AbilityInputInfos,int32 index);
	void SetInputUnLock();
	void CheckFinalInputAndCheckExecute();
	bool FindExecuteAbilityInputInfo(const TArray<FAbilityInputInfo> & InputTagsBuff,FAbilityInputInfo &OutInputInfo);
	UPROPERTY(EditDefaultsOnly)
	float AbilityInputBuffTime=0.0f;
	FTimerHandle FinalInputTimer;
	/*输入锁委托和输入解锁的委托*/
	UPROPERTY()
	FTimeLockDelegate InputLockDelegate;
	UPROPERTY()
	FTimerUnlockDelegate InputUnlockDelegate;
	UPROPERTY()
	FInputExecuteDelegate InputExecuteDelegate;
	/* 预输入内容 预输入为0 就是不可以预输入的内容*/ 
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag,int> AllowedPreInputTag;
	void SetPreInputDisable(const FGameplayTagContainer & DisableTagContainer);
	//将预输入内容回归为1
	void TurnPreInputToDefault();
	bool CheckIsAllowed(FGameplayTag InputTag);
	void OnPreSkillExecute(FGameplayTag ExeTag, int Count);
	UFUNCTION(BlueprintCallable)
	void OnInputFinal(const FAbilityInputInfo & InputInfo);
	void OnPreAbilityEnd(FGameplayTag InputTag);
#pragma	endregion
private:
	TMap<FGameplayTag,FSkillTitle> InputDataMap;
	TMap<FGameplayTag,FSkillTitle> PreInputDataMap;
	EInputState CurrentInputState=EInputState::NormalInputState;
	EInputState LastInputState=EInputState::NormalInputState;
	TArray<FAbilityInputInfo> InputTagsInBuff;
	UPROPERTY(EditDefaultsOnly)
	USkillManager * SkillManager;
};
