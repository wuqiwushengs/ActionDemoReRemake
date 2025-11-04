// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraComponentFold/CameraMontage/CameraMontageInterface.h"
#include "CameraDataFold/ActionCameraTypes.h"
#include "UObject/Object.h"
#include "CameraMontagePlayer.generated.h"



enum class ECameraMontageBlendType : uint8;
struct FCameraAnimMontagePlay;
/**
 * 
 */
namespace ActionViewInterp
{
	FActionCameraNormalViewInfo InterpActionView(float DeltaTime,bool bFovInterp,float interpFovSpeed,bool bLocationInterp,float InterpLocationSpeed,bool bRotationInterp,float InterpRotationSpeed,const FActionCameraNormalViewInfo & FromViewInfo,const FActionCameraNormalViewInfo & ToViewInfo);	
}

UCLASS(Blueprintable)
class UCameraMontageSequence : public UObject
{
	GENERATED_BODY()
public:
	UCameraMontageSequence();
	UPROPERTY(EditDefaultsOnly)
	FCameraAnimMontagePlay CameraAnimMontageInfo;
	void SetBlendType(ECameraMontageBlendType BlendType);
	ECameraMontageBlendType GetBlendType() { return CameraMontageBlendType;}
	FGuid Getid() const { return id;}
	virtual void OnDeactivate() ;
	virtual void OnActive();
	UFUNCTION()
	virtual void UpdateCameraMontageSequence(float DeltaTime,FActionCameraNormalViewInfo & MontageViewInfo);
	UFUNCTION()
	AActor * GetTargetActor()
	{
		if(!TargetActor)
		{
			TargetActor=SetTargetActor_Implementation();
		}
		return TargetActor;
	}
	//如果默认的话这个目标Actor就是角色自身
	UFUNCTION(BlueprintNativeEvent)
	AActor * SetTargetActor();
	AActor* SetTargetActor_Implementation();
	UFUNCTION(BlueprintCallable)
	APawn * GetControlledPlayer();
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FName GetSocket() { return CameraAnimMontageInfo.SocketName;}
	//获取插槽信息
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FVector GetSocketLocation();
	UFUNCTION(BlueprintCallable,BlueprintPure)
	float GetPlayedTime() { return PlayedTime;}
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FGameplayTag GetMontageGameplayTag() { return MontageTag;}
	virtual void DisplayDebug(UCanvas * Canvas);
	float GetCurrentWeight() { return CurrentBlendWeight;}
private:
	float CurrentBlendAlpha=0.0f;
	float CurrentBlendWeight=0.0f;
	float PlayedTime=0.0f;
	ECameraMontageBlendType CameraMontageBlendType;
	FGuid id;
	virtual  void UpdateMontageInfo(float DeltaTime,FActionCameraNormalViewInfo & MontageViewInfo);
	virtual void UpdateBlendInfo(float DeltaTime);
	UPROPERTY()
	AActor * TargetActor=nullptr;
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag MontageTag;
};
UCLASS(Blueprintable)
class ACTIONCAMERASYSTEMASSET_API UCameraMontagePlayer : public UObject
{
	GENERATED_BODY()
	
public:
	UCameraMontagePlayer();
	UFUNCTION(BlueprintCallable)
	void PushCameraMontageSequence(TSubclassOf<UCameraMontageSequence> MontageSequence);
	UFUNCTION(BlueprintCallable)
	void DeactivateAdditiveMontageByTag(FGameplayTag GameplayTag);
	UFUNCTION(BlueprintCallable)
	void DeactivateAdditiveMontageByClass(TSubclassOf<UCameraMontageSequence> MontageClass);
	UFUNCTION(BlueprintCallable)
	void DeactivateModifyMontage();
	UFUNCTION(BlueprintCallable)
	UCameraMontageSequence * GetSequenceInstance(TSubclassOf<UCameraMontageSequence> SequenceClass);
	FCameraMontageValueCalculateFactory * GetCameraMontageValueCalculateFactory();
	void UpdateCameraMontagePlay(float DeltaTime,FActionCameraNormalViewInfo & NormalViewInfo,FActionCameraNormalViewInfo & NormalViewInfoCache);
	UPROPERTY(EditDefaultsOnly)
	bool bCameraLocationLag=true;
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="bCameraLocationLag"))
	float CameraLogSpeed=20.0f;
	UPROPERTY(EditDefaultsOnly)
	bool bCameraRotationLag=true;
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="bCameraRotationLag"))
	float CameraRotationLagSpeed=20.0;
	UPROPERTY(EditDefaultsOnly)
	bool bCameraFovLag=true;
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="bCameraFovLag"))
	float CameraFovLagSpeed=20.0f;
	void DisplayDebug(UCanvas * Canvas);
private:
	
	void PushAdditiveMontage(UCameraMontageSequence * Sequence);
	void PushModifyMontage(UCameraMontageSequence * Sequence);
	void UpdateCameraMontageStack(float DeltaTime);
	void UpdateCameraMontageAdditivePlay(float DeltaTime,FActionCameraNormalViewInfo & InAdditiveViewInfo);
	void UpdateCameraMontageModifyPlay(float DeltaTime,FActionCameraNormalViewInfo & InModifyViewInfo,FActionCameraNormalViewInfo & NormalViewInfoCache);
	void ProcessFindChangedMontage(float DeltaTime,const FActionCameraNormalViewInfo& InitialViewInfo,const FActionCameraNormalViewInfo& InAdditiveViewInfo,FActionCameraNormalViewInfo InModifyViewInfo,FActionCameraNormalViewInfo & ApplyViewInfo);
	UPROPERTY()
	TSet<UCameraMontageSequence *> CameraMontageSequenceModifyInstances;
	UPROPERTY()
	TSet<UCameraMontageSequence*> CameraMontageSequencesAdditiveInstances;
	//直接设置一个单独的，当有新的插入是过度直接从原来定格的位置通过lag过度到最后想要的位置。
	UPROPERTY()
	UCameraMontageSequence* CameraMontagePlayModifyStack;
	UPROPERTY()
	TArray<UCameraMontageSequence*> CameraMontagePlayAdditiveStack;
	//相机蒙太奇处理的工厂
	TSharedPtr<FCameraMontageValueCalculateFactory,ESPMode::NotThreadSafe> CameraMontageValueCalculateFactory;
};
