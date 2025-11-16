// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "CameraModeFold/ActionCameraStack.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ActionPlayerCameraManager.generated.h"

class ACameraViewInfoFactory;
class UPostBlendbaseMode;
class UPostBlendStack;
class UCameraMontagePlayer;
class UActionUiCameraComponent;
struct FActionCameraNormalViewInfo;
struct FGameplayTag;
class UCameraComponent;
enum class ECameraForm : uint8;
class UCameraSceneColorDataAsset;
struct FCameraSceneColorChange;
/**
 * 
 */
UCLASS()
class ACTIONCAMERASYSTEMASSET_API AActionPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	AActionPlayerCameraManager();
	/*CameraAvoidance*/
	UPROPERTY(EditDefaultsOnly,Category="Avoidance")
	TEnumAsByte<ETraceTypeQuery> AvoidanceTraceType;
	UPROPERTY(EditDefaultsOnly,Category="Avoidance")
	TEnumAsByte<EDrawDebugTrace::Type> AvoidanceDebugType;
	UPROPERTY(EditDefaultsOnly,Category="Avoidance")
	TArray<TSubclassOf<AActor>> IgnoreActor;
	UPROPERTY(EditDefaultsOnly,Category="Avoidance")
	float AvoidanceInterpSpeed=20.0;
	/*CameraMontage*/
	UPROPERTY(EditDefaultsOnly,Category="Montage")
	TSubclassOf<UCameraMontagePlayer> MontagePlayerClass;
	UPROPERTY(Transient)
	UCameraMontagePlayer * CameraMontagePlayer;
	/*CameraMode*/
	UPROPERTY(Transient)
	UActionCameraStack * CameraStack;
	UFUNCTION(BlueprintCallable,BlueprintPure)
	UCameraMontagePlayer * GetCameraMontagePlayer() {return CameraMontagePlayer;}
	//更新的最后使用。 这个offset是相对于摄像机方向。
	UFUNCTION(BlueprintCallable)
	void AddCameraOffset(FVector NewOffset,float FovOffset) ;
	UPROPERTY(EditDefaultsOnly,Category="Offset")
	FVector CameraViewLocationOffset;
	UPROPERTY(EditDefaultsOnly,Category="Montage")
	float CameraViewFovOffset;
	//需要的内容需要绑定到这个位置
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UActionCameraMode> DefaultCameraMode;
	UFUNCTION(BlueprintCallable)
	void PushCameraMode(TSubclassOf<UActionCameraMode> CameraMode);
	UPROPERTY(BlueprintReadOnly)
	ACameraViewInfoFactory* ViewInfoFactory; 
private:
	virtual void InitializeFor(class APlayerController* PC) override;
	virtual void BeginPlay() override;
	//在CameraStack中更新位置数据是进行Interp处理。
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
	void UpdateActionCameraValue(FMinimalViewInfo  &OutPOV,float DeltaTime);
	void UpdateCameraLag(FActionCameraNormalViewInfo &ActionCameraNormalViewInfo,float DeltaTime);
	FActionCameraNormalViewInfo AdditiveViewInfoCache;
	FActionCameraNormalViewInfo ModifyViewInfoCache;
	//避障处理
	void UpdateCameraAvoidance(FActionCameraNormalViewInfo &ActionCameraNormalViewInfo,FActionCameraNormalViewInfo Cache,float DeltaTime);
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="CameraLock")
	bool LockX=false;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="CameraLock")
	bool LockY=false;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="CameraLock")
	bool LockZ=false;
	UPROPERTY(EditDefaultsOnly,Category="CameraLag")
	bool bDoRotationLag;
	UPROPERTY(EditDefaultsOnly,Category="CameraLag")
	bool bDoLocationLag;
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="bDoLocationLag"),Category="CameraLag")
	float CameraLagSpeed=5.0;
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="bDoRotationLag"),Category="CameraLag")
	float CameraRotationLagSpeed=5.0f;
	UPROPERTY(EditDefaultsOnly,Category="CameraLag")
	float PivotLagSpeed=5.0f;
	//迭代时间
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="CameraLag")
	bool bUseCameraTimeStep;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(EditCondition="bUseCameraTimeStep",ClampMin="0.005",ClampMax="0.5",UIMin="0.005",UIMax="0.5"),Category="CameraLag")
	float CameraLagMaxTimeStep;
	const FActionCameraNormalViewInfo & GetLastUpdateCameraNormalViewInfo() { return CameraNormalViewInfoCache;}
#pragma region Debug
	virtual void DisplayDebug(class UCanvas* Canvas, const class FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
#pragma  endregion
#pragma region UICameraComponent
    UFUNCTION(BlueprintCallable,BlueprintPure)
	UActionUiCameraComponent * GetActionUICameraComponent();
	UFUNCTION(BlueprintCallable)
	void ActiveUICameraComponent(AActor * InUiCameraActor,FViewTargetTransitionParams TransitionParams);
	UFUNCTION(BlueprintCallable)
	void DeactiveUICameraComponent(FViewTargetTransitionParams TransitionParams);
	UPROPERTY(BlueprintReadOnly)
	UActionUiCameraComponent * UICameraComponent;
	FTimerHandle ViewHandle;
#pragma  endregion 
	UPROPERTY(BlueprintReadOnly)
	APawn * ControlledPlayer;
private:
	FActionCameraNormalViewInfo CameraMontageViewInfoCache;
	FActionCameraNormalViewInfo CameraNormalViewInfoCache;
#pragma region  SceneColorChange
public:
	UFUNCTION(BlueprintCallable)
	virtual void SetPrimarySceneColor(FVector Color);
	UFUNCTION(BlueprintCallable)
	virtual void SetSceneColorWithCustom(FVector WantedColorScale,float BlendInTime=0,float BlendOutTime=0.f,float DurationTime=0);

private:
	FVector PrimaryColorScale{1,1,1};
	FTimerDelegate ColorScaleDelegate;
	FTimerHandle ColorScaleHandle;
#pragma endregion
	bool bFirst=true;
	FVector LastPivotLocation;

#pragma  region PostProcessingBlend
public:
	/*这里更改后处理材质不会有任何的作用*/
	UFUNCTION(BlueprintCallable)
	void SetNewPostProcess(FPostProcessSettings PostSettings);
	UFUNCTION(BlueprintCallable)
	void AddPostProcess(TSubclassOf<UPostBlendbaseMode> BlendMode);
private:
	UPROPERTY()
	UPostBlendStack * PostBlendStack;
	UPROPERTY()
	FPostProcessSettings FinalCameraProcessSettingCache;

#pragma  endregion 
};
