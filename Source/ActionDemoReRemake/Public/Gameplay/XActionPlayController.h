// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GamePlay/ActionPlayController.h"
#include "XActionPlayController.generated.h"

class UActionInputComponent;
/**
 * 
 */
UCLASS()
class ACTIONDEMOREREMAKE_API AXActionPlayController : public AActionPlayController
{
	GENERATED_BODY()
public:
		virtual void CustomNormalInputBinding(UEnhancedInputLocalPlayerSubsystem *InputSubsystem,UActionInputComponent * ActionInputComponent ,AActionPlayerCharacter * RefCharacter) override;
};
