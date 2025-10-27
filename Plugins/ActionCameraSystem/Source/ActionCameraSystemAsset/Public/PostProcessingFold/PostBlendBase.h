// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PostBlendBase.generated.h"

/**
 * 通过重载他来更改后处理内容
 */
UCLASS(Abstract)
class ACTIONCAMERASYSTEMASSET_API UPostBlendBase : public UObject,public IBlendableInterface
{
	GENERATED_BODY()

public:
	/**/
	virtual void ChangeBlendableSettings(FSceneView& View,float Weight) const;
private:
	virtual void OverrideBlendableSettings(class FSceneView& View, float Weight)  const override final;
};
