// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraComponentInterface.h"
#include "Camera/CameraComponent.h"
#include "CameraDataFold/ActionCameraTypes.h"
#include "ActionCameraComponent.generated.h"

/**
 * 
 */


UCLASS(meta=(BlueprintSpawnableComponent))
class ACTIONCAMERASYSTEMASSET_API UActionCameraComponent : public UCameraComponent,public ICameraComponentInterface
{
	GENERATED_BODY()
public:
	UActionCameraComponent();
	UPROPERTY(EditDefaultsOnly)
	ECameraForm CameraForm;
	virtual ECameraForm GetCameraForm_Implementation() override;
	UFUNCTION(BlueprintPure,BlueprintCallable)
	static UActionCameraComponent * FindActionCameraComponent(const AActor * Actor ) { return Actor?Actor->FindComponentByClass<UActionCameraComponent>():nullptr;}
#if WITH_EDITOR
	virtual void DrawDebug(UCanvas *Canvas) override;
#endif
};
