// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillTypes/SkillTypes.h"
#include "UObject/Interface.h"
#include "SkillClipTypeCheck.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USkillClipTypeCheck : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONSKILLASSETRUNTIME_API ISkillClipTypeCheck
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual ESkillCLipType GetSkillClipType()=0;
	virtual  FEndAbilityEvent& GetAbilitySignal()=0;
};
