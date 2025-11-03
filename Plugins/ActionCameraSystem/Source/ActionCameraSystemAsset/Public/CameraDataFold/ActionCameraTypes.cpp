#include "ActionCameraTypes.h"
#include "CameraAnimationSequence.h"
#include "CameraManagerFold/ActionPlayerCameraManager.h"
namespace  CameraGlobalFunc
{
	 AActionPlayerCameraManager* TryGetPlayerCameraManager(const AActor* Owner)
	{	check(Owner)
		if(!Owner) return nullptr;
		APlayerController * Controller=Cast<APlayerController>(Owner->GetInstigatorController());
		if(Controller)
		{
			if(AActionPlayerCameraManager * APCM=Cast<AActionPlayerCameraManager>(Controller->PlayerCameraManager); APCM)
			{
				return  APCM;
			}
			return  nullptr;
		}
		UE_LOG(LogTemp,Warning,TEXT("Can't find Real CameraManager"))
		return nullptr;
	}
}


void FCameraMontageSequenceInfo::InitializeCameraSequence(UObject * Owner)
{
	if(!CameraSequence|| !Owner)
	{
		UE_LOG(LogTemp,Warning,TEXT("Invalid CameraAnimSequence"))
		return;
	}
	UCameraAnimationSequenceCameraStandIn * StandIn=GetCameraStandInInstance(Owner);
	UCameraAnimationSequencePlayer * SequencePlayer=GetCameraAnimationSequencePlayerInstance(Owner);
	if(!StandIn||!SequencePlayer)
	{
		UE_LOG(LogTemp,Warning,TEXT("Failded to create CameraStandIn or Player"))
		return;
	}
	StandIn->Initialize(CameraSequence);
	SequencePlayer->SetBoundObjectOverride(StandIn);
	SequencePlayer->Initialize(CameraSequence,0.0f);
}

void FCameraMontageSequenceInfo::GetCameraDataAtTime(float TimeInSeconds, FMinimalViewInfo& OutCameraData,UObject * Owner)
{
	if(!CameraStandInInstance || !SequencePlayerInstance)
	{
		if(!Owner) return;
	}
	FFrameRate FrameRate=SequencePlayerInstance->GetInputRate();
	FFrameTime TargetTime=TimeInSeconds*FrameRate;
	SequencePlayerInstance->Update(TargetTime);
	CameraStandInInstance->RecalcDerivedData();
	const FTransform AnimatedTransform =CameraStandInInstance->GetTransform();
	OutCameraData.Location=AnimatedTransform.GetLocation();
	OutCameraData.Rotation=AnimatedTransform.GetRotation().Rotator();
}
