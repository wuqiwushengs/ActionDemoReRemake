#pragma once
#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "UActionUiCameraComponent.generated.h"

struct FActionCameraNormalViewInfo;

UCLASS()
class ACTIONCAMERASYSTEMASSET_API UActionUiCameraComponent:public  UCameraComponent
{
public:
	GENERATED_BODY()
	UActionUiCameraComponent();
	virtual void InitializeComponent() override;
	AActor * GetViewTarget() const { return Player;}
	void SetViewTarget(AActor* InPlayer, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams());
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void OnSetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams());
	void OnSetViewTarget_Implementation(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams());
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void OnLeaveViewTarget();
	void OnLeaveViewTarget_Implementation();
	virtual void UpdateViewTarget(FTViewTarget & OutVT,float DeltaTime);
	virtual void UpdateViewTargetInternal(FTViewTarget & OutVT,float DeltaTime);
	UFUNCTION(BlueprintImplementableEvent)
	bool  BlueprintUpdateUiCamera(FActionCameraNormalViewInfo & CameraNormalViewInfo ,float DeltaTime) ;
private:
	UPROPERTY(Transient)
	TObjectPtr<AActor> Player;
};
