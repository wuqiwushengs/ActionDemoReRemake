// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraViewInfoFactory.generated.h"

UCLASS()
class ACTIONCAMERASYSTEMASSET_API ACameraViewInfoFactory : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACameraViewInfoFactory();

protected:
	// Called when the game starts or when spawned
	UFUNCTION(BlueprintCallable)
	AActor* GetActorFromSceneByClass(TSubclassOf<AActor> ActorClass);
	UFUNCTION(BlueprintCallable)
	void GetAllActorFromClass(TSubclassOf<AActor>ActorClass,TArray<AActor*> & Actors);
	UFUNCTION(BlueprintCallable)
	void GetAllActorsByTag(FName Tag,TArray<AActor*> & Actors);
	UFUNCTION(BlueprintCallable)
	void GetAllActorsFromClassAndTag(FName Tag,TSubclassOf<AActor>ActorClass,TArray<AActor*>&Actors);
public:
	// Called every frame
};
