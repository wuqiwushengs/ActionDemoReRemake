// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CallPreInput.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONSKILLASSETRUNTIME_API UCallPreInput : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	//用来检测当前输入时什么预输入是不允许的。
	UPROPERTY(EditAnywhere,meta=(ExposeOnSpawn),BlueprintReadOnly)
	FGameplayTagContainer DisablePreTag;
};
