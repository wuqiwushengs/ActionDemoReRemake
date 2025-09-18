// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "InputTriggerOnceChordAction.generated.h"

struct FInputActionInstance;
/**
 * 
 */
UCLASS()
class ACTIONSKILLASSETRUNTIME_API UInputTriggerOnceChordAction : public UInputTrigger
{
	GENERATED_BODY()
public:
	virtual ETriggerType GetTriggerType_Implementation() const { return ETriggerType::Implicit; }
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Trigger Settings", meta = (DisplayThumbnail = "false"))
	TObjectPtr<const UInputAction> PreInputAction=nullptr;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Trigger Settings", meta = (DisplayThumbnail = "false"))
	TObjectPtr<const UInputAction> PostInputAction=nullptr;
	bool bTriggered=false;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Trigger Settings", meta = (DisplayThumbnail = "false"))
	float TriggerTimeOffset=0.05f;
protected:
	virtual   ETriggerState UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime) override;
	bool CheckPreInputIsOnGoing(const FInputActionInstance  * PreEventData);
};
