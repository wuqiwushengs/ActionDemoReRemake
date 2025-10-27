#include "UActionUiCameraComponent.h"
#include "CameraManagerFold/ActionPlayerCameraManager.h"

UActionUiCameraComponent::UActionUiCameraComponent()
{
	bWantsInitializeComponent = true;
}

void UActionUiCameraComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UActionUiCameraComponent::SetViewTarget(AActor* InPlayer, FViewTargetTransitionParams TransitionParams)
{
	Player=InPlayer;
	OnSetViewTarget(InPlayer,TransitionParams);
}

void UActionUiCameraComponent::OnSetViewTarget_Implementation(AActor* InViewTarget,
                                                              FViewTargetTransitionParams TransitionParams)
{
}

void UActionUiCameraComponent::OnLeaveViewTarget_Implementation()
{
}

void UActionUiCameraComponent::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	FActionCameraNormalViewInfo CurrentValue;
	CurrentValue.CameraLocation=OutVT.POV.Location;
	CurrentValue.CameraRotation=OutVT.POV.Rotation;
	CurrentValue.FOV=OutVT.POV.FOV;
	/*设置为控制方向和*/
	if(BlueprintUpdateUiCamera(CurrentValue,DeltaTime))
	{
		OutVT.POV.Location=CurrentValue.CameraLocation;
		OutVT.POV.Rotation=CurrentValue.CameraRotation;
		OutVT.POV.FOV=CurrentValue.FOV;
		return;
	}
	UpdateViewTargetInternal(OutVT,DeltaTime);
}

void UActionUiCameraComponent::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
}
