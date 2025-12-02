// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGlobalAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GolablAttributeSimpleExecution.generated.h"

/**
 * 
 */
struct FHealthStatics
{
	FHealthStatics()
	{
		HealthAttribute=FGameplayEffectAttributeCaptureDefinition(UActionGlobalAttributeSet::GetCurrentHealthAttribute(),EGameplayEffectAttributeCaptureSource::Target,true);
		DamageAttribute=FGameplayEffectAttributeCaptureDefinition(UActionGlobalAttributeSet::GetAttackValueAttribute(),EGameplayEffectAttributeCaptureSource::Source,true);
		DefendAttribute=FGameplayEffectAttributeCaptureDefinition(UActionGlobalAttributeSet::GetDefendValueAttribute(),EGameplayEffectAttributeCaptureSource::Target,true);
		AttackTypeAttribute=FGameplayEffectAttributeCaptureDefinition(UActionGlobalAttributeSet::GetAttackTypeValueAttribute(),EGameplayEffectAttributeCaptureSource::Source,true);
	}
	FGameplayEffectAttributeCaptureDefinition HealthAttribute;
	FGameplayEffectAttributeCaptureDefinition DamageAttribute;
	FGameplayEffectAttributeCaptureDefinition DefendAttribute;
	FGameplayEffectAttributeCaptureDefinition AttackTypeAttribute;
	
	
};
static  FHealthStatics & HealthStatic()
{
	static FHealthStatics HealthInfo;
	return HealthInfo;
}
UCLASS()
class ACTIONATTRIBUTESYSTEMRUNTIME_API UGolablAttributeSimpleExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	UGolablAttributeSimpleExecution();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
