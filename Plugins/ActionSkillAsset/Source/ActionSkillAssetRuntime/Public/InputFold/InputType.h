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
	float InputWorldTime;
	UPROPERTY()
	float InputIntervalTime;
	UPROPERTY()
	EInputWeight InputWeight;
};


UENUM(BlueprintType)
enum  class EInputState:uint8
{
	PreInputState,
	NormalInputState,
	DisableInputState,
};
