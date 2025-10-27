// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraComponentInterface.h"
#include "CineCameraComponent.h"
#include "CameraDataFold/ActionCameraTypes.h"
#include "ActionCineCameraComponent.generated.h"

/**
 * 
 */


UCLASS(meta=(BlueprintSpawnableComponent))
class ACTIONCAMERASYSTEMASSET_API UActionCineCameraComponent : public UCineCameraComponent,public  ICameraComponentInterface
{
	GENERATED_BODY()
public:
	UActionCineCameraComponent();
	UPROPERTY(EditDefaultsOnly)
	ECameraForm CameraForm;
	virtual ECameraForm GetCameraForm_Implementation() override;
	UFUNCTION(BlueprintPure,BlueprintCallable)
	static UActionCineCameraComponent * FindActionCameraComponent(const AActor * Actor ) { return Actor?Actor->FindComponentByClass<UActionCineCameraComponent>():nullptr;}
	UPROPERTY(EditDefaultsOnly)
	FActionCameraNormalViewInfo CameraOffset;
#if WITH_EDITOR
	virtual void DrawDebug(UCanvas *Canvas) override;
#endif
};
