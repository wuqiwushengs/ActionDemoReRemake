// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionSystem/CollisionType.h"
#include "CollisionSystem/CollisionCompoent/AttackCollisionComponent.h"
#include "UObject/Interface.h"
#include "CollisionSystemInterface.generated.h"

enum class ECollisionType : uint8;
// This class does not need to be modified.
UINTERFACE()
class UCollisionSystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */


class ACTIONSKILLASSETRUNTIME_API ICollisionSystemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAttackCollisionComponent * GetAttackCollisionComponent();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void OnBeAttackRecallInternal(FAttackedResult AttackedResult);
	
};
