// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraModeFold/ActionCameraMode.h"

#include "CameraManagerFold/ActionPlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "Engine/Canvas.h"
#include "GameFramework/Character.h"

UActionCameraMode::UActionCameraMode()
{
	CameraID=FGuid::NewGuid();
	UE_LOG(LogTemp,Warning,TEXT("Create"))
}

UActionCameraMode::~UActionCameraMode()
{
	UE_LOG(LogTemp,Warning,TEXT("Camera Mode Destory"))
}

void UActionCameraMode::UpdateCameraMode(float DeltaTime)
{
	UpdateView(DeltaTime);
	UpdateBlendWeight(DeltaTime);
}

void UActionCameraMode::UpdateView(float DeltaTime)
{
	//这里面更新位置
	switch (PlacedMode)
	{
	case ECameraPlacedMode::Settled:
		UpdateSettledView(DeltaTime);
		break;
	case ECameraPlacedMode::FreeMovement:
		UpdateFreeMovementView(DeltaTime);
		break;
	}
}

void UActionCameraMode::UpdateSettledView(float DeltaTime)
{
	ViewInfo=FixedViewInfo+CameraOffset;
	if(!SettledTargetActor)
	{
		if(SettledTargetActor=SetTargetActor();SettledTargetActor)
		{
			ViewInfo.CameraLocation= SettledTargetActor->GetActorLocation();
			UE_LOG(LogTemp,Warning,TEXT("Location %s"), *ViewInfo.CameraLocation.ToString())
			ViewInfo.CameraRotation= SettledTargetActor->GetActorRotation();
			ViewInfo.FOV=FixedViewInfo.FOV;
			ViewInfo=ViewInfo+CameraOffset;
			return;
		}
	}
	else
	{
		ViewInfo.CameraLocation= SettledTargetActor->GetActorLocation();
		UE_LOG(LogTemp,Warning,TEXT("Location %s"), *ViewInfo.CameraLocation.ToString())
		ViewInfo.CameraRotation= SettledTargetActor->GetActorRotation();
		ViewInfo.FOV=FixedViewInfo.FOV;
		ViewInfo=ViewInfo+CameraOffset;
	}
	
	
}

void UActionCameraMode::UpdateFreeMovementView(float DeltaTime)
{
	FVector PivotLocation=GetPivotLocation();
	FRotator PivotRotation=GetPivotRotation();
	PivotRotation.Pitch=FMath::ClampAngle(PivotRotation.Pitch,ViewPitchMin,ViewPitchMax);
	ViewInfo.CameraLocation=PivotLocation;
	ViewInfo.CameraRotation=PivotRotation;
	ViewInfo.FOV=FieldOfView;
	UpdateSpringArmFunc(ViewInfo,DeltaTime);
	//根据旋转量控制Fov
	if(OffsetMapCurve.FovInfo)
	{
		const float  TargetFovOffset=OffsetMapCurve.FovInfo->GetFloatValue(PivotRotation.Pitch);
		ViewInfo.FOV+=TargetFovOffset;
	}
		if(OffsetMapCurve.LocationInfo)
		{
			const FVector TargetVectorOffset=OffsetMapCurve.LocationInfo->GetVectorValue(GetPivotRotation().Pitch);
			ViewInfo.CameraLocation+=FRotationMatrix(ViewInfo.CameraRotation).TransformVector(TargetVectorOffset);
		}
	ViewInfo.CameraLocation=ViewInfo.CameraLocation+FRotationMatrix(ViewInfo.CameraRotation).TransformVector(CameraOffset.CameraLocation);
	ViewInfo.CameraRotation=ViewInfo.CameraRotation+CameraOffset.CameraRotation;
	ViewInfo.FOV=ViewInfo.FOV+CameraOffset.FOV;
}
FVector UActionCameraMode::GetPivotAfterOffsetLocation() const
{
	if(PlacedMode==ECameraPlacedMode::FreeMovement)
	{
		FRotator PivotRotation=GetPivotRotation();
		FVector PivotLocation=LastPivot+FRotationMatrix(PivotRotation).TransformVector(SpringArmInfo.CameraOffset);
		return PivotLocation;
	}
	return ViewInfo.CameraLocation;
}

void UActionCameraMode::UpdateSpringArmFunc(FActionCameraNormalViewInfo& CameraNormalViewInfo,float Deltatime)
{
	FVector ArmOrigin=FMath::VInterpTo(LastPivot,CameraNormalViewInfo.CameraLocation,Deltatime,GetPlayerCameraManager()->PivotLagSpeed);
	FVector DesiredLoc=ArmOrigin+SpringArmInfo.TargetOffset;
	LastPivot=ArmOrigin;
	DesiredLoc-=CameraNormalViewInfo.CameraRotation.Vector()*SpringArmInfo.SpringArmLens;
	DesiredLoc+=FRotationMatrix(CameraNormalViewInfo.CameraRotation).TransformVector(SpringArmInfo.CameraOffset);
	CameraNormalViewInfo.CameraLocation=DesiredLoc;

}	

void UActionCameraMode::UpdateBlendWeight(float DeltaTime)
{
	if(CurrentBlendMode==EBlendType::WaitAdd ||CurrentBlendMode==EBlendType::Loop)
	{
		return;
	}
	//最后如果都不是那就是混入了,如果混入大于等于1那么就变成
	if(CurrentBlendMode==EBlendType::BlendIn)
	{
		//这里是正向推导Weight所以可以正常使用
		BlendAlpha+=DeltaTime/BlendCurveInfo.BlendTime;
		BlendAlpha=FMath::Min(BlendAlpha,1);
		UCurveFloat * BlendCurve=nullptr;
		if(BlendCurveInfo.BlendFunction==EActionCameraBlendFunction::CustomCurve )
		{
			BlendCurve=BlendCurveInfo.bUseRuntimeCurve?BlendCurveInfo.CustomBlendCurve.ExternalCurve.Get():BlendCurveInfo.BlendCurve;
		}
		//进行权重混合
		BlendWeight=CameraData::BlendInfo::GetBlendWeightFromBlendInFunction(BlendCurveInfo.BlendFunction,
			BlendCurve,BlendAlpha,BlendCurveInfo.BlendExp);
		BlendWeight=FMath::Min(BlendWeight,1);
		if(BlendWeight>=1) {SetBlendWeight(EBlendType::Loop); return;}
	}
}

AActionPlayerCameraManager * UActionCameraMode::GetPlayerCameraManager() const 
{
	return CastChecked<AActionPlayerCameraManager>(GetTypedOuter<AActionPlayerCameraManager>());
}

AActor* UActionCameraMode::GetTargetActor() const 
{
	AActionPlayerCameraManager * Manager=GetPlayerCameraManager();
	if(Manager)
	{
		return Manager->ControlledPlayer;
	}
	return nullptr;
}

void UActionCameraMode::DrawDebug(UCanvas * Canvas)
{
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;
	DisplayDebugManager.DrawString(FString::Printf(TEXT("CameraMode %s"),*this->GetName()));
	DisplayDebugManager.DrawString(FString::Printf(TEXT("CurrentModeAlpha %f"),BlendAlpha));
	DisplayDebugManager.DrawString(FString::Printf(TEXT("CurrentModeWeight %f"),BlendWeight));
}

AActor* UActionCameraMode::SetTargetActor_Implementation()
{
	return nullptr;
}

void UActionCameraMode::SetBlendWeight(EBlendType BlendType,float Weight)
{	CurrentBlendMode=BlendType;
	//如果切换为WaitToAdd的话那么就退出。
	if(BlendType==EBlendType::WaitAdd)
	{
		BlendWeight=0.0f;
		BlendAlpha=0.0f;
		
	}
	//如果设置为混入那么就重新设置权重
	if(BlendType==EBlendType::BlendIn)
	{
		OnBlendIn();
		//这里是反向推导Alpha所以需要反过来。
		BlendWeight=FMath::Clamp(Weight,0,1);
		float InvExponent=(BlendCurveInfo.BlendExp>0.0f)?(1.0f/BlendCurveInfo.BlendExp):1.0f;
		BlendAlpha=CameraData::BlendInfo::GetBlendWeightFromBlendInFunction(BlendCurveInfo.BlendFunction,BlendCurveInfo.BlendCurve,BlendWeight,InvExponent	);
		BlendAlpha=FMath::Min(BlendWeight,1);
	}
	if(BlendType==EBlendType::Loop)
	{
		OnLoop();
	}
}
FActionCameraNormalViewInfo UActionCameraMode::GetActionCameraOffsetValue()
{
	return CameraOffset;
}

FActionCameraNormalViewInfo UActionCameraMode::GetActionCameraViewInfo() const 
{
	return ViewInfo;
}

void UActionCameraMode::OnActivation()
{
	OnActivationBP();
}

void UActionCameraMode::OnActivationBP_Implementation()
{
}

void UActionCameraMode::OnBlendIn_Implementation()
{
}

void UActionCameraMode::OnLoop_Implementation()
{
}
FVector UActionCameraMode::GetPivotLocation() const
{
	if(PlacedMode==ECameraPlacedMode::Settled)
	{
		return ViewInfo.CameraLocation;
	}
	if(PlacedMode==ECameraPlacedMode::FreeMovement)
	{
		const AActor* TargetActor = GetTargetActor();
		check(TargetActor);

		if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
		{
			// Height adjustments for characters to account for crouching.
			if (const ACharacter* TargetCharacter = Cast<ACharacter>(TargetPawn))
			{
				const ACharacter* TargetCharacterCDO = TargetCharacter->GetClass()->GetDefaultObject<ACharacter>();
				check(TargetCharacterCDO);

				const UCapsuleComponent* CapsuleComp = TargetCharacter->GetCapsuleComponent();
				check(CapsuleComp);

				const UCapsuleComponent* CapsuleCompCDO = TargetCharacterCDO->GetCapsuleComponent();
				check(CapsuleCompCDO);

				const float DefaultHalfHeight = CapsuleCompCDO->GetUnscaledCapsuleHalfHeight();
				const float ActualHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
				const float HeightAdjustment = (DefaultHalfHeight - ActualHalfHeight) + TargetCharacterCDO->BaseEyeHeight;

				return TargetCharacter->GetActorLocation() + (FVector::UpVector * HeightAdjustment);
			}
			
			return TargetPawn->GetPawnViewLocation();
		}
		
		return TargetActor->GetActorLocation();
	}
	return FVector(0,0,0);
}

FRotator UActionCameraMode::GetPivotRotation() const
{
	if(PlacedMode==ECameraPlacedMode::Settled)
	{
		return ViewInfo.CameraRotation;
	}
	if(PlacedMode==ECameraPlacedMode::FreeMovement)
	{
		const AActor* TargetActor = GetTargetActor();
		check(TargetActor);

		if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
		{
			return TargetPawn->GetViewRotation();
		}

		return TargetActor->GetActorRotation();
	}
	return FRotator(0,0,0);
}



