// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CheckFollowEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONSKILLASSETRUNTIME_API UCheckFollowEnemy : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	UPROPERTY(EditAnywhere)
	float WarpDistanceWarp=50.0f;
	UPROPERTY(EditAnywhere)
	bool ToTargetDirectly=false;
};
