// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CollisionInfoTransation.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCollisionInfoTransation : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONSKILLASSETRUNTIME_API ICollisionInfoTransation
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void TransInfoToDetector(const FHitResult & HitResult);
	virtual 	void TransInfoToDetector_Implementation(const FHitResult & HitResult )=0;
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void TransInfoToFollower(const FHitResult & BeHitResult);
	virtual 	void TransInfoToFollower_Implementation(const FHitResult & BeHitResult)=0;

};
