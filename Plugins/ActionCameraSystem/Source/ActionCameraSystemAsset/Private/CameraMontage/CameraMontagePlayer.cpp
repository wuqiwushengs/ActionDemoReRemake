// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraComponentFold/CameraMontage/CameraMontagePlayer.h"
#include "CameraComponentFold/CameraMontage/CameraMontageInterface.h"
#include "CameraManagerFold/ActionPlayerCameraManager.h"
#include "Engine/Canvas.h"
#include "GameFramework/Character.h"

namespace ActionViewInterp
{
	FActionCameraNormalViewInfo InterpActionView(float DeltaTime,bool bFovInterp,float interpFovSpeed,bool bLocationInterp,float InterpLocationSpeed,bool bRotationInterp,float InterpRotationSpeed,const FActionCameraNormalViewInfo& FromViewInfo,const FActionCameraNormalViewInfo& ToViewInfo)
	{	FVector CameraLocation;
		FRotator CameraRotation;
		float Fov;
		if(bLocationInterp)
		{
			CameraLocation=FMath::VInterpTo(FromViewInfo.CameraLocation,ToViewInfo.CameraLocation,DeltaTime,InterpLocationSpeed);
		}
		else
		{
			CameraLocation=ToViewInfo.CameraLocation;	
		}
		if(bRotationInterp)
		{
			CameraRotation=FMath::RInterpTo(  FromViewInfo.CameraRotation,ToViewInfo.CameraRotation,DeltaTime,InterpRotationSpeed);
		
		}
		else
		{
			CameraRotation=ToViewInfo.CameraRotation;
		}
		if(bFovInterp)
		{
			Fov=FMath::FInterpTo(FromViewInfo.FOV,ToViewInfo.FOV,DeltaTime,interpFovSpeed);
		}
		else
		{
			Fov=ToViewInfo.FOV;
		}
		return FActionCameraNormalViewInfo(CameraLocation,CameraRotation,Fov);
	}
}
UCameraMontageSequence::UCameraMontageSequence()
{
	id=FGuid::NewGuid();
}
void UCameraMontageSequence::SetBlendType(ECameraMontageBlendType BlendType)
{
	CameraMontageBlendType=BlendType;
	switch (BlendType)
	{
	case ECameraMontageBlendType::WaitAdd:
		CurrentBlendAlpha=0.0f;
		CurrentBlendWeight=0.0f;
		PlayedTime=0.0f;
		break;
	case ECameraMontageBlendType::BlendIn:
		{CurrentBlendWeight=FMath::Clamp(CurrentBlendWeight,0,1);
		FBlendCurveInfo BlendInfo=CameraAnimMontageInfo.MontageInfo.BlendInfo.BlendInCurveInfo;
		float InvExponent=(BlendInfo.BlendExp>0.0f)?(1.0f/BlendInfo.BlendExp):1.0f;
		CurrentBlendAlpha=CameraData::BlendInfo::GetBlendWeightFromBlendInFunction(BlendInfo.BlendFunction,BlendInfo.BlendCurve,CurrentBlendWeight,InvExponent);
		CurrentBlendAlpha=FMath::Min(CurrentBlendAlpha,1);
		break;
		}
	case ECameraMontageBlendType::Loop:
		CurrentBlendAlpha=0.0f;
		break;
	case ECameraMontageBlendType::BlendOut:
		break;
	}
}

void UCameraMontageSequence::OnDeactivate()
{
	if(CameraAnimMontageInfo.MontageInfo.MontageType==ECameraMontageType::AnimSequenceMontage&&CameraAnimMontageInfo.MontageInfo.CameraSequence.CameraSequence)
	{
		CameraAnimMontageInfo.MontageInfo.CameraSequence.GetCameraAnimationSequencePlayerInstance(this)->Stop();
	}
	
}

void UCameraMontageSequence::OnActive()
{
	if(CameraAnimMontageInfo.MontageInfo.MontageType==ECameraMontageType::AnimSequenceMontage)
	{
		CameraAnimMontageInfo.MontageInfo.CameraSequence.InitializeCameraSequence(this);
		CameraAnimMontageInfo.MontageInfo.CameraSequence.GetCameraAnimationSequencePlayerInstance(this)->Play(true,false);
	}
}

void UCameraMontageSequence::UpdateCameraMontageSequence(float DeltaTime,FActionCameraNormalViewInfo & MontageViewInfo)
{
	UpdateMontageInfo(DeltaTime, MontageViewInfo);
	UpdateBlendInfo(DeltaTime);
}
AActor* UCameraMontageSequence::SetTargetActor_Implementation()
{
	return  GetControlledPlayer();
}

APawn* UCameraMontageSequence::GetControlledPlayer()
{
	AActionPlayerCameraManager * CameraManager=GetTypedOuter<AActionPlayerCameraManager>();
	if(CameraManager)
	{
		return CameraManager->ControlledPlayer;
	}
	return nullptr;
}

FVector UCameraMontageSequence::GetSocketLocation()
{
	FVector ActorLocation=GetTargetActor()->GetActorLocation();
	if(CameraAnimMontageInfo.SocketName.IsNone()) return ActorLocation;
	 ACharacter *Character= Cast<ACharacter>(GetTargetActor());
	switch (CameraAnimMontageInfo.SocketType)
	{
	case EMontageSocketType::CharacterSocket:
		 return  Character->GetMesh()->GetSocketLocation(CameraAnimMontageInfo.SocketName);
	case EMontageSocketType::CharacterBoneSocket:
		return Character->GetMesh()->GetBoneLocation(CameraAnimMontageInfo.SocketName);
	}
	return ActorLocation;
}

void UCameraMontageSequence::UpdateMontageInfo(float DeltaTime,FActionCameraNormalViewInfo & MontageViewInfo)
{
	//通过工具进行蒙太奇内容的更新
	UCameraMontagePlayer * Player=Cast<UCameraMontagePlayer>(GetOuter());
	FActionCameraNormalViewInfo ViewInfo=MontageViewInfo;
	Player->GetCameraMontageValueCalculateFactory()->CalculateMontageValue(ViewInfo,this,CameraAnimMontageInfo.CameraMontageType,CurrentBlendWeight);
	MontageViewInfo=ViewInfo;
}

void UCameraMontageSequence::UpdateBlendInfo(float DeltaTime)
{
	PlayedTime+=DeltaTime;
	if(CameraMontageBlendType==ECameraMontageBlendType::WaitAdd) return;
	if(CameraMontageBlendType==ECameraMontageBlendType::Loop)
	{
		if(CameraAnimMontageInfo.MontageInfo.bLoop) return;
		if(PlayedTime>=(CameraAnimMontageInfo.MontageInfo.DurationTime-CameraAnimMontageInfo.MontageInfo.BlendInfo.BlendOutCurveInfo.BlendTime))
		{
			SetBlendType(ECameraMontageBlendType::BlendOut);
			return;
		}
	}
	if(CameraMontageBlendType==ECameraMontageBlendType::BlendIn)
	{
		FBlendData BlendInfo=CameraAnimMontageInfo.MontageInfo.BlendInfo;
		CurrentBlendAlpha+=DeltaTime/BlendInfo.BlendInCurveInfo.BlendTime;
		CurrentBlendAlpha=FMath::Clamp(CurrentBlendAlpha,0,1);
		CurrentBlendWeight=CameraMontageBlend::GetCorrectBlendWeight(ECameraMontageBlendType::BlendIn,BlendInfo,CurrentBlendAlpha); 
		CurrentBlendWeight=FMath::Min(CurrentBlendWeight,1);
		if(CurrentBlendWeight>=1) { SetBlendType(ECameraMontageBlendType::Loop);}
	}
	if(CameraMontageBlendType==ECameraMontageBlendType::BlendOut)
	{
		FBlendData BlendInfo=CameraAnimMontageInfo.MontageInfo.BlendInfo;
		CurrentBlendAlpha+=DeltaTime/BlendInfo.BlendOutCurveInfo.BlendTime;
		CurrentBlendAlpha=FMath::Clamp(CurrentBlendAlpha,0,1);
		CurrentBlendWeight=CameraMontageBlend::GetCorrectBlendWeight(ECameraMontageBlendType::BlendOut,BlendInfo,CurrentBlendAlpha); 
		CurrentBlendWeight=FMath::Max(CurrentBlendWeight,0);
		if(CurrentBlendWeight<=0) { SetBlendType(ECameraMontageBlendType::WaitAdd);}
	}
}

void UCameraMontageSequence::DisplayDebug(UCanvas* Canvas)
{
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;
	DisplayDebugManager.DrawString(FString::Printf(TEXT("CameraMontageSequence %s"),*this->GetName()));
	const UEnum * Enum=StaticEnum<ECameraMontageBlendType>();
	FString EnumName=Enum->GetNameStringByIndex(static_cast<int32>(GetBlendType()));
	DisplayDebugManager.DrawString(FString::Printf(TEXT("CurrentMontageState %s"),*EnumName));
	DisplayDebugManager.DrawString(FString::Printf(TEXT("CurrentBlendAlpha %f"),CurrentBlendAlpha));
	DisplayDebugManager.DrawString(FString::Printf(TEXT("CurrentBlendWeight %f"),CurrentBlendWeight));
}
UCameraMontagePlayer::UCameraMontagePlayer()
{
	CameraMontageSequenceModifyInstances.Empty();
	CameraMontageSequencesAdditiveInstances.Empty();
	CameraMontagePlayModifyStack=nullptr;
	CameraMontagePlayAdditiveStack.Empty();
}
void UCameraMontagePlayer::PushCameraMontageSequence(TSubclassOf<UCameraMontageSequence> MontageSequence)
{
	UCameraMontageSequence * Sequence=GetSequenceInstance(MontageSequence);
	if(!Sequence) return;
	switch (Sequence->CameraAnimMontageInfo.CameraMontageType)
	{
	case ECameraMontagePlayType::Additive:
		PushAdditiveMontage(Sequence);
		break;
	case ECameraMontagePlayType::Modify:
		PushModifyMontage(Sequence);
		break;
	}
}
void UCameraMontagePlayer::DeactivateAdditiveMontageByTag(FGameplayTag GameplayTag)
{	if(GameplayTag.IsValid()) return; 
	TSet<int> RemoveIndex;
	for (int i=0;auto Var:CameraMontagePlayAdditiveStack)
	{		if(Var->GetMontageGameplayTag()==GameplayTag)
			{
				RemoveIndex.Add(i);
			}
			i++;
	}
	if(RemoveIndex.Num()<=0) return;
	for (int index : RemoveIndex)
	{
		CameraMontagePlayAdditiveStack.RemoveAt(index);
	}
}
void UCameraMontagePlayer::DeactivateAdditiveMontageByClass(TSubclassOf<UCameraMontageSequence> MontageClass)
{
	if(!MontageClass) return;
	TSet<int> RemoveIndex;
	for(int i=0;auto Var:CameraMontagePlayAdditiveStack)
	{
		if(Var->GetClass()==MontageClass->GetClass())
		{
			RemoveIndex.Add(i);
		}
	}
	if(RemoveIndex.Num()<=0) return;
	for (int index:RemoveIndex)
	{
		CameraMontagePlayAdditiveStack.RemoveAt(index);
	}
}
void UCameraMontagePlayer::DeactivateModifyMontage()
{
	CameraMontagePlayModifyStack->SetBlendType(ECameraMontageBlendType::WaitAdd);
	CameraMontagePlayModifyStack=nullptr;
}

void UCameraMontagePlayer::DisplayDebug(UCanvas* Canvas)
{
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;
	DisplayDebugManager.DrawString(FString::Printf( TEXT("CurrentCameraMontagePlayer %s"),*this->GetName()));
	DisplayDebugManager.DrawString(FString::Printf(TEXT("LocationlagSpeed %f \n RotationLagSpeed %f \n  FovLagSpeed %f \n "),CameraLogSpeed,CameraRotationLagSpeed,CameraFovLagSpeed));
	if(CameraMontagePlayModifyStack)
	{
		CameraMontagePlayModifyStack->DisplayDebug(Canvas);
	}
	if(CameraMontagePlayAdditiveStack.Num()>0)
	{
		for (auto var:CameraMontagePlayAdditiveStack)
		{
			var->DisplayDebug(Canvas);
		}
	}
}
void UCameraMontagePlayer::PushAdditiveMontage(UCameraMontageSequence* Sequence)
{
	CameraMontagePlayAdditiveStack.Insert(Sequence,0);
	Sequence->SetBlendType(ECameraMontageBlendType::BlendIn);
	Sequence->OnActive();
}
void UCameraMontagePlayer::PushModifyMontage(UCameraMontageSequence* Sequence)
{
		if(CameraMontagePlayModifyStack)
		{
			CameraMontagePlayModifyStack->SetBlendType(ECameraMontageBlendType::WaitAdd);
		}
		CameraMontagePlayModifyStack=nullptr;
		CameraMontagePlayModifyStack=Sequence;
		Sequence->SetBlendType(ECameraMontageBlendType::BlendIn);
		Sequence->OnActive();
		
}
void UCameraMontagePlayer::UpdateCameraMontageStack(float DeltaTime)
{
	if(CameraMontagePlayModifyStack&& CameraMontagePlayModifyStack->GetBlendType()==ECameraMontageBlendType::WaitAdd)
	{
		CameraMontagePlayModifyStack->OnDeactivate();
		CameraMontagePlayModifyStack=nullptr;
	}
	TSet<int> RemoveIndex;
	if(CameraMontagePlayAdditiveStack.Num()>0)
	{
		for (int i=0; auto Var : CameraMontagePlayAdditiveStack )
		{
			if(Var->GetBlendType()==ECameraMontageBlendType::WaitAdd)
			{
				RemoveIndex.Add(i);
			}
			i++;
		}
		if(RemoveIndex.Num()<=0) return;
		for (int index:RemoveIndex)
		{
			CameraMontagePlayAdditiveStack[index]->OnDeactivate();
			CameraMontagePlayAdditiveStack.RemoveAt(index);
		}
	}
	
}
UCameraMontageSequence* UCameraMontagePlayer::GetSequenceInstance(TSubclassOf<UCameraMontageSequence> SequenceClass)
{
	if(!SequenceClass) return nullptr;
	UCameraMontageSequence *Sequence=nullptr;
	//这里尝试进行性能优化但是Additive无法优化所以赋值object了
	switch (SequenceClass->GetDefaultObject<UCameraMontageSequence>()->CameraAnimMontageInfo.CameraMontageType)
	{
	case ECameraMontagePlayType::Additive:
		for(auto Var:CameraMontageSequencesAdditiveInstances)
		{
			if(Var->GetClass()==SequenceClass)
			{
				return DuplicateObject(Var,this);
			}
		}
		Sequence=NewObject<UCameraMontageSequence>(this,SequenceClass);
		CameraMontageSequencesAdditiveInstances .Add(Sequence);
	return Sequence;
	case ECameraMontagePlayType::Modify:
		for(auto	Var:CameraMontageSequenceModifyInstances)
		{
			if(Var->GetClass()==SequenceClass)
			{
 			return Var;
			}
		}
		Sequence=NewObject<UCameraMontageSequence>(this,SequenceClass);
		CameraMontageSequenceModifyInstances.Add(Sequence);
		return NewObject<UCameraMontageSequence>(this,SequenceClass);
	}
	return nullptr;
}

FCameraMontageValueCalculateFactory* UCameraMontagePlayer::GetCameraMontageValueCalculateFactory()
{
	if(!CameraMontageValueCalculateFactory)
	{
		CameraMontageValueCalculateFactory=MakeShareable(new FCameraMontageValueCalculateFactory);
	}
	return CameraMontageValueCalculateFactory.Get();
}

void UCameraMontagePlayer::UpdateCameraMontagePlay(float DeltaTime, FActionCameraNormalViewInfo& NormalViewInfo,FActionCameraNormalViewInfo & NormalViewInfoCache)
{
	UpdateCameraMontageStack(DeltaTime);
	//场景修改后的位置。
	FActionCameraNormalViewInfo ModifyViewInfo;
	//世界场景偏移 经过工具类计算之后的偏移最后都是世界方向的偏转了。
	FActionCameraNormalViewInfo AdditiveWorldViewInfo;
	UpdateCameraMontageModifyPlay(DeltaTime,ModifyViewInfo,NormalViewInfoCache);
	UpdateCameraMontageAdditivePlay(DeltaTime,AdditiveWorldViewInfo);
	ProcessFindChangedMontage(DeltaTime,NormalViewInfoCache,AdditiveWorldViewInfo,ModifyViewInfo,NormalViewInfo);


}
void UCameraMontagePlayer::UpdateCameraMontageAdditivePlay(float DeltaTime,FActionCameraNormalViewInfo& InAdditiveViewInfo)
{
	if(CameraMontagePlayAdditiveStack.Num()>0)
	{
		for (auto Var:CameraMontagePlayAdditiveStack)
		{
			Var->UpdateCameraMontageSequence(DeltaTime,InAdditiveViewInfo);
		}
	}
}
void UCameraMontagePlayer::UpdateCameraMontageModifyPlay(float DeltaTime, FActionCameraNormalViewInfo& InModifyViewInfo,FActionCameraNormalViewInfo & NormalViewInfoCache)
{
	if(CameraMontagePlayModifyStack)
	{
		CameraMontagePlayModifyStack->UpdateCameraMontageSequence(DeltaTime,InModifyViewInfo);
		InModifyViewInfo.Blend(NormalViewInfoCache,1-CameraMontagePlayModifyStack->GetCurrentWeight());
	}
}
void UCameraMontagePlayer::ProcessFindChangedMontage(float DeltaTime, const FActionCameraNormalViewInfo& InitialViewInfo,const FActionCameraNormalViewInfo &InAdditiveViewInfo, FActionCameraNormalViewInfo InModifyViewInfo
	,FActionCameraNormalViewInfo & ApplyViewInfo)
{	if(!CameraMontagePlayModifyStack&&CameraMontagePlayAdditiveStack.Num()<=0) return;
	//没有修改的情况
	FActionCameraNormalViewInfo FinalViewInfo;
	if(!CameraMontagePlayModifyStack)
	{
		FinalViewInfo=InitialViewInfo+InAdditiveViewInfo;
	}
	else
	{
		FinalViewInfo=InModifyViewInfo+InAdditiveViewInfo;
	}
	FinalViewInfo=ActionViewInterp::InterpActionView(DeltaTime,bCameraFovLag,CameraFovLagSpeed
		,bCameraLocationLag,CameraLogSpeed,bCameraRotationLag,CameraRotationLagSpeed,InitialViewInfo
		,FinalViewInfo);
	ApplyViewInfo=FinalViewInfo;
	
	
}

