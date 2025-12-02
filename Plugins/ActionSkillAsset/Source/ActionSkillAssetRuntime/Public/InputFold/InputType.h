#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputInfo/InputInfo.h"
#include  "InputType.generated.h"

USTRUCT(BlueprintType)
struct FAbilityInputInfo
{
	GENERATED_BODY()
	FAbilityInputInfo(){};
	FAbilityInputInfo(FGameplayTag InInputTag):InputTag(InInputTag){};
	FAbilityInputInfo(FGameplayTag InTag,float InInputWordTime=0,float InInputIntervalTime=0,EInputWeight InputWeight=EInputWeight::Weight0)
		:InputTag(InTag),InputWorldTime(InInputWordTime),InputIntervalTime(InInputIntervalTime),InputWeight(InputWeight){};
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag;
	UPROPERTY()
	float InputWorldTime=0.0f;
	UPROPERTY()
	float InputIntervalTime=0.0f;
	UPROPERTY()
	EInputWeight InputWeight=EInputWeight::Weight0;
};


UENUM(BlueprintType)
enum  class EInputState:uint8
{
	PreInputState,
	NormalInputState,
	DisableInputState,
};

//用于存储角色的各种输入数据用来进行记录
USTRUCT(BlueprintType)
struct FCharacterNormalInputData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FVector2D MoveInputValue;
	UPROPERTY(BlueprintReadOnly)
	FVector2D LookInputValue;
	
};
