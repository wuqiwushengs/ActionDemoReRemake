// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ActionPlayController.generated.h"

class UInputMappingContext;
class UInputDataAsset;
/**
 * 
 */
UCLASS()
class ACTIONSKILLASSETRUNTIME_API AActionPlayController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	UInputDataAsset * GetInputDataAsset();
	void RegisterInputAction(APawn * InPawn);
protected:
	UPROPERTY(EditDefaultsOnly)
	UInputDataAsset* InputDataAsset;
	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext *NormalMappingContext;
	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext * AbilityMappingContext;
};
