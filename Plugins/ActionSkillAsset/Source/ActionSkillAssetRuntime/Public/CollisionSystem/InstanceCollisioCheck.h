// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionType.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "InstanceCollisioCheck.generated.h"

class ATargetPoint;
/**
 *  当创建后必须要Notify一次不然更改变量会崩溃
 */

UCLASS()
class ACTIONSKILLASSETRUNTIME_API UInstanceCollisioCheck : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual  void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	//用来编辑内容
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FCollisionContext EditCollisionContext;
	//用来添加射线检测时root的偏移因为用的是actor的，而actor通常是中心而不是最底下会出现偏移问题。
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector TraceBaseOffset;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FRotator RotationBaseOffset;
};
