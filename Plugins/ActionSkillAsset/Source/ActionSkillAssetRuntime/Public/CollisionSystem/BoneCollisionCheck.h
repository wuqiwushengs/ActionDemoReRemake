// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionType.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BoneCollisionCheck.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONSKILLASSETRUNTIME_API UBoneCollisionCheck : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FCollisionContext EditCollisionContext;
	/*//该变量用于在当有别的碰撞开启了trace后如果再次调用则可能会出现提前关闭的情况
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bAllowAsTraceEnder;*/
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent) override;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual  void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
};
