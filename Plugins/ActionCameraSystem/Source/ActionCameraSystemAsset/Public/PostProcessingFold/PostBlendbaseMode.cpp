#include "PostBlendbaseMode.h"
#include "CameraComponentFold/CameraMontage/CameraMontageInterface.h"
#include "CameraDataFold/ActionCameraTypes.h"
#include "Engine/Canvas.h"

UPostBlendbaseMode::UPostBlendbaseMode()
{
	id=FGuid::NewGuid();
}

void UPostBlendbaseMode::UpdatePostBlendable(float DeltaTime)
{
	UpdateBlendInfo(DeltaTime);
}

void UPostBlendbaseMode::DisplayDebug(UCanvas* Canvas)
{
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;
	DisplayDebugManager.DrawString(FString::Printf(TEXT("CameraMode %s"),*GetName()));
	DisplayDebugManager.DrawString(FString::Printf(TEXT("CurrentModeAlpha %f"),CurrentAlpha));
	DisplayDebugManager.DrawString(FString::Printf(TEXT("CurrentModeWeight %f"),CurrentWeight));
}

TArray<FWeightedBlendable> UPostBlendbaseMode::GetFinalWeightedBlendable()
{
	TArray<FWeightedBlendable> Blendables;
	if(PostBlendObject.Num()<0) return Blendables;
	for (auto Var: PostBlendObject)
	{
		if(!Var.PostBlendobject) continue;
		FWeightedBlendable Blendable;
		Blendable.Object=Var.PostBlendobject;
		if(PostProcessBlendType==ECameraMontageBlendType::BlendIn||PostProcessBlendType==ECameraMontageBlendType::Loop)
		{
			Blendable.Weight= FMath::GetMappedRangeValueClamped(FVector2d(0.0f,1.0f),FVector2d(Var.FromAlpha,Var.ToAlpha),CurrentWeight);
		}
		if(PostProcessBlendType==ECameraMontageBlendType::BlendOut)
		{
			Blendable.Weight=FMath::GetMappedRangeValueClamped(FVector2d(0.0f,1.0f),FVector2d(0.0f,Var.ToAlpha),CurrentWeight);
		}
		Blendables.Add(Blendable);
	}
	return Blendables;
}

void UPostBlendbaseMode::OnActive()
{
}

void UPostBlendbaseMode::DeActive()
{
}

void UPostBlendbaseMode::UpdateBlendInfo(float DeltaTime)
{

	PlayedTime+=DeltaTime;
	if(PostProcessBlendType==ECameraMontageBlendType::WaitAdd) return;
	if(PostProcessBlendType==ECameraMontageBlendType::Loop)
	{
		if(this->bLoop) return;
		if(PlayedTime>=(this->Duration -BlendData.BlendOutCurveInfo.BlendTime))
		{
			SetBlendType(ECameraMontageBlendType::BlendOut);
			return;
		}
	}
	if(PostProcessBlendType==ECameraMontageBlendType::BlendIn)
	{
		CurrentAlpha+=DeltaTime/BlendData.BlendInCurveInfo.BlendTime;
		CurrentAlpha=FMath::Clamp(CurrentAlpha,0,1);
		CurrentWeight=CameraMontageBlend::GetCorrectBlendWeight(ECameraMontageBlendType::BlendIn,BlendData,CurrentAlpha); 
		CurrentWeight=FMath::Min(CurrentWeight,1);
		if(CurrentWeight>=1) { SetBlendType(ECameraMontageBlendType::Loop);}
	}
	if(PostProcessBlendType==ECameraMontageBlendType::BlendOut)
	{
		FBlendData BlendInfo=BlendData;
		CurrentAlpha+=DeltaTime/BlendInfo.BlendOutCurveInfo.BlendTime;
		CurrentAlpha=FMath::Clamp(CurrentAlpha,0,1);
		CurrentWeight=CameraMontageBlend::GetCorrectBlendWeight(ECameraMontageBlendType::BlendOut,BlendInfo,CurrentAlpha); 
		CurrentWeight=FMath::Max(CurrentWeight,0);
		if(CurrentWeight<=0) { SetBlendType(ECameraMontageBlendType::WaitAdd);}
	}
}
void UPostBlendbaseMode::SetBlendType(ECameraMontageBlendType BlendType)
{
	PostProcessBlendType=BlendType;
	switch (BlendType)
	{
	case ECameraMontageBlendType::WaitAdd:
		CurrentAlpha=0.0f;
		CurrentWeight=0.0f;
		PlayedTime=0.0f;
		break;
	case ECameraMontageBlendType::BlendIn:
		{CurrentWeight=FMath::Clamp(CurrentWeight,0,1);
			FBlendCurveInfo BlendInfo=BlendData.BlendInCurveInfo;
			float InvExponent=(BlendInfo.BlendExp>0.0f)?(1.0f/BlendInfo.BlendExp):1.0f;
			CurrentAlpha=CameraData::BlendInfo::GetBlendWeightFromBlendInFunction(BlendInfo.BlendFunction,BlendInfo.BlendCurve,CurrentWeight,InvExponent);
			CurrentAlpha=FMath::Min(CurrentAlpha,1);
			break;
		}
	case ECameraMontageBlendType::Loop:
		CurrentAlpha=0.0f;
		break;
	case ECameraMontageBlendType::BlendOut:
		break;
	}
}
