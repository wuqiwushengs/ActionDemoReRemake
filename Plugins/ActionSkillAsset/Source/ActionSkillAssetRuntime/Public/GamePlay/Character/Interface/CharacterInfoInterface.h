// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterInfoInterface.generated.h"

struct FCharacterNormalInputData;
// This class does not need to be modified.
UINTERFACE()
class UCharacterInfoInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONSKILLASSETRUNTIME_API ICharacterInfoInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	 virtual FCharacterNormalInputData GetCharacterNormalInputData()=0;
};
