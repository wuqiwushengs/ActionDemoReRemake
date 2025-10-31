#include "CameraMontageInterface.h"
#include "CameraMontagePlayer.h"
#include "CameraManagerFold/ActionPlayerCameraManager.h"
#include "Curves/CurveVector.h"

namespace CameraMontageBlend
{
	float GetCorrectBlendWeight(ECameraMontageBlendType MontageBlendType, FBlendData BlendInfo,float BlendAlpha)
	{
		UCurveFloat * CurveFloat=nullptr;
		switch (MontageBlendType)
		{
		case ECameraMontageBlendType::WaitAdd:
			return 0.0f;
		case  ECameraMontageBlendType::Loop:
			return 1.0f;
		case  ECameraMontageBlendType::BlendIn:
			if(BlendInfo.BlendInCurveInfo.BlendCurve) CurveFloat=BlendInfo.BlendInCurveInfo.BlendCurve;
			if(BlendInfo.BlendInCurveInfo.CustomBlendCurve.ExternalCurve.Get()) CurveFloat=BlendInfo.BlendInCurveInfo.CustomBlendCurve.ExternalCurve.Get();
			return CameraData::BlendInfo::GetBlendWeightFromBlendInFunction(BlendInfo.BlendInCurveInfo.BlendFunction,CurveFloat,BlendAlpha,BlendInfo.BlendInCurveInfo.BlendExp); 
		case ECameraMontageBlendType::BlendOut:
			if(BlendInfo.BlendOutCurveInfo.BlendCurve) CurveFloat=BlendInfo.BlendOutCurveInfo.BlendCurve;
			if(BlendInfo.BlendOutCurveInfo.CustomBlendCurve.ExternalCurve.Get()) CurveFloat=BlendInfo.BlendOutCurveInfo.CustomBlendCurve.ExternalCurve.Get();
			return CameraData::BlendInfo::GetBlendWeightFromBlendOutFunction(BlendInfo.BlendOutCurveInfo.BlendFunction,CurveFloat,BlendAlpha,BlendInfo.BlendOutCurveInfo.BlendExp); 
		default:
			return 0.0f;
		}
	}

	FPivotInfo GetCorrectAdditivePivot(EAdditiveType AdditiveType, AActor* FollowTarget)
	{
		FPivotInfo PivotInfo;
		AActionPlayerCameraManager * PlayerCameraManager=CameraGlobalFunc::TryGetPlayerCameraManager(FollowTarget);
		FVector CameraLocation=PlayerCameraManager->GetLastUpdateCameraNormalViewInfo().CameraLocation;
		FRotator CameraRotation=PlayerCameraManager->GetLastUpdateCameraNormalViewInfo().CameraRotation;
		switch (AdditiveType)
		{
		case EAdditiveType::CameraLocation:
			PivotInfo.OriginLocation=CameraLocation;
			PivotInfo.OriginRotator=CameraRotation;
			PivotInfo.ForwardVector=FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Type::X);
			PivotInfo.RightVector=FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Type::Y);
			PivotInfo.UpVector=FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Type::Z);
			break;
		case EAdditiveType::WorldLocation:
			PivotInfo.OriginLocation=FVector(0,0,0);
			PivotInfo.OriginRotator=FRotator(0,0,0);
			PivotInfo.ForwardVector=FVector(1,0,0);
			PivotInfo.RightVector=FVector(0,1,0);
			PivotInfo.UpVector=FVector(0,0,1);
			break;
		case EAdditiveType::TargetActorLocation:
			PivotInfo.OriginLocation=FollowTarget->GetActorLocation();
			PivotInfo.OriginRotator=FollowTarget->GetActorRotation();
			PivotInfo.ForwardVector=FRotationMatrix(PivotInfo.OriginRotator).GetUnitAxis(EAxis::Type::X);
	
			PivotInfo.RightVector=FRotationMatrix(PivotInfo.OriginRotator).GetUnitAxis(EAxis::Type::Y);
			PivotInfo.UpVector=FRotationMatrix(PivotInfo.OriginRotator).GetUnitAxis(EAxis::Type::Z);
			break;
		case EAdditiveType::CameraToTargetLocation:
			PivotInfo.OriginLocation=CameraLocation;
			PivotInfo.OriginRotator= (FollowTarget->GetActorLocation()-CameraLocation).Rotation();
			PivotInfo.ForwardVector=FRotationMatrix(PivotInfo.OriginRotator).GetUnitAxis(EAxis::Type::X);
			PivotInfo.RightVector=FRotationMatrix(PivotInfo.OriginRotator).GetUnitAxis(EAxis::Type::Y);
			PivotInfo.UpVector=FRotationMatrix(PivotInfo.OriginRotator).GetUnitAxis(EAxis::Type::Z);
			break;
		}
		return PivotInfo;
	}

	FMinimalViewInfo GetAnimationSequenceTransformInfo( UCameraMontageSequence* Sequence)
	{
		FMinimalViewInfo ViewInfo;
		float PlayTime= FMath::Fmod(Sequence->GetPlayedTime(),Sequence->CameraAnimMontageInfo.MontageInfo.DurationTime);
		Sequence->CameraAnimMontageInfo.MontageInfo.CameraSequence.GetCameraDataAtTime(PlayTime,ViewInfo);
		return ViewInfo;
	}
}
//仅仅用于处理数据
//FixTransform
void FCameraMontageFixTransformFactory::ProcessCameraMontagePlayAdditive(FActionCameraNormalViewInfo& AdditiveViewInfo,
	UCameraMontageSequence* Sequence,float BlendWeight)
{
	 FActionCameraNormalViewInfo ModifyInfo=Sequence->CameraAnimMontageInfo.MontageInfo.FixedRelativeTransformInfo.FixedViewInfo;
	 FPivotInfo PivotInfo=CameraMontageBlend::GetCorrectAdditivePivot(Sequence->CameraAnimMontageInfo.AdditiveType,Sequence->GetTargetActor());
	AdditiveViewInfo.CameraLocation=PivotInfo.GetCorrectLocation(AdditiveViewInfo.CameraLocation,ModifyInfo.CameraLocation*BlendWeight);
	AdditiveViewInfo.CameraRotation+=ModifyInfo.CameraRotation*BlendWeight;

	AdditiveViewInfo.FOV+=ModifyInfo.FOV*BlendWeight;
}

void FCameraMontageFixTransformFactory::ProcessCameraMontagePlayModify(FActionCameraNormalViewInfo& ModifyViewInfo,
	UCameraMontageSequence* Sequence)
{
	FActionCameraNormalViewInfo ModifyInfo=Sequence->CameraAnimMontageInfo.MontageInfo.FixedRelativeTransformInfo.FixedViewInfo;
	//获取的旋转这里永远是Target的位置以及旋转等信息
	FPivotInfo PivotInfo=CameraMontageBlend::GetCorrectAdditivePivot(EAdditiveType::TargetActorLocation,Sequence->GetTargetActor());
	FVector SocketLocation=Sequence->GetSocketLocation();
	ModifyViewInfo.CameraLocation=PivotInfo.GetCorrectLocation(SocketLocation,ModifyInfo.CameraLocation);
	ModifyViewInfo.CameraRotation=ModifyInfo.CameraRotation;
	ModifyViewInfo.FOV=ModifyInfo.FOV;
}
//CurveMontage
void FCameraMontageCurveMontageFactory::ProcessCameraMontagePlayAdditive(FActionCameraNormalViewInfo& AdditiveViewInfo,
	UCameraMontageSequence* Sequence,float BlendWeight)
{
	FCameraMontageCurve MontageCurve=Sequence->CameraAnimMontageInfo.MontageInfo.MontageCurve;
	//并且从前-后-前 而是反复播放
	float PlayTime= FMath::Fmod(Sequence->GetPlayedTime(),Sequence->CameraAnimMontageInfo.MontageInfo.DurationTime);
	FVector Location=MontageCurve.LocationInfo?MontageCurve.LocationInfo->GetVectorValue(PlayTime):FVector(0,0,0);
	FVector RotatorVec=MontageCurve.RotationInfo? MontageCurve.RotationInfo->GetVectorValue(PlayTime):FVector(0,0,0);
	FRotator Rotator=FRotator(RotatorVec.Y,RotatorVec.Z,RotatorVec.X);
	float Fov=MontageCurve.FovInfo?MontageCurve.FovInfo->GetFloatValue(PlayTime):0.0f;
	FPivotInfo PivotInfo=CameraMontageBlend::GetCorrectAdditivePivot(Sequence->CameraAnimMontageInfo.AdditiveType,Sequence->GetTargetActor());
	AdditiveViewInfo.CameraLocation=PivotInfo.GetCorrectLocation(AdditiveViewInfo.CameraLocation,Location*BlendWeight);
	AdditiveViewInfo.CameraRotation+=Rotator*BlendWeight;
	AdditiveViewInfo.FOV+=Fov*BlendWeight;
}

void FCameraMontageCurveMontageFactory::ProcessCameraMontagePlayModify(FActionCameraNormalViewInfo& ModifyViewInfo,
	UCameraMontageSequence* Sequence)
{
	FCameraMontageCurve MontageCurve=Sequence->CameraAnimMontageInfo.MontageInfo.MontageCurve;
	//不是从前->后->前 而是反复播放 前后 前后 前后
	float PlayTime= FMath::Fmod(Sequence->GetPlayedTime(),Sequence->CameraAnimMontageInfo.MontageInfo.DurationTime);
	FVector Location=MontageCurve.LocationInfo?MontageCurve.LocationInfo->GetVectorValue(PlayTime):FVector(0,0,0);
	FVector RotatorVec=MontageCurve.RotationInfo? MontageCurve.RotationInfo->GetVectorValue(PlayTime):FVector(0,0,0);
	FRotator Rotator=FRotator(RotatorVec.Y,RotatorVec.Z,RotatorVec.X);
	float Fov=MontageCurve.FovInfo?MontageCurve.FovInfo->GetFloatValue(PlayTime):0.0f;
	FVector SocketLocation=Sequence->GetSocketLocation();
	FPivotInfo PivotInfo=CameraMontageBlend::GetCorrectAdditivePivot(EAdditiveType::TargetActorLocation,Sequence->GetTargetActor());
	ModifyViewInfo.CameraLocation=PivotInfo.GetCorrectLocation(SocketLocation,Location);
	ModifyViewInfo.CameraRotation=Rotator;
	ModifyViewInfo.FOV=Fov;
}
//AnimSequenceMontage
void FCameraMontageAnimSequenceMontageFactory::ProcessCameraMontagePlayAdditive(
	FActionCameraNormalViewInfo& AdditiveViewInfo, UCameraMontageSequence* Sequence,float BlendWeight)
{
	FMinimalViewInfo MinimalViewInfo=CameraMontageBlend::GetAnimationSequenceTransformInfo(Sequence);
	FPivotInfo PivotInfo=CameraMontageBlend::GetCorrectAdditivePivot(Sequence->CameraAnimMontageInfo.AdditiveType,Sequence->GetTargetActor());
	AdditiveViewInfo.CameraLocation=PivotInfo.GetCorrectLocation(AdditiveViewInfo.CameraLocation,MinimalViewInfo.Location*BlendWeight);
	AdditiveViewInfo.CameraRotation+=MinimalViewInfo.Rotation*BlendWeight;
	AdditiveViewInfo.FOV+=MinimalViewInfo.FOV*BlendWeight;
}

void FCameraMontageAnimSequenceMontageFactory::ProcessCameraMontagePlayModify(
	FActionCameraNormalViewInfo& ModifyViewInfo, UCameraMontageSequence* Sequence)
{
	FMinimalViewInfo MinimalViewInfo=CameraMontageBlend::GetAnimationSequenceTransformInfo(Sequence);
	FPivotInfo PivotInfo=CameraMontageBlend::GetCorrectAdditivePivot(EAdditiveType::TargetActorLocation,Sequence->GetTargetActor());
	FVector SocketLocation=Sequence->GetSocketLocation();
	ModifyViewInfo.CameraLocation=PivotInfo.GetCorrectLocation(SocketLocation,MinimalViewInfo.Location);
	ModifyViewInfo.CameraRotation=MinimalViewInfo.Rotation;
	ModifyViewInfo.FOV=MinimalViewInfo.FOV;
}

void FCameraMontageValueCalculateFactory::CalculateMontageValue(FActionCameraNormalViewInfo& ModifyValue,
	UCameraMontageSequence* Sequence,ECameraMontagePlayType MontagePlayType,float BlendWeight)
{
	check(Sequence)
	if(!Sequence) return;
 	ICameraMontagePlayPattern * Pattern=CalculatePattern.Find(Sequence->CameraAnimMontageInfo.MontageInfo.MontageType)->Get();
	switch (MontagePlayType)
	{
	case ECameraMontagePlayType::Additive:
		Pattern->ProcessCameraMontagePlayAdditive(ModifyValue,Sequence,BlendWeight);
		break;
	case ECameraMontagePlayType::Modify:
		Pattern->ProcessCameraMontagePlayModify(ModifyValue,Sequence);
		break;
	}
}

void FCameraMontageValueCalculateFactory::AddReferencedObjects(FReferenceCollector& Collector)
{
	
}

FString FCameraMontageValueCalculateFactory::GetReferencerName() const
{
	return TEXT("FCameraMontageValueCalculateFactory");
}


