// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraModeFold/ActionCameraStack.h"

#include "CameraManagerFold/ActionPlayerCameraManager.h"
#include "CameraModeFold/ActionCameraMode.h"

UActionCameraStack::UActionCameraStack()
{
	bisActive=true;
}

void UActionCameraStack::ActiveStack()
{
	if(!bisActive)
	{
		bisActive=true;
		for(UActionCameraMode * CameraMode : CameraModeStack)
		{
			check(CameraMode);
			CameraMode->OnActivation();
			CameraMode->CurrentBlendMode=EBlendType::BlendIn;
		}
	}
}

void UActionCameraStack::DeactivateStack()
{
	if(bisActive)
	{
		bisActive=false;
		for (UActionCameraMode * CameraMode:CameraModeStack)
		{
			check(CameraMode);
			CameraMode->OnDeactivation();
		}
		CameraModeStack.Empty();
	}
}
void UActionCameraStack::DrawDebug(UCanvas* Canvas)
{
	for (UActionCameraMode * CameraMode : CameraModeStack)
	{
		check(CameraMode)
		CameraMode->DrawDebug(Canvas);
	}
}

void UActionCameraStack::GetBlendWeightByTag(FGameplayTag ModeTag, float& BlendWeight)
{
	if(CameraModeStack.Num()<=0)
	{
		BlendWeight=0;
		UE_LOG(LogTemp,Warning,TEXT("Can't Find Correct Camera"))
		return;
	}
	for (UActionCameraMode * CameraMode: CameraModeStack)
	{
		if(CameraMode->GetCameraTag()==ModeTag)
		{
			BlendWeight=CameraMode->GetBlendWeight();
			return;
		}
	}
	BlendWeight=0;
	UE_LOG(LogTemp,Warning,TEXT("Can't Find Correct Camera"))
}

void UActionCameraStack::GetTopBlendWeight(FGameplayTag& TopTag, float& BlendWeight)
{
	if(CameraModeStack.Num()<=0)
	{
		BlendWeight=0;
		TopTag=FGameplayTag();
		UE_LOG(LogTemp,Warning,TEXT("Can't Find Correct Camera"))
		return;
	}
	else
	{
		//注意这里的Top指的是最末尾的，因为最后一个是必定是1；
		UActionCameraMode * TopCameraMode =CameraModeStack.Last();
		check(TopCameraMode);
		TopTag=TopCameraMode->GetCameraTag();
		BlendWeight=TopCameraMode->GetBlendWeight();
	}
}

void UActionCameraStack::PushCameraMode(TSubclassOf<UActionCameraMode> CameraModeClass)
{
	if(!CameraModeClass)
	{
		return;
	}
	UActionCameraMode * CameraMode=GetCameraModeInstance(CameraModeClass);
	if(CameraMode)
	{
		int32 StackSize=CameraModeStack.Num();
		if((StackSize>0)&&(CameraModeStack[0]->CameraID==CameraMode->CameraID))
		{
			return;
		}
		//查看是否已经在这个堆栈里面的了那么就移除他,然后重新设置它的Alpha;
		int32 ExistingStackIndex=-1;
		float ExistingStackContribution=1.0f;
		for(int32 i=0;i<StackSize;i++)
		{
			check(CameraModeStack[i])
			if(CameraModeStack[i]->CameraID==CameraMode->CameraID)
			{
				ExistingStackIndex=i;
				ExistingStackContribution*=CameraModeStack[i]->GetBlendWeight();
				break;
			}
		}
		//检测是否存在这个CameraMode在这个栈当中，如果存在那么就剔除因为还要再加进来
		if(ExistingStackIndex!=-1)
		{
			CameraModeStack.RemoveAt(ExistingStackIndex);
			StackSize--;
		}
		//如果不存在那么就将其权重设置为0重新开始。
		else
		{
			ExistingStackContribution=0.0f;
		}
		//如果堆栈里面没有相机或者相机模式中没有混合时间时会直接混合为1
		bool bShouldBlend=(CameraMode->BlendCurveInfo.BlendTime>0 && StackSize>0) ;
		const float BlendWeight=bShouldBlend?ExistingStackContribution:1.0f;
		CameraMode->SetBlendWeight(EBlendType::BlendIn,BlendWeight);
		//直接将相机嵌入到最前面
		CameraModeStack.Insert(CameraMode,0);
		CameraModeStack.Last()->SetBlendWeight(EBlendType::BlendIn,1.0f);
		if(ExistingStackIndex==-1)
		{
			CameraMode->OnActivation();
		}
	}
}

bool UActionCameraStack::EvaluateStack(float DeltaTime,FActionCameraNormalViewInfo  & StackViewInfo)
{
	if(!IsActive())
	{
		return false;
	}
	UpdateStack(DeltaTime);
	BlendStack(DeltaTime,StackViewInfo);
	UpdatePivot(DeltaTime);
	return true;
}
void UActionCameraStack::UpdateStack(float DeltaTime)
{
	const int32 IndexSize=CameraModeStack.Num();
	if(IndexSize<=0)
	{
		return;
	}
	int32 RemoveCount=0;
	int32 RemoveIndex=INDEX_NONE;
	for (int32 StackIndex=0;StackIndex<IndexSize;StackIndex++)
	{
			UActionCameraMode *  CurrentCameraMode=CameraModeStack[StackIndex];
			check(CurrentCameraMode)
			CurrentCameraMode->UpdateCameraMode(DeltaTime);
			if(CurrentCameraMode->GetBlendWeight()>=1.0f)
			{
				RemoveIndex=StackIndex+1;
				RemoveCount=IndexSize-RemoveIndex;
				break;
			}
	}
	if(RemoveCount>0)
	{
		for (int32 StackIndex=RemoveIndex;StackIndex<IndexSize;StackIndex++)
		{
			UActionCameraMode * CameraMode=CameraModeStack[StackIndex];
			check(CameraMode)
			if(CameraMode)
			{
				CameraMode->OnDeactivation();
			}
		}
		CameraModeStack.RemoveAt(RemoveIndex,RemoveCount);
	}
}

void UActionCameraStack::BlendStack(float DeltaTime,FActionCameraNormalViewInfo & StackViewInfo)
{
	const int32 StackSize=CameraModeStack.Num();
	if(StackSize<=0)
	{
		return;
	}
	const  UActionCameraMode *	CameraMode=CameraModeStack[StackSize-1];
	check(CameraMode);
	StackViewInfo=CameraMode->GetActionCameraViewInfo();
	for (int StackIndex=StackSize-2;StackIndex>=0;StackIndex--)
	{
		CameraMode=CameraModeStack[StackIndex];
		check(CameraMode)
		StackViewInfo.Blend(CameraMode->GetActionCameraViewInfo(),CameraMode->GetBlendWeight());
	}
}

void UActionCameraStack::UpdatePivot(float DeltaTime)
{
	const int32 StackSize=CameraModeStack.Num();
	if(StackSize<=0)
	{
		return;
	}
	const  UActionCameraMode *	CameraMode=CameraModeStack[StackSize-1];
	check(CameraMode);
	PivotLocation=CameraMode->GetPivotLocation();
	for (int StackIndex=StackSize-2;StackIndex>=0;StackIndex--)
	{
		CameraMode=CameraModeStack[StackIndex];
		check(CameraMode)
		CameraGlobalFunc::BlendPivot(PivotLocation,CameraMode->GetPivotAfterOffsetLocation(),CameraMode->GetBlendWeight());
	}
}

UActionCameraMode* UActionCameraStack::GetCameraModeInstance(TSubclassOf<UActionCameraMode> CameraModeClass)
{
	if(!CameraModeClass||lockindex!=0)
	{
		return nullptr;
	}
	for (UActionCameraMode * CameraMode:CameraModeInstance)
	{
		if(CameraMode->GetClass()==CameraModeClass)
		{
			return CameraMode;
		}
	}
	UActionCameraMode * NewCameraMode=NewObject<UActionCameraMode>(this,CameraModeClass);
	check(NewCameraMode)
	NewCameraMode->ViewInfoFactory= Cast<AActionPlayerCameraManager>(GetOuter())->ViewInfoFactory;
	if(NewCameraMode->DoOnce) return NewCameraMode;
	CameraModeInstance.Add(NewCameraMode);
	return NewCameraMode;
}
