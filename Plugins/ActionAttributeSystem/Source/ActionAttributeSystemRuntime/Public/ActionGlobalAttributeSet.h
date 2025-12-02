// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "ActionGlobalAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS_BASIC(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class ACTIONATTRIBUTESYSTEMRUNTIME_API UActionGlobalAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UActionGlobalAttributeSet():CurrentHealth(100.0f),MaxHealth(100.0f),DefendValue(1.0f),AttackValue(10.0f),AttackTypeValue(1.0f){};
	UPROPERTY(BlueprintReadWrite)
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS_BASIC(UActionGlobalAttributeSet,CurrentHealth);
	UPROPERTY(BlueprintReadWrite)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS_BASIC(UActionGlobalAttributeSet,MaxHealth);
	UPROPERTY(BlueprintReadWrite)
	FGameplayAttributeData  DefendValue;
	ATTRIBUTE_ACCESSORS_BASIC(UActionGlobalAttributeSet,DefendValue)
	UPROPERTY(BlueprintReadWrite)
	FGameplayAttributeData  AttackValue;
	ATTRIBUTE_ACCESSORS_BASIC(UActionGlobalAttributeSet,AttackValue);
	UPROPERTY(BlueprintReadWrite)
	FGameplayAttributeData AttackTypeValue;
	ATTRIBUTE_ACCESSORS_BASIC(UActionGlobalAttributeSet,AttackTypeValue);
	
	virtual   void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual  void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void AdjustForMaxAttributeDataChange(FGameplayAttributeData & CurrentAttributeData ,const  FGameplayAttributeData & MaxAttributeData,float NewMaxValue ,const  FGameplayAttribute &AffectedAttribute);
	
};
