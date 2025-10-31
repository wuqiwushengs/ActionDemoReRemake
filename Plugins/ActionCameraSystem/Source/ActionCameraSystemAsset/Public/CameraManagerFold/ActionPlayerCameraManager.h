// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "CameraModeFold/ActionCameraStack.h"
#include "ActionPlayerCameraManager.generated.h"

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
DECLARE_DELEGATE_RetVal(TSubclassOf<UActionCameraMode>,FCameraModeDelegate)
UCLASS()
class ACTIONCAMERASYSTEMASSET_API AActionPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	AActionPlayerCameraManager();
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraMontagePlayer> MontagePlayerClass;
	UPROPERTY(Transient)
	UCameraMontagePlayer * CameraMontagePlayer;
	UPROPERTY(Transient)
	UActionCameraStack * CameraStack;
	UFUNCTION(BlueprintCallable,BlueprintPure)
	UCameraMontagePlayer * GetCameraMontagePlayer() {return CameraMontagePlayer;}
	/*你可以预先在玩家组件中添加摄像机,摄像机管理器中会自动添加相机内容，也可以直接赋值*/
	TMap<ECameraForm, UCameraComponent *> OwnedCamera;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	ECameraForm DefaultCameraForm;
	//通过缓存的方式降低获取时的消耗 需要和SetComponent配合使用
	UPROPERTY(BlueprintReadOnly,Transient)
	UCameraComponent * CurrentActiveCameraCache;
	UPROPERTY(BlueprintReadOnly,Transient)
	ECameraForm  CurrentActiveCameraTypeCache;
	UFUNCTION(BlueprintCallable,BlueprintPure)
	UCameraComponent * GetCurrentActiveCameraComponent(ECameraForm & CameraForm );
	TArray<UCameraComponent *> GetUnActivateCameraComponent();
	//必须用这个来设置相机组件，默认设置相机组件会自动设置不需要通过此管理
	UFUNCTION(BlueprintCallable)
	void SetCurrentActiveCameraComponent(ECameraForm ActiveCameraEnum);
	void SetAllCameraLocationAndRotation();
	//更新的最后使用。 这个offset是相对于摄像机方向。
	void AddCameraOffset(FVector NewOffset,float FovOffset) ;
	FVector CameraViewLocationOffset;
	float CameraViewFovOffset;
	//需要的内容需要绑定到这个位置
	FCameraModeDelegate CameraModeBindSingleDelegate;
private:
	virtual void InitializeFor(class APlayerController* PC) override;
	virtual void BeginPlay() override;
	//在CameraStack中更新位置数据是进行Interp处理。
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
	void UpdateCameraModes();
	void UpdateActionCameraValue(FMinimalViewInfo  &OutPOV,float DeltaTime);
	void UpdateCameraLag(FActionCameraNormalViewInfo &ActionCameraNormalViewInfo,float DeltaTime);
public:
	UPROPERTY(EditDefaultsOnly)
	bool bDoRotationLag;
	UPROPERTY(EditDefaultsOnly)
	bool bDoLocationLag;
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="bDoLocationLag"))
	float CameraLagSpeed=5.0;
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition="bDoRotationLag"))
	float CameraRotationLagSpeed=5.0f;
	UPROPERTY(EditDefaultsOnly)
	float PivotLagSpeed=5.0f;
	//迭代时间
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	bool bUseCameraTimeStep;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(EditCondition="bUseCameraTimeStep",ClampMin="0.005",ClampMax="0.5",UIMin="0.005",UIMax="0.5"))
	float CameraLagMaxTimeStep;
	FActionCameraNormalViewInfo GetLastUpdateCameraNormalViewInfo() { return CameraNormalViewInfoCache;}
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
};
