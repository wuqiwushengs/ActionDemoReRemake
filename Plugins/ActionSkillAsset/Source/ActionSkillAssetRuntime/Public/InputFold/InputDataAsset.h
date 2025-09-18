// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "InputDataAsset.generated.h"

enum class EInputWeight : uint8;
class UInputAction;

USTRUCT(BlueprintType)
struct FInputData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	FGameplayTag InputTag;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	UInputAction * InputAction;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	bool bAbilityInput;
	//这里设计的是在后面技能那里添加蓄力技能然后做判断所以这里不进行更改
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input",meta=(EditCondition="bAbilityInput"))
	EInputWeight InputType;
	//当输入状态改变成普通状态时如果还在按下的状态是否允许执行
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input",meta=(EditCondition="bAbilityInput"))
	bool AllowedInputActiveAfterInputStateChangeToNormal=false;
	bool IsValid()
	{
		return  InputTag.IsValid();
	}
};
/**
 * 
 */
UCLASS()
class ACTIONSKILLASSETRUNTIME_API UInputDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	//所有的输入内容
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InputSetting",meta=(TitleProperty="InputTag"))
	TArray<FInputData> NativeInputData;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="InputSetting",meta=(TitleProperty="InputTag"))
	TArray<FInputData> AbilityInputData;

	UFUNCTION()
	UInputAction * GetNativeInputActionByTag(FGameplayTag InputTag);
	UFUNCTION()
	TArray<FInputData> GetNativeInputActions();
	UFUNCTION()
	FInputData & GetNativeInputData(FGameplayTag InputTag);
	UFUNCTION()
	UInputAction * GetAbilityInputActionByTag(FGameplayTag InputTag);
	UFUNCTION()
	TArray<FInputData> GetAbilityInputActions();
     UFUNCTION()
	FInputData & GetAbilityInputData(FGameplayTag InputTag)  ;
	
};
