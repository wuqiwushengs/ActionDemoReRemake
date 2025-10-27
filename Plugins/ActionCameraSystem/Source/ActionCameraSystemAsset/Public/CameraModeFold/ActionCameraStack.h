// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraDataFold/ActionCameraTypes.h"
#include "UObject/Object.h"
#include "ActionCameraStack.generated.h"

struct FActionCameraNormalViewInfo;
struct FGameplayTag;
class UActionCameraMode;
/**
 * 
 */

UCLASS()
class ACTIONCAMERASYSTEMASSET_API UActionCameraStack : public UObject
{
	GENERATED_BODY()
	UActionCameraStack();
public:
	void ActiveStack();
	void DeactivateStack();
	bool IsActive() {return bisActive;};
	void DrawDebug(UCanvas * Canvas);
	void GetBlendWeightByTag(FGameplayTag ModeTag,float & BlendWeight );
	void GetTopBlendWeight(FGameplayTag & TopTag,float & BlendWeight);
	void PushCameraMode(TSubclassOf<UActionCameraMode> CameraModeClass);
	bool EvaluateStack(float DeltaTime,FActionCameraNormalViewInfo &StackViewInfo);
	void UpdateStack(float DeltaTime);
	void BlendStack(float DeltaTime,FActionCameraNormalViewInfo  &StackViewInfo);
	void UpdatePivot(float DeltaTime);
	UActionCameraMode * GetCameraModeInstance(TSubclassOf<UActionCameraMode> CameraModeClass);
	FVector GetPivotLocation() {return  PivotLocation ;}
protected:
	FVector PivotLocation;
	bool bisActive;
	UPROPERTY()
	TArray<TObjectPtr<UActionCameraMode>> CameraModeInstance;
	UPROPERTY()
	TArray<TObjectPtr<UActionCameraMode>> CameraModeStack;
};
