// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "SkillExecutorConfigFactory.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, hidecategories=Object)
class USkillExecutorConfigFactory : public UFactory
{
	GENERATED_BODY()
public:
	USkillExecutorConfigFactory(const FObjectInitializer& ObjectInitializer);
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext);
	
};
