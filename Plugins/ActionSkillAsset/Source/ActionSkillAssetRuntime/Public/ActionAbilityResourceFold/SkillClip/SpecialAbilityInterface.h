// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SpecialAbilityInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USpecialAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONSKILLASSETRUNTIME_API ISpecialAbilityInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UAnimMontage * GetSelectedAnimMontage()=0;
	virtual int32 GetSelectedAnimMontageIndex()=0;
};
