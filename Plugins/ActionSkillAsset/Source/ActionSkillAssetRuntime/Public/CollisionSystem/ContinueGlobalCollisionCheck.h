// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionType.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ContinueGlobalCollisionCheck.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONSKILLASSETRUNTIME_API UContinueGlobalCollisionCheck : public UAnimNotifyState
{
	GENERATED_BODY()
public:
#if WITH_EDITOR
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	//用来编辑内容
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FCollisionContext EditCollisionContext;
	//用来添加射线检测时root的偏移因为用的是actor的，而actor通常是中心而不是最底下会出现偏移问题。
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector TraceBaseOffset;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FRotator RotationBaseOffset;
};
