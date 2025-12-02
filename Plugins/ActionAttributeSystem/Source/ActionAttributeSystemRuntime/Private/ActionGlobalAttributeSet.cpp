// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGlobalAttributeSet.h"

#include "AttributeSystemTags.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "Interface/GlobalAttributeInterface.h"

void UActionGlobalAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if(Attribute==GetMaxHealthAttribute())
	{
		//保证Current其在0——Max内容 保证NewValue最起码是0
		NewValue=FMath::Max(NewValue,0);
		AdjustForMaxAttributeDataChange(CurrentHealth,MaxHealth,NewValue,GetCurrentHealthAttribute());
	}
	if(Attribute==GetCurrentHealthAttribute())
	{
		NewValue=FMath::Max(NewValue,0);
	}
}

void UActionGlobalAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if(Data.EvaluatedData.Attribute==GetCurrentHealthAttribute())
	{
		FGameplayTagContainer AssetTag;
		Data.EffectSpec.GetAllAssetTags(AssetTag);
		//当有受伤信息时才能调用此内容
		if(Data.Target.GetOwnerActor()->GetClass()->ImplementsInterface(UGlobalAttributeInterface::StaticClass())&&AssetTag.HasTag(AttributeSystemTags::Hurt))
		{
			IGlobalAttributeInterface::Execute_OnAttributeChanged(Data.Target.GetOwnerActor(),CurrentHealth);
		}
	}
}

void UActionGlobalAttributeSet::AdjustForMaxAttributeDataChange(FGameplayAttributeData & CurrentAttributeData,const FGameplayAttributeData& MaxAttributeData,
	float NewMaxValue, const FGameplayAttribute& AffectedAttribute)
{
	UAbilitySystemComponent * AbilitySystemComponent=GetOwningAbilitySystemComponent();
	const float MaxValue=MaxAttributeData.GetCurrentValue();
	if(!FMath::IsNearlyEqual(MaxValue,NewMaxValue)&&AbilitySystemComponent)
	{
		const float CurrentValue=CurrentAttributeData.GetCurrentValue();
		float NewDeltaValue=CurrentValue-FMath::Clamp(0,NewMaxValue,CurrentValue);
		AbilitySystemComponent->ApplyModToAttributeUnsafe(AffectedAttribute,EGameplayModOp::Additive,NewDeltaValue);
	}
}

