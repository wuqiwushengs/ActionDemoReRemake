#pragma once
#include "BlendData.generated.h"

UENUM(BlueprintType)
enum class  EActionCameraBlendFunction:uint8
{
	Linear,
	Cubic,
	EaseIn,
	EaseOut,
	EaseInOut,
	CustomCurve,
	PreBlend,
	MaxCount,
};
namespace CameraData
{
	
	namespace  BlendInfo
	{
		/*该函数用了两个浮点一个是混合混合强度通常用时间的比率来算，另外一个是混合强度仅仅用在了EaseIn,EaseOut以及EaseInOut当中,如果用了自定义曲线并且没有曲线的话就会用线性模式*/
		static float GetBlendWeightFromBlendFunction(EActionCameraBlendFunction BlendFunction,UCurveFloat * BlendCurve ,float BlendAlpha,float BlendExp=1)
		{
			switch (BlendFunction)
			{
			case	EActionCameraBlendFunction::Linear :
				return FMath::Lerp(0.f,1.f,BlendAlpha);
			case EActionCameraBlendFunction::Cubic :
				return FMath::CubicInterp(0.f,0.f,1.f,0.f,BlendAlpha);
			case EActionCameraBlendFunction::EaseIn :
				return FMath::Lerp(0.f,1.f,FMath::Pow(BlendAlpha,BlendExp)); 
			case EActionCameraBlendFunction::EaseOut:
				return  FMath::Lerp(0.f,1.f,FMath::Pow(1-BlendAlpha,BlendExp)); 
			case EActionCameraBlendFunction::EaseInOut:
				return FMath::InterpEaseInOut(0.f,1.f,BlendAlpha,BlendExp);
			case EActionCameraBlendFunction::PreBlend:
				return 1.0f;
			case EActionCameraBlendFunction::CustomCurve:
				return BlendCurve?BlendCurve->GetFloatValue(BlendAlpha):BlendAlpha;
			default:
				return  BlendAlpha;
				break;
			}
		}
	}
}
USTRUCT(BlueprintType)
struct FBlendCurveInfo
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	float BlendTime;
	UPROPERTY(EditDefaultsOnly)
	bool bUseRuntimeCurve=false;
	/*只支持0---1*/
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="bUseRuntimeCurve &&bUseCustomCurve"))
	FRuntimeFloatCurve   CustomBlendCurve;
	UPROPERTY(EditDefaultsOnly)
	bool bUseCustomCurve=false;
	/*只支持0---1*/
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="!bUseRuntimeCurve&& bUseCustomCurve"))
	 UCurveFloat * BlendCurve;
	UPROPERTY(EditDefaultsOnly)
	EActionCameraBlendFunction BlendFunction;
	/*混合强度仅仅用在了EaseIn,EaseOut以及EaseInOut当中*/
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="!UseCustomCurve"))
	float BlendExp=1.0f;
};

UENUM(BlueprintType)
enum class EBlendType:uint8
{
	WaitAdd,
	BlendIn,
	Loop,
};
USTRUCT(BlueprintType)
struct FBlendData
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	FBlendCurveInfo BlendInCurveInfo;
	UPROPERTY(EditDefaultsOnly)
	FBlendCurveInfo BlendOutCurveInfo;
};
	
