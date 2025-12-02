// Fill out your copyright notice in the Description page of Project Settings.


#include "ExecutionCalculation/GolablAttributeSimpleExecution.h"

UGolablAttributeSimpleExecution::UGolablAttributeSimpleExecution()
{
	RelevantAttributesToCapture.Add(HealthStatic().DamageAttribute);
	RelevantAttributesToCapture.Add(HealthStatic().DefendAttribute);
	RelevantAttributesToCapture.Add(HealthStatic().HealthAttribute);
	RelevantAttributesToCapture.Add(HealthStatic().AttackTypeAttribute);
}

void UGolablAttributeSimpleExecution::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{

	UAbilitySystemComponent * TargetAbilitySystemComponent=ExecutionParams.GetTargetAbilitySystemComponent();
	FGameplayEffectSpec EffectSpec=ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags=EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags=EffectSpec.CapturedTargetTags.GetAggregatedTags();
	float HealthAttribute;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealthStatic().HealthAttribute,EvaluateParameters,HealthAttribute);
	UE_LOG(LogTemp,Warning,TEXT("%f"),HealthAttribute);
	float DamageAttribute;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealthStatic().DamageAttribute,EvaluateParameters,DamageAttribute);
	UE_LOG(LogTemp,Warning,TEXT("%f"),DamageAttribute);
	float DefendAttribute;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealthStatic().DefendAttribute,EvaluateParameters,DefendAttribute);
	UE_LOG(LogTemp,Warning,TEXT("%f"),DefendAttribute);
	float AttackTypeAttribute;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealthStatic().AttackTypeAttribute,EvaluateParameters,AttackTypeAttribute);
	UE_LOG(LogTemp,Warning,TEXT("%f"),AttackTypeAttribute);
	float FinalHealth=HealthAttribute-DamageAttribute*(1-FMath::Clamp(DefendAttribute,0,1))*AttackTypeAttribute;
	FinalHealth=FMath::Max(0,FinalHealth);
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(HealthStatic().HealthAttribute.AttributeToCapture,EGameplayModOp::Override,FinalHealth));
}
