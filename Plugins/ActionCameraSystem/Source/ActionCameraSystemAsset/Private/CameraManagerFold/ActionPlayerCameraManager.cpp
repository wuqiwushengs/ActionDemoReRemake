// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraManagerFold/ActionPlayerCameraManager.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "CameraComponentFold/UActionUiCameraComponent.h"
#include "CameraComponentFold/CameraMontage/CameraMontagePlayer.h"
#include "CameraDataFold/ActionCameraTypes.h"
#include "CameraModeFold/ActionCameraMode.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PostProcessingFold/PostBlendStack.h"

AActionPlayerCameraManager::AActionPlayerCameraManager()
{
	CameraStack=nullptr;
	
}

void AActionPlayerCameraManager::PushCameraMode(TSubclassOf<UActionCameraMode> CameraMode)
{
	check(CameraStack);

	if (CameraStack->IsActive())
	{
		CameraStack->PushCameraMode(CameraMode);
	}
}

void AActionPlayerCameraManager::InitializeFor(class APlayerController* PC)
{
	Super::InitializeFor(PC);
	if(!CameraStack)
	{
		CameraStack=NewObject<UActionCameraStack>(this);
		check(CameraStack);
		CameraStack->PushCameraMode(DefaultCameraMode);
	}
	if(!CameraMontagePlayer)
	{
		check(MontagePlayerClass)
		CameraMontagePlayer=NewObject<UCameraMontagePlayer>(this,MontagePlayerClass);
	}
	if(!PostBlendStack)
	{
		PostBlendStack=NewObject<UPostBlendStack>(this);
	}
	if(!ViewInfoFactory)
	{
		ViewInfoFactory=Cast<ACameraViewInfoFactory>(GetWorld()->SpawnActor(ACameraViewInfoFactory::StaticClass()));
	}
}

void AActionPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();
	//获取对象
	ControlledPlayer=PCOwner->GetPawn();
	TArray<UCameraComponent *> CameraComponents;
	ControlledPlayer->GetComponents<UCameraComponent>(CameraComponents);
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
			if ((PendingViewTarget.Target != NULL) && BlendParams.bLockOutgoing && OutVT.Equal(ViewTarget))
		{
			return;
		}
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
	CameraMontagePlayer->UpdateCameraMontagePlay(DeltaTime,StackViewInfo,CameraNormalViewInfoCache,AdditiveViewInfoCache,ModifyViewInfoCache);
	//摄像机避障处理的更新
	//因为MontageInfo是最后的更新缓存所以我们不用原来camera的数据，因为我们不期望改变相机的DesiredLocation而是对其进行更改
	UpdateCameraAvoidance(StackViewInfo,CameraMontageViewInfoCache,DeltaTime);
	//给摄像机的缓存赋值。
	bFirst=false;	
	CameraMontageViewInfoCache=StackViewInfo;
	//给摄像机赋值
	OutPOV.Location=StackViewInfo.CameraLocation;
	OutPOV.Rotation=StackViewInfo.CameraRotation;
	OutPOV.FOV=StackViewInfo.FOV;
	//摄像机后处理的更新
	PostBlendStack->UpdateBlendInfo(DeltaTime);
	OutPOV.PostProcessSettings=PostBlendStack->GetFinalPostProcessSettings();
	FinalCameraProcessSettingCache=OutPOV.PostProcessSettings;
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

void AActionPlayerCameraManager::UpdateCameraAvoidance(FActionCameraNormalViewInfo& ActionCameraNormalViewInfo,
	FActionCameraNormalViewInfo Cache, float DeltaTime)
{
	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::LineTraceMulti(
		this,
		CameraStack->GetPivotLocation() + AdditiveViewInfoCache.CameraLocation,
		ActionCameraNormalViewInfo.CameraLocation,
		AvoidanceTraceType,
		true,
		{ ControlledPlayer }, // 直接初始化忽略列表
		AvoidanceDebugType,
		HitResults,
		true
	);
	if (const FHitResult* ValidHit = HitResults.FindByPredicate([this](const FHitResult& Hit) {
		return !IgnoreActor.Contains(Hit.GetActor()->GetClass());
	}))
	{
		ActionCameraNormalViewInfo.CameraLocation = ValidHit->ImpactPoint;
	}
}
#pragma region Debug
void AActionPlayerCameraManager::DisplayDebug(class UCanvas* Canvas, const class FDebugDisplayInfo& DebugDisplay,float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);
	/*这种方法不能用在具有反射功能的函数当中，反射宏会创建一个另外的函数，直接使用会为空*/
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;
	DisplayDebugManager.DrawString(FString::Printf(TEXT("CurrentFov: %f"),GetCameraCacheView().FOV));
	CameraStack->DrawDebug(Canvas);
	CameraMontagePlayer->DisplayDebug(Canvas);
	PostBlendStack->DisplayDebug(Canvas);
	
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
	if(ViewHandle.IsValid()) return;
	SetViewTarget(ControlledPlayer,TransitionParams);
	GetWorld()->GetTimerManager().SetTimer(ViewHandle,[this]()
	{
		if(!UICameraComponent) return;
		UICameraComponent->OnLeaveViewTarget();
	UICameraComponent=nullptr;
	},TransitionParams.BlendTime,false);
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

void AActionPlayerCameraManager::SetNewPostProcess(FPostProcessSettings PostSettings)
{
	if(PostBlendStack)
	{
		PostBlendStack->StartPostProcessingChange(PostSettings);
	}
}

void AActionPlayerCameraManager::AddPostProcess(TSubclassOf<UPostBlendbaseMode> BlendMode)
{
	PostBlendStack->AddBlendMode(BlendMode);
}
#pragma endregion
