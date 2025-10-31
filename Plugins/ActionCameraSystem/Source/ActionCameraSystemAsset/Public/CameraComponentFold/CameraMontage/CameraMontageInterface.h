#pragma once
#include "UObject/GCObject.h"
#include "UObject/GCObject.h"
#include "CameraDataFold/ActionCameraTypes.h"

class UCameraMontageSequence;
struct FActionCameraNormalViewInfo;

struct FPivotInfo
{
	//用来记录原点的位置和原点的旋转
	FVector OriginLocation;
	FRotator OriginRotator;
	//三个方向的朝向；
	FVector ForwardVector;
	FVector RightVector;
	FVector UpVector;
	FVector GetCorrectLocation(FVector BaseLocation,FVector AdditiveVector)
	{
		return BaseLocation+AdditiveVector.X*this->ForwardVector
	+AdditiveVector.Y*this->RightVector+AdditiveVector.Z*this->UpVector;
	}
	
};
namespace CameraMontageBlend
{
	  float GetCorrectBlendWeight( ECameraMontageBlendType MontageBlendType,FBlendData BlendInfo,float BlendAlpha);
	//注意这个必须是目标自身不然获取不到相机
	 FPivotInfo GetCorrectAdditivePivot(EAdditiveType AdditiveType,AActor * FollowTarget);
	FMinimalViewInfo GetAnimationSequenceTransformInfo( UCameraMontageSequence *  Sequence);
}
//仅仅用来计算相机的位置
class ICameraMontagePlayPattern
{
public:
	virtual  ~ICameraMontagePlayPattern()=default;
	virtual void  ProcessCameraMontagePlayAdditive(FActionCameraNormalViewInfo & AdditiveViewInfo,UCameraMontageSequence * Sequence,float BlendWeight)=0;
	virtual void  ProcessCameraMontagePlayModify(FActionCameraNormalViewInfo &  ModifyViewInfo,UCameraMontageSequence * Sequence)=0;
	
};
class FCameraMontageFixTransformFactory:public ICameraMontagePlayPattern
{
public:
	virtual void ProcessCameraMontagePlayAdditive(FActionCameraNormalViewInfo&AdditiveViewInfo,UCameraMontageSequence * Sequence,float BlendWeight) override;
	virtual void ProcessCameraMontagePlayModify(FActionCameraNormalViewInfo& ModifyViewInfo, UCameraMontageSequence * Sequence) override;
	
};
class FCameraMontageCurveMontageFactory:public  ICameraMontagePlayPattern
{
public:
	virtual  void ProcessCameraMontagePlayAdditive(FActionCameraNormalViewInfo& AdditiveViewInfo,UCameraMontageSequence * Sequence,float BlendWeight) override;
	virtual  void ProcessCameraMontagePlayModify(FActionCameraNormalViewInfo& ModifyViewInfo,UCameraMontageSequence * Sequence) override;
};
class FCameraMontageAnimSequenceMontageFactory:public  ICameraMontagePlayPattern
{
public:
	virtual void ProcessCameraMontagePlayAdditive(FActionCameraNormalViewInfo& AdditiveViewInfo, UCameraMontageSequence * Sequence,float BlendWeight) override;
	virtual void ProcessCameraMontagePlayModify(FActionCameraNormalViewInfo& ModifyViewInfo, UCameraMontageSequence * Sequence) override;
};
class FCameraMontageValueCalculateFactory:public FGCObject
{
public:
	FCameraMontageValueCalculateFactory()
	{
		CalculatePattern.Emplace(ECameraMontageType::FixTransform,MakeUnique<FCameraMontageFixTransformFactory>());
		CalculatePattern.Emplace(ECameraMontageType::CurveMontage,MakeUnique<FCameraMontageCurveMontageFactory>());
		CalculatePattern.Emplace(ECameraMontageType::AnimSequenceMontage,MakeUnique<FCameraMontageAnimSequenceMontageFactory>());
	}
	virtual void CalculateMontageValue(FActionCameraNormalViewInfo&ModifyValue,UCameraMontageSequence * Sequence,ECameraMontagePlayType MontagePlayType,float BlendWeight);
private:
	TMap<ECameraMontageType,TUniquePtr<ICameraMontagePlayPattern>> CalculatePattern;
	//为其提供一个垃圾回收能力
	virtual  void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;
	
};