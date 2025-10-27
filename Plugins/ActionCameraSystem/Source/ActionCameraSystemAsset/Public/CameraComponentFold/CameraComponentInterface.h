// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CameraComponentInterface.generated.h"

enum class ECameraForm : uint8;
struct FActionCameraNormalViewInfo;
// This class does not need to be modified.
UINTERFACE()
class UCameraComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONCAMERASYSTEMASSET_API ICameraComponentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void DrawDebug(UCanvas *Canvas)=0;
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	ECameraForm GetCameraForm();
};
