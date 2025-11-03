// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraManagerFold/ActionPlayerCameraManager.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "CameraComponentFold/CameraComponentInterface.h"
#include "CameraComponentFold/UActionUiCameraComponent.h"
#include "CameraComponentFold/CameraMontage/CameraMontagePlayer.h"
#include "CameraDataFold/ActionCameraTypes.h"
#include "CameraModeFold/ActionCameraMode.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetSystemLibrary.h"

AActionPlayerCameraManager::AActionPlayerCameraManager()
{
	CameraStack=nullptr;
	
}

void AActionPlayerCameraManager::InitializeFor(class APlayerController* PC)
{
	Super::InitializeFor(PC);
	if(!CameraStack)
	{
		CameraStack=NewObject<UActionCameraStack>(this);
		check(CameraStack);
	}
	if(!CameraMontagePlayer)
	{
		check(MontagePlayerClass)
		CameraMontagePlayer=NewObject<UCameraMontagePlayer>(this,MontagePlayerClass);
	}
}

void AActionPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();
	//获取对象
	ControlledPlayer=PCOwner->GetPawn();
	TArray<UCameraComponent *> CameraComponents;
	ControlledPlayer->GetComponents<UCameraComponent>(CameraComponents);
	if(CameraComponents.Num()>0)
	{
		for(auto CameraComponent:CameraComponents)
		{	ECameraForm CameraForm=ICameraComponentInterface::Execute_GetCameraForm(CameraComponent);
			OwnedCamera.FindOrAdd(CameraForm)=CameraComponent;
		}
		SetCurrentActiveCameraComponent(DefaultCameraForm);
	}
}


UCameraComponent* AActionPlayerCameraManager::GetCurrentActiveCameraComponent(ECameraForm& CameraForm)
{
	if(!CurrentActiveCameraCache)return nullptr;
	CameraForm=CurrentActiveCameraTypeCache;
	return  CurrentActiveCameraCache;
}

TArray<UCameraComponent*> AActionPlayerCameraManager::GetUnActivateCameraComponent()
{
	TArray<UCameraComponent*> CameraComponents;
	 for (auto CameraInfo:OwnedCamera)
	 {
		  if(!CameraInfo.Value->IsActive())
		  {
			  CameraComponents.Add(CameraInfo.Value);
		  }
	 }
	return CameraComponents;
}

void AActionPlayerCameraManager::SetCurrentActiveCameraComponent(ECameraForm ActiveCameraEnum)
{
	if(OwnedCamera.Num()<=0 || !OwnedCamera.Find(ActiveCameraEnum)) return;
	for (TPair<ECameraForm,UCameraComponent*> Camera: OwnedCamera)
	{
		if (!Camera.Value || !Camera.Value->IsValidLowLevelFast()) continue;
		UE_LOG(LogTemp,Warning,TEXT("%s"),*Camera.Value->GetName())
		if (Camera.Key == ActiveCameraEnum)
		{
			if (!Camera.Value->IsActive())
			{
				Camera.Value->SetActive(true);
			}
			CurrentActiveCameraCache = Camera.Value;
			CurrentActiveCameraTypeCache = Camera.Key;
		}
		else
		{
			if (Camera.Value->IsActive())
			{
				Camera.Value->SetActive(false);
			}
		}
	}
		
	
}

void AActionPlayerCameraManager::SetAllCameraLocationAndRotation()
{
	for( auto Camera :OwnedCamera)
	{	//不允许摄像机为空或者摄像机为当前摄像机的情况进行修改避免出现重复设置的问题
		if(!Camera.Value || Camera.Value==CurrentActiveCameraCache) continue;
		Camera.Value->MoveComponent(CameraNormalViewInfoCache.CameraLocation,CameraNormalViewInfoCache.CameraRotation,false);
	}
}

void AActionPlayerCameraManager::AddCameraOffset(FVector NewOffset,float FovOffset)
{
	CameraViewLocationOffset+=NewOffset;
	CameraViewFovOffset+=FovOffset;
}


void AActionPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	/*前方获取相机数据*/
	/*如果是UI相机那么就更新其内部的内容*/
	if(UICameraComponent)
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
		UICameraComponent->UpdateViewTarget(OutVT,DeltaTime);
		return;
	}
	if(!CameraStack)
	{
		CameraStack=NewObject<UActionCameraStack>(this);
		Super::UpdateViewTarget(OutVT, DeltaTime);
		OutVT.POV.Location+=FRotationMatrix(OutVT.POV.Rotation).TransformVector(CameraViewLocationOffset);
		OutVT.POV.FOV+=CameraViewFovOffset;
		CameraNormalViewInfoCache.CameraLocation=OutVT.POV.Location;
		CameraNormalViewInfoCache.CameraRotation=OutVT.POV.Rotation;
		CameraNormalViewInfoCache.FOV=OutVT.POV.FOV;
		check(CameraStack);
	}
	else
	{
		UpdateCameraModes();
		// Don't update outgoing viewtarget during an interpolation 
		if ((PendingViewTarget.Target != NULL) && BlendParams.bLockOutgoing && OutVT.Equal(ViewTarget))
		{
			return;
		}
		// Reset the view target POV fully
		static const FMinimalViewInfo DefaultViewInfo;
		OutVT.POV = DefaultViewInfo;
		OutVT.POV.FOV = DefaultFOV;
		OutVT.POV.OrthoWidth = DefaultOrthoWidth;
		OutVT.POV.AspectRatio = DefaultAspectRatio;
		OutVT.POV.bConstrainAspectRatio = bDefaultConstrainAspectRatio;
		OutVT.POV.ProjectionMode = bIsOrthographic ? ECameraProjectionMode::Orthographic : ECameraProjectionMode::Perspective;
		OutVT.POV.PostProcessBlendWeight = 1.0f;
		OutVT.POV.bAutoCalculateOrthoPlanes = bAutoCalculateOrthoPlanes;
		OutVT.POV.AutoPlaneShift = AutoPlaneShift;
		OutVT.POV.bUpdateOrthoPlanes = bUpdateOrthoPlanes;
		OutVT.POV.bUseCameraHeightAsViewTarget = bUseCameraHeightAsViewTarget;
		UpdateActionCameraValue(OutVT.POV,DeltaTime);
		//这里要在最后处理相机的Rotation和Location的Lag;
		bool bDoNotApplyModifiers = false;
		if (!bDoNotApplyModifiers || bAlwaysApplyModifiers)
		{
			// Apply camera modifiers at the end (view shakes for example)
			ApplyCameraModifiers(DeltaTime, OutVT.POV);
		}
		// Synchronize the actor with the view target results
		SetActorLocationAndRotation(OutVT.POV.Location, OutVT.POV.Rotation, false);
		if (bAutoCalculateOrthoPlanes && OutVT.Target)
		{
			OutVT.POV.SetCameraToViewTarget(OutVT.Target->GetActorLocation());
		}

		UpdateCameraLensEffects(OutVT);
	}
	ECameraForm  CameraForm;
	GetCurrentActiveCameraComponent(CameraForm)->SetWorldLocationAndRotation(OutVT.POV.Location,OutVT.POV.Rotation);
	SetAllCameraLocationAndRotation();
}

void AActionPlayerCameraManager::UpdateCameraModes()
{
	check(CameraStack);

	if (CameraStack->IsActive())
	{
		if (CameraModeBindSingleDelegate.IsBound())
		{
			if (const TSubclassOf<UActionCameraMode> CameraMode = CameraModeBindSingleDelegate.Execute())
			{
				//推进去的时候进行了判断避免反复推进去同一个 但是绑定多了我觉得还是有很大的问题。
				CameraStack->PushCameraMode(CameraMode);
			}
		}
	}
}

void AActionPlayerCameraManager::UpdateActionCameraValue(FMinimalViewInfo & OutPOV, float DeltaTime)
{
	FActionCameraNormalViewInfo  StackViewInfo;
	//摄像机模式的过度内容
	CameraStack->EvaluateStack(DeltaTime,StackViewInfo);
	StackViewInfo.CameraLocation+=FRotationMatrix(StackViewInfo.CameraRotation).TransformVector(CameraViewLocationOffset);
	OutPOV.FOV+=CameraViewFovOffset;
	CameraNormalViewInfoCache=StackViewInfo;
	UpdateCameraLag(StackViewInfo,DeltaTime);
	//摄像机蒙太奇的更新
	CameraMontagePlayer->UpdateCameraMontagePlay(DeltaTime,StackViewInfo,CameraNormalViewInfoCache);
	//摄像机延迟变换的更新
	
	//摄像机避障处理的更新

	//给摄像机的缓存赋值。
	/*UpdateCameraMontageLag(StackViewInfo,DeltaTime);*/
	bFirst=false;	
	CameraMontageViewInfoCache=StackViewInfo;
	//给摄像机赋值
	OutPOV.Location=StackViewInfo.CameraLocation;
	OutPOV.Rotation=StackViewInfo.CameraRotation;
	OutPOV.FOV=StackViewInfo.FOV;
	//摄像机后处理的更新
}

void AActionPlayerCameraManager::UpdateCameraLag(FActionCameraNormalViewInfo& ActionCameraNormalViewInfo,
	float DeltaTime)
{	if(bFirst) return;
	FRotator DesiredRot=ActionCameraNormalViewInfo.CameraRotation;
	if(bDoRotationLag)
	{
		//这里是一个分布迭代的算法
		if(bUseCameraTimeStep && DeltaTime>CameraLagMaxTimeStep &&	CameraRotationLagSpeed>0.f)
		{
			const FRotator RotStep=(ActionCameraNormalViewInfo.CameraRotation-CameraNormalViewInfoCache.CameraRotation).GetNormalized()*(1.f/DeltaTime);
			FRotator LerpTarget=CameraNormalViewInfoCache.CameraRotation;
			float RemainingTime=DeltaTime;
			while (RemainingTime>UE_KINDA_SMALL_NUMBER)
			{
				const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
				LerpTarget += RotStep * LerpAmount;
				RemainingTime -= LerpAmount;

				DesiredRot = FRotator(FMath::QInterpTo(FQuat(CameraNormalViewInfoCache.CameraRotation), FQuat(LerpTarget), LerpAmount, CameraRotationLagSpeed));
				ActionCameraNormalViewInfo.CameraRotation=DesiredRot;
				CameraNormalViewInfoCache.CameraRotation= DesiredRot;
			}
		}
		else
		{
			DesiredRot=FRotator(FMath::QInterpTo(FQuat(CameraNormalViewInfoCache.CameraRotation),FQuat(DesiredRot),DeltaTime,CameraRotationLagSpeed));
		}
	}
	CameraNormalViewInfoCache.CameraRotation=DesiredRot;
	ActionCameraNormalViewInfo.CameraRotation=DesiredRot;
	FVector Origin=CameraNormalViewInfoCache.CameraLocation;
	FVector DesiredLoc=ActionCameraNormalViewInfo.CameraLocation;
	FVector CurrentLoc=CameraNormalViewInfoCache.CameraLocation;
	if(bDoLocationLag)
	{
		if(bUseCameraTimeStep && DeltaTime>CameraLagMaxTimeStep && CameraLagSpeed>0.f)
		{
			const FVector MovementStep=(DesiredLoc-CurrentLoc)*(1.f/DeltaTime);
			FVector Target=CurrentLoc;
			float RemainingTime = DeltaTime;
			while (RemainingTime > UE_KINDA_SMALL_NUMBER)
			{
				const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
				Target +=MovementStep  * LerpAmount;
				RemainingTime -= LerpAmount;

				DesiredLoc = FMath::VInterpTo(CurrentLoc, Target, LerpAmount, CameraLagSpeed);
				CurrentLoc = DesiredLoc;
				ActionCameraNormalViewInfo.CameraLocation=DesiredLoc;
				CameraNormalViewInfoCache.CameraLocation=DesiredLoc;
			}
		}
		else
		{
			DesiredLoc = FMath::VInterpTo(CurrentLoc, DesiredLoc, DeltaTime, CameraLagSpeed);
		}
	}
	ActionCameraNormalViewInfo.CameraLocation=DesiredLoc;
	CameraNormalViewInfoCache.CameraLocation=DesiredLoc;
}

void AActionPlayerCameraManager::UpdateCameraMontageLag(FActionCameraNormalViewInfo& ActionCameraNormalViewInfo,
	float DeltaTime)
{
	if(bFirst) return;
	FRotator DesiredRot=ActionCameraNormalViewInfo.CameraRotation;
	if(bDoRotationLag)
	{
		//这里是一个分布迭代的算法
		if(bUseCameraTimeStep && DeltaTime>CameraLagMaxTimeStep &&	CameraRotationLagSpeed>0.f)
		{
			const FRotator RotStep=(ActionCameraNormalViewInfo.CameraRotation-CameraMontageViewInfoCache.CameraRotation).GetNormalized()*(1.f/DeltaTime);
			FRotator LerpTarget=CameraMontageViewInfoCache.CameraRotation;
			float RemainingTime=DeltaTime;
			while (RemainingTime>UE_KINDA_SMALL_NUMBER)
			{
				const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
				LerpTarget += RotStep * LerpAmount;
				RemainingTime -= LerpAmount;
				DesiredRot = FRotator(FMath::QInterpTo(FQuat(CameraMontageViewInfoCache.CameraRotation), FQuat(LerpTarget), LerpAmount, CameraRotationLagSpeed));
			}
			ActionCameraNormalViewInfo.CameraRotation=DesiredRot;
			CameraMontageViewInfoCache.CameraRotation= DesiredRot;
		}
		else
		{
			DesiredRot=FRotator(FMath::QInterpTo(FQuat(CameraMontageViewInfoCache.CameraRotation),FQuat(DesiredRot),DeltaTime,CameraRotationLagSpeed));
		}
	}
	
	CameraMontageViewInfoCache.CameraRotation=DesiredRot;
	ActionCameraNormalViewInfo.CameraRotation=DesiredRot;
	FVector Origin=CameraMontageViewInfoCache.CameraLocation;
	FVector DesiredLoc=ActionCameraNormalViewInfo.CameraLocation;
	FVector CurrentLoc=CameraMontageViewInfoCache.CameraLocation;
	if(bDoLocationLag)
	{
		if(bUseCameraTimeStep && DeltaTime>CameraLagMaxTimeStep && CameraLagSpeed>0.f)
		{
			const FVector MovementStep=(DesiredLoc-CurrentLoc)*(1.f/DeltaTime);
			FVector Target=CurrentLoc;
			float RemainingTime = DeltaTime;
			while (RemainingTime > UE_KINDA_SMALL_NUMBER)
			{
				const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
				Target +=MovementStep  * LerpAmount;
				RemainingTime -= LerpAmount;

				DesiredLoc = FMath::VInterpTo(CurrentLoc, Target, LerpAmount, CameraLagSpeed);
				CurrentLoc = DesiredLoc;
				ActionCameraNormalViewInfo.CameraLocation=DesiredLoc;
				CameraMontageViewInfoCache.CameraLocation=DesiredLoc;
			}
		}
		else
		{
			DesiredLoc = FMath::VInterpTo(CurrentLoc, DesiredLoc, DeltaTime, CameraLagSpeed);
		}
	}
	ActionCameraNormalViewInfo.CameraLocation=DesiredLoc;
	CameraMontageViewInfoCache.CameraLocation=DesiredLoc;
}
#pragma region Debug
void AActionPlayerCameraManager::DisplayDebug(class UCanvas* Canvas, const class FDebugDisplayInfo& DebugDisplay,float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);
	/*这种方法不能用在具有反射功能的函数当中，反射宏会创建一个另外的函数，直接使用会为空*/
	ICameraComponentInterface * CI_TMP=Cast<ICameraComponentInterface>(CurrentActiveCameraCache);
	if(CI_TMP){CI_TMP->DrawDebug(Canvas);}
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;
	ECameraForm CameraForm;
	DisplayDebugManager.DrawString(FString::Printf(TEXT("CurrentCameraName: %s"),*GetCurrentActiveCameraComponent(CameraForm)->GetName()));
	DisplayDebugManager.DrawString(FString::Printf(TEXT("CurrentFov: %f"),GetCameraCacheView().FOV));
	CameraStack->DrawDebug(Canvas);
	CameraMontagePlayer->DisplayDebug(Canvas);
}
#pragma endregion

#pragma region  UIComponent
UActionUiCameraComponent* AActionPlayerCameraManager::GetActionUICameraComponent()
{
	return UICameraComponent?UICameraComponent:nullptr;
}
void AActionPlayerCameraManager::ActiveUICameraComponent(AActor* InUiCameraActor,FViewTargetTransitionParams TransitionParams)
{
	SetViewTarget(InUiCameraActor,TransitionParams);
	UActionUiCameraComponent * ActionUiCamera= InUiCameraActor->FindComponentByClass<UActionUiCameraComponent>();
	if(ActionUiCamera)
	{
		ActionUiCamera->SetViewTarget(ControlledPlayer,TransitionParams);
		UICameraComponent=ActionUiCamera;
	}
} 
void AActionPlayerCameraManager::DeactiveUICameraComponent(FViewTargetTransitionParams TransitionParams)
{
	SetViewTarget(ControlledPlayer,TransitionParams);
	UICameraComponent->OnLeaveViewTarget();
	UICameraComponent=nullptr;
}
#pragma endregion
#pragma region  SceneColorChange
void AActionPlayerCameraManager::SetPrimarySceneColor(FVector Color)
{
	if(PrimaryColorScale!=ColorScale)
	{
		SetDesiredColorScale(Color,0);
	}
	PrimaryColorScale=Color;
}
void AActionPlayerCameraManager::SetSceneColorWithCustom(FVector WantedColorScale, float BlendInTime,float BlendOutTime,float DurationTime)
{
	SetDesiredColorScale(WantedColorScale,BlendInTime);
	if(ColorScaleHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ColorScaleHandle);
	}
	check(GetWorld());
	GetWorld()->GetTimerManager().SetTimer(ColorScaleHandle,[this,BlendOutTime,DurationTime]()
	{
		if(DurationTime>0)
		{
			SetDesiredColorScale(PrimaryColorScale,BlendOutTime);
		}
		else
		{
			if(ColorScaleHandle.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(ColorScaleHandle);
			}
		}
	},DurationTime,false);
}
#pragma endregion