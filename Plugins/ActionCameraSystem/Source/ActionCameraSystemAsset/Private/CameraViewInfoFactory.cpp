// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraViewInfoFactory.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
ACameraViewInfoFactory::ACameraViewInfoFactory()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

AActor* ACameraViewInfoFactory::GetActorFromSceneByClass(TSubclassOf<AActor> ActorClass)
{
	return UGameplayStatics::GetActorOfClass(this,ActorClass);
}

void ACameraViewInfoFactory::GetAllActorFromClass(TSubclassOf<AActor> ActorClass,TArray<AActor*> & Actors)
{
	return UGameplayStatics::GetAllActorsOfClass(this,ActorClass,Actors);
}

void ACameraViewInfoFactory::GetAllActorsByTag(FName Tag, TArray<AActor*>& Actors)
{
	return UGameplayStatics::GetAllActorsWithTag(this,Tag,Actors);
}

void ACameraViewInfoFactory::GetAllActorsFromClassAndTag(FName Tag, TSubclassOf<AActor> ActorClass,
	TArray<AActor*>& Actors)
{
	return UGameplayStatics::GetAllActorsOfClassWithTag(this,ActorClass,Tag,Actors);
}


