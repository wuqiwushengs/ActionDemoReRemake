#include "PostBlendStack.h"
#include "PostBlendbaseMode.h"
#include "CameraComponentFold/CameraMontage/CameraMontagePlayer.h"
#include "Engine/Canvas.h"

void UPostBlendStack::DisplayDebug(UCanvas* Canvas)
{
	
	for (auto Var:BlendbaseModesStack)
	{
		Var->DisplayDebug(Canvas);
	}
	

}

void UPostBlendStack::StartPostProcessingChange(FPostProcessSettings InPostProcessSettings)
{
	FWeightedBlendables Blendables=FinalPostProcessSettings.WeightedBlendables;
	FinalPostProcessSettings=InPostProcessSettings;
	FinalPostProcessSettings.WeightedBlendables=Blendables;
}

void UPostBlendStack::AddBlendMode(TSubclassOf<UPostBlendbaseMode>BlendMode)
{
	if(!BlendMode) return;
	for (auto  PostInstance:BlendbaseModesInstance)
	{
		if(PostInstance->GetClass()==BlendMode)
		{
			for (UPostBlendbaseMode* ModesStack : BlendbaseModesStack)
			{
				if(ModesStack->GetID()==PostInstance->GetID())
				{
					ModesStack->SetBlendType(ECameraMontageBlendType::WaitAdd);
					ModesStack->DeActive();
				}
			}
			BlendbaseModesStack.RemoveAll([PostInstance](UPostBlendbaseMode * BlendMode)
			{
				return BlendMode->GetID()==PostInstance->GetID();
			});
			BlendbaseModesStack.Add(PostInstance);
			PostInstance->SetBlendType(ECameraMontageBlendType::BlendIn);
			PostInstance->OnActive();
			return;
		}
	}
	check(BlendMode);
	UPostBlendbaseMode * Mode=NewObject<UPostBlendbaseMode>(this,BlendMode);
	BlendbaseModesInstance.Add(Mode);
	//增加实例然后更改。
	BlendbaseModesStack.Add(Mode);
	Mode->SetBlendType(ECameraMontageBlendType::BlendIn);
	Mode->OnActive();
	
 }
void UPostBlendStack::UpdateBlendInfo(float DeltaTime)
{
	//更新堆栈数据
	UpdateBlendStack(DeltaTime);
	//更新每个Mode的混合
	UpdateBlendMode(DeltaTime);
	//更新后期处理object
	UpdatePostProcessObject(DeltaTime);
	//更新后期处理数据
}

void UPostBlendStack::UpdatePostProcessObject(float DeltaTime)
{
	TArray<FWeightedBlendable> Blendables;
	for (auto BlendMode:BlendbaseModesStack)
	{
		Blendables.Append(BlendMode->GetFinalWeightedBlendable());
	}
	for (auto WeightBlendable:Blendables)
	{
		FinalPostProcessSettings.AddBlendable(WeightBlendable.Object,WeightBlendable.Weight);
	}
}

void UPostBlendStack::UpdateBlendStack(float DeltaTime)
{	if(BlendbaseModesStack.Num()<=0) return;
	TSet<int> RemoveIndex;
	for (int i=0;i<BlendbaseModesStack.Num();i++)
	{
		if(BlendbaseModesStack[i]->GetPostProcessBlendType()==ECameraMontageBlendType::WaitAdd)
		{
			RemoveIndex.Add(i);
		}
	}
	for (int index:RemoveIndex)
	{
		BlendbaseModesStack[index]->DeActive();
		for (auto PostObject:BlendbaseModesStack[index]->GetFinalWeightedBlendable())
		{
			FinalPostProcessSettings.RemoveBlendable(PostObject.Object);
		}
		BlendbaseModesStack.RemoveAt(index);
	}
}

void UPostBlendStack::UpdateBlendMode(float DeltaTime)
{

	for(auto BlendMode:BlendbaseModesStack)
	{
		BlendMode->UpdatePostBlendable(DeltaTime);
	}
}
