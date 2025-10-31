#pragma once
#include "BlendData.h"
#include "CameraAnimationSequencePlayer.h"
#include "GameplayTagContainer.h"
#include "ActionCameraTypes.generated.h"
/*这两个宏可以直接用 但是用完记得将宏解开 UBT不支持通过宏编写UClass类型*/
#define CREATE_DATAASSET_AUTO(ClassName,ContentStructName,PropertyName)\
	UCLASS()\
	class  ACTIONCAMERASYSTEMASSET_API U##ClassName:public UPrimaryDataAsset\
	{\
			GENERATED_BODY()\
		public:\
			ContentStructName PropertyName;\
	};
	
#define CREAT_ARRAY_DATAASSET_AUTO(ClassName,ContentStructName,PropertyName)\
UCLASS()\
class  ACTIONCAMERASYSTEMASSET_API U##ClassName:public UPrimaryDataAsset\
{\
		GENERATED_BODY()\
public:\
	TArray<ContentStructName> PropertyName;\
};

class AActionPlayerCameraManager;
class UPostBlendBase;
class UCameraAnimationSequence;
class UCurveVector;
namespace  CameraGlobalFunc
{
	 AActionPlayerCameraManager * TryGetPlayerCameraManager(const AActor *  Owner);
	static  void BlendPivot(FVector & CurrentPivot,FVector OtherPivot,float PivotWeight)
	{
		if(PivotWeight<=0.0f)
		{
			return;	
		}
		if(PivotWeight>=1.0f)
		{
			CurrentPivot=OtherPivot;
			return;
		}
		CurrentPivot=FMath::Lerp(CurrentPivot,OtherPivot,PivotWeight);
	};
}
USTRUCT(BlueprintType)
struct FActionCameraNormalViewInfo
{
	GENERATED_BODY()
	FActionCameraNormalViewInfo()=default;
	FActionCameraNormalViewInfo(const FActionCameraNormalViewInfo &ViewInfo)
	{
		CameraLocation=ViewInfo.CameraLocation;
		CameraRotation=ViewInfo.CameraRotation;
		FOV=ViewInfo.FOV;
	};
	FActionCameraNormalViewInfo(const FVector & Location, const FRotator & Rotation,const float & Fov)
	{
		CameraLocation=Location;
		CameraRotation=Rotation;
		FOV=Fov;
	};
	virtual ~FActionCameraNormalViewInfo()=default;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FVector CameraLocation=FVector(0,0,0);
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FRotator CameraRotation=FRotator(0,0,0);
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float FOV=0.0f;

	virtual FActionCameraNormalViewInfo operator+(const FActionCameraNormalViewInfo & Other) const 
	{
		FActionCameraNormalViewInfo Result;
		Result.CameraLocation=this->CameraLocation+Other.CameraLocation;
		Result.CameraRotation=this->CameraRotation+Other.CameraRotation;
		Result.FOV=this->FOV+Other.FOV;
		return Result;
	}
	void Blend(const FActionCameraNormalViewInfo  & OtherCameraViewInfo,float BlendWeight)
	{
		if(BlendWeight<=0.0f)
		{
			return;
		}
		if(BlendWeight>=1.0f)
		{
			*this=OtherCameraViewInfo;
			return;
		}
		CameraLocation=FMath::Lerp(CameraLocation,OtherCameraViewInfo.CameraLocation,BlendWeight);
		//通过四元数插值来使插值更加平滑
		// 对 CameraRotation 使用四元数插值
		FQuat CurrentCameraQuat = CameraRotation.Quaternion();
		FQuat TargetCameraQuat = OtherCameraViewInfo.CameraRotation.Quaternion();
		FQuat InterpolatedCameraQuat = FQuat::Slerp(CurrentCameraQuat, TargetCameraQuat, BlendWeight);
		CameraRotation = InterpolatedCameraQuat.Rotator();

		FOV=FMath::Lerp(FOV,OtherCameraViewInfo.FOV,BlendWeight);
			
	};
};
UENUM(BlueprintType)
enum class ECameraForm:uint8
{
	NormalCamera,
	CinemaCamera,
	OtherCamera,
	NullCamera,
};
UENUM(BlueprintType)
enum class	ECameraPlacedMode:uint8
{
	FreeMovement,
	Settled,
};
USTRUCT(BlueprintType)
struct FCameraOffsetMapCurve
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveVector> LocationInfo;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveFloat> FovInfo;
};

USTRUCT(BlueprintType)
struct FCameraSpringArmInfo
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	float  SpringArmLens;
	UPROPERTY(EditDefaultsOnly)
	FVector TargetOffset;
	UPROPERTY(EditDefaultsOnly)
	FVector CameraOffset;
	
	
};
#pragma  region CameraMontage
UENUM(BlueprintType)
enum class ECameraMontageBlendType:uint8
{
	WaitAdd,
	BlendIn,
	Loop,
	BlendOut,
};
UENUM(BlueprintType)
enum class  ECameraMontagePlayType:uint8
{
	Additive,
	Modify,
};
UENUM(BlueprintType)
enum class  ECameraMontageType:uint8
{
	FixTransform,
	CurveMontage,
	AnimSequenceMontage,
};

UENUM(BlueprintType)
enum class EAdditiveType:uint8
{
	WorldLocation,
	CameraLocation,
	TargetActorLocation,
	CameraToTargetLocation,
};
USTRUCT(BlueprintType)
struct FCameraMontageFixedModifyInfo
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	FActionCameraNormalViewInfo  FixedViewInfo;
};
USTRUCT(BlueprintType)
struct FCameraMontageCurve
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveVector> LocationInfo;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveVector> RotationInfo;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveFloat> FovInfo;
};
USTRUCT(BlueprintType)
struct FCameraMontageSequenceInfo
{
	GENERATED_BODY()
	//制作相机序列的时候需要注意默认的相机原点位置必须为0 也就是世界原点，不然计算会出错。
	//相机序列的制作数据可以直接从动画序列中复制粘贴因此不一定非要用相机序列来制作。
	UPROPERTY(EditDefaultsOnly)
	UCameraAnimationSequence * CameraSequence;
private:
	//这两个用作缓存
	UPROPERTY(Transient)
	UCameraAnimationSequenceCameraStandIn *CameraStandInInstance=nullptr;
	UPROPERTY(Transient)
	UCameraAnimationSequencePlayer * SequencePlayerInstance=nullptr;
public:
	UCameraAnimationSequenceCameraStandIn * GetCameraStandInInstance(UObject * Owner)
	{
			if(!CameraStandInInstance)
			{
				check(Owner)
				CameraStandInInstance=NewObject<UCameraAnimationSequenceCameraStandIn>(Owner);
			}
		return CameraStandInInstance;
	}
	UCameraAnimationSequencePlayer * GetCameraAnimationSequencePlayerInstance(UObject * Owner)
	{
		if(!SequencePlayerInstance)
		{
			check(Owner)
			SequencePlayerInstance=NewObject<UCameraAnimationSequencePlayer>(Owner);
		}
		return SequencePlayerInstance;
	}
	void  InitializeCameraSequence(UObject * Owner);
	void GetCameraDataAtTime(float TimeInSeconds,FMinimalViewInfo & OutCameraData);
};
USTRUCT(BlueprintType)
struct FCameraMontageInfo
{
	GENERATED_BODY()
	/*三种不同的Camera类型固定信息，曲线数据以及相机序列*/
	/*基于相机位置*/
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="MontageType==ECameraMontageType::FixTransform",EditConditionHides))
	FCameraMontageFixedModifyInfo FixedRelativeTransformInfo;
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="MontageType==ECameraMontageType::AnimSequenceMontage",EditConditionHides))
	FCameraMontageSequenceInfo CameraSequence;
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="MontageType==ECameraMontageType::CurveMontage",EditConditionHides))
	FCameraMontageCurve MontageCurve;
	UPROPERTY(EditDefaultsOnly)
	ECameraMontageType MontageType;
	UPROPERTY(EditDefaultsOnly)
	FBlendData BlendInfo;
	UPROPERTY(EditDefaultsOnly)
	float DurationTime=0.0f;
	UPROPERTY(EditDefaultsOnly)
	bool bLoop=false;
};
/*允许有多个MontagePlay*/
UENUM(BlueprintType)
enum  class EMontageSocketType:uint8
{
	CharacterBoneSocket,
	CharacterSocket,
};

USTRUCT(BlueprintType)
struct FCameraAnimMontagePlay
{
	GENERATED_BODY()
	
	//如果相机模式是Modify, 这个Modify是相对于角色或者插槽的位置来说的，但是其旋转为了正确设定根据其角色的旋转来作为modify的基础。
	UPROPERTY(EditDefaultsOnly)
	ECameraMontagePlayType CameraMontageType;
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="CameraMontageType==ECameraMontagePlayType::Additive",EditConditionHides))
	EAdditiveType AdditiveType;
	UPROPERTY(EditDefaultsOnly)
	FCameraMontageInfo MontageInfo;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FName SocketName;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	EMontageSocketType SocketType;
};
USTRUCT(BlueprintType)
struct FCameraAnimMontagePlayPair:public  FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	FCameraAnimMontagePlay MontageInfo;
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag MontageTag; 
};
UCLASS()
class ACTIONCAMERASYSTEMASSET_API UCameraMontageDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,meta=(TitleProperty="{MontageTag}"))
	TArray<FCameraAnimMontagePlayPair> CameraMontagePlayPairs;
};
//摄像机蒙太奇
#pragma  endregion
//摄像机渐变
#pragma region CameraFade
USTRUCT(BlueprintType)
struct FCameraFadeInfo:public  FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	float StartAlpha=0.0f;
	UPROPERTY(EditAnywhere)
	float ToAlpha=0.0f;
	UPROPERTY(EditAnywhere)
	float Duration=0.0f;
	UPROPERTY(EditAnywhere)
	FLinearColor FadeColor=FLinearColor::Black;
	UPROPERTY(EditAnywhere)
	bool ShouldFadeAudio=false;
	UPROPERTY(EditAnywhere)
	bool HoldWhenFinish;
	UPROPERTY(EditAnywhere)
	FGameplayTag CameraFadeTag;
};
UCLASS()
class UCameraFadeDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FCameraFadeInfo> CameraFadeInfo;
};
#pragma endregion
#pragma region SceneColorChnage
//摄像机的场景颜色
USTRUCT(BlueprintType)
struct FCameraSceneColorChange:public  FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	FVector DesiredColorScale{1,1,1};
	UPROPERTY(EditAnywhere)
	float BlendInTime=0.0f;
	UPROPERTY(EditAnywhere)
	float BlendOutTime=0.0;
	UPROPERTY(EditAnywhere)
	float DurationTime=0.0f;
	UPROPERTY(EditAnywhere)
	FGameplayTag ColorChangeTag;
};
UCLASS()
class ACTIONCAMERASYSTEMASSET_API UCameraSceneColorDataAsset:public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<FCameraSceneColorChange> SceneColorChangeInfo ;
};
#pragma endregion
#pragma  region CameraPostChange
USTRUCT()
struct FPostProcessMaterialWithTag
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PostProcessMaterialTag;
	/*一定要是后处理类型*/
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInstance> PostProcessMaterial;
};
USTRUCT(BlueprintType)
struct FCameraPostProcessMaterialChange:public  FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	FBlendData BlendData;
	UPROPERTY(EditDefaultsOnly,meta=(TitleProperty="{PostProcessMaterialTag}"))
	FPostProcessMaterialWithTag PostMaterialInfo;
	UPROPERTY(EditDefaultsOnly)
	bool bLoop=false;
};
UCLASS()
class ACTIONCAMERASYSTEMASSET_API UPostProcessMaterialDataAsset:public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FCameraPostProcessMaterialChange> ProcessMaterialData;
};
USTRUCT(BlueprintType)
struct FCameraPostProcessValue
{
	GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Overrides)
   FPostProcessSettings CameraPostProcessSetting;
};
USTRUCT(BlueprintType)
struct FPostProcessValueChange:public  FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FCameraPostProcessValue CameraPostProcessValue;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FBlendData BlendData;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	bool bLoop;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag PostProcessValueTag;
};
UCLASS()
class	ACTIONCAMERASYSTEMASSET_API UPostProcessValueChangeDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,meta=(TitleProperty="{PostProcessValueTag}"))
	TArray<FPostProcessValueChange>  PostProcessValue; 
};
/*为以后的扩展作准备*/
USTRUCT(BlueprintType)
struct FCameraPostProcessBlendableValue
{
	GENERATED_BODY()
	/*必须重载PostBlendBase类并且继承ChangeBlendableSettings */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UPostBlendBase>  PostProcessBlendableClass;
	
};
USTRUCT(BlueprintType)
struct FCameraPostProcessBlendableValueChange:public  FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FCameraPostProcessBlendableValue BlendableValue;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FBlendData BlendData;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	bool bLoop;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FGameplayTag PostProcessBlendableTag;
};

UCLASS()
class UPostProcessBlendableValueChange : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FCameraPostProcessBlendableValueChange> BlendableValue;
};
#pragma  endregion
#pragma region CineCamera
USTRUCT(BlueprintType)
struct FCineCameraInfo
{
	GENERATED_BODY()
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Current Camera Settings")
	FCameraFilmbackSettings Filmback;
	/** Controls the camera's lens. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Current Camera Settings")
	FCameraLensSettings LensSettings;
	/** Controls the camera's focus. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Current Camera Settings")
	FCameraFocusSettings FocusSettings;
	/** Controls the crop settings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Current Camera Settings")
	FPlateCropSettings CropSettings;
	/** Current focal length of the camera (i.e. controls FoV, zoom) */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Current Camera Settings")
	float CurrentFocalLength;
	/** Current aperture, in terms of f-stop (e.g. 2.8 for f/2.8) */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Current Camera Settings")
	float CurrentAperture;
	/** Read-only. Control this value via FocusSettings. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Current Camera Settings")
	float CurrentFocusDistance;
};

USTRUCT(BlueprintType)
struct FCineCameraValueChange
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FCineCameraInfo CineCameraInfo;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FBlendData BlendData;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float Duration=0.0f;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bLoop;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGameplayTag CineCameraTag;
};

UCLASS()
class ACTIONCAMERASYSTEMASSET_API UCineCameraValueDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(TitleProperty="{CineCameraTag}"))
	TArray<FCineCameraValueChange> CineCameraValues;
};
#pragma endregion
//TODO::CameraShake 和 Camera lensEffect这两个需要进行调整

