// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraViewInfoFactory.h"
#include "CameraDataFold/ActionCameraTypes.h"
#include "CameraDataFold/BlendData.h"
#include "UObject/Object.h"
#include "ActionCameraMode.generated.h"
class ACameraViewInfoFactory;
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
	UFUNCTION(BlueprintCallable)
	AActionPlayerCameraManager * GetPlayerCameraManager() const ;
	AActor * GetTargetActor() const ;
	virtual void DrawDebug(UCanvas * Canvas);
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	AActor * SetTargetActor();
	AActor* SetTargetActor_Implementation();
	UFUNCTION()
	float GetBlendAlpha() const  {return  BlendAlpha;} 
	UFUNCTION()
	float GetBlendWeight() const {return BlendWeight;}  
	void SetBlendWeight(EBlendType BlendType,float Weight=0);
	FGuid CameraID;
	//用于一些演出类型，避免过多临时演出类型存储到堆栈造成膨胀;
	UPROPERTY(EditDefaultsOnly)
	bool DoOnce=false;
	UPROPERTY(EditAnywhere)
	FBlendCurveInfo BlendCurveInfo;
	UPROPERTY(BlueprintReadOnly)
	ACameraViewInfoFactory * ViewInfoFactory;
	EBlendType CurrentBlendMode=EBlendType::WaitAdd;
	virtual  FActionCameraNormalViewInfo GetActionCameraOffsetValue();
	virtual FActionCameraNormalViewInfo GetActionCameraViewInfo() const;
	//仅仅附加一次的相机offset 
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FActionCameraNormalViewInfo CameraOffset;
	//根据Pitch轴角度进行映射的相机偏移offset 用在Freedom模式当中
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
	
	
	void OnActivation();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void OnActivationBP();
	void  OnActivationBP_Implementation();
	void OnDeactivation()
	{
		BlendAlpha=0.0f;
		BlendWeight=0.0f;
		SettledTargetActor=nullptr;
		OnDeactivationBP();
	};
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void OnDeactivationBP();
	void OnDeactivationBP_Implementation()
	{
	
	};
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void OnBlendIn();
	void OnBlendIn_Implementation();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void OnLoop();
	void OnLoop_Implementation();
	
	virtual FVector GetPivotLocation() const;
	virtual FRotator GetPivotRotation() const;
	virtual FVector GetPivotAfterOffsetLocation()const;

private:
	float BlendAlpha=0.0f;
	float BlendWeight=0.0f;
	FVector LastPivot;
	UPROPERTY()
	AActor * SettledTargetActor;
};
