// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraDataFold/ActionCameraTypes.h"
#include "CameraDataFold/BlendData.h"
#include "UObject/Object.h"
#include "ActionCameraMode.generated.h"
class UActionCameraStack;
/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONCAMERASYSTEMASSET_API UActionCameraMode : public UObject
{
	GENERATED_BODY()
	friend UActionCameraStack;
public:
	UActionCameraMode();
	~UActionCameraMode();
	void UpdateCameraMode(float DeltaTime);
	void UpdateView(float DeltaTime);
	void UpdateSettledView(float DeltaTime);
	void UpdateFreeMovementView(float DeltaTime);
	void UpdateSpringArmFunc(FActionCameraNormalViewInfo & CameraNormalViewInfo,float DeltaTime);
	void UpdateBlendWeight(float DeltaTime);
	AActionPlayerCameraManager * GetPlayerCameraManager() const ;
	AActor * GetTargetActor() const ;
	virtual void DrawDebug(UCanvas * Canvas);
	UFUNCTION()
	float GetBlendAlpha() const  {return  BlendAlpha;} 
	UFUNCTION()
	float GetBlendWeight() const {return BlendWeight;}  
	void SetBlendWeight(EBlendType BlendType,float Weight=0);
	FGuid CameraID;
	UPROPERTY(EditAnywhere)
	FBlendCurveInfo BlendCurveInfo;
	UPROPERTY(BlueprintReadOnly)
	EBlendType CurrentBlendMode=EBlendType::WaitAdd;
	virtual  FActionCameraNormalViewInfo GetActionCameraOffsetValue();
	virtual FActionCameraNormalViewInfo GetActionCameraViewInfo() const;
	//仅仅附加一次的相机offset
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FActionCameraNormalViewInfo CameraOffset;
	//根据Pitch轴角度进行映射的相机偏移offset
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FCameraOffsetMapCurve OffsetMapCurve;
	UPROPERTY(BlueprintReadOnly)
	FActionCameraNormalViewInfo ViewInfo;
	//设置模式
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	ECameraPlacedMode PlacedMode;
	//适用于固定相机的内容
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,meta=(EditCondition="PlacedMode==ECameraPlacedMode::Settled"))
	FActionCameraNormalViewInfo FixedViewInfo;
	//仅仅只对自由移动相机起作用，固定相机没用
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FCameraSpringArmInfo SpringArmInfo;
	UPROPERTY(EditDefaultsOnly)
	float FieldOfView=90.0f;
	UPROPERTY(EditDefaultsOnly)
	float ViewPitchMin=-90.0f;
	UPROPERTY(EditDefaultsOnly)
	float ViewPitchMax=90.0f;
	FGameplayTag GetCameraTag()
	{
		return CameraTypeTag;
	}
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag CameraTypeTag;
	virtual void OnActivation()
	{
		UE_LOG(LogTemp,Warning,TEXT("Activation"))
	};
	virtual void OnDeactivation()
	{
		BlendAlpha=0.0f;
		BlendWeight=0.0f;
	};
	virtual FVector GetPivotLocation() const;
	virtual FRotator GetPivotRotation() const;
private:
	float BlendAlpha=0.0f;
	float BlendWeight=0.0f;
	FVector LastPivot;
};
