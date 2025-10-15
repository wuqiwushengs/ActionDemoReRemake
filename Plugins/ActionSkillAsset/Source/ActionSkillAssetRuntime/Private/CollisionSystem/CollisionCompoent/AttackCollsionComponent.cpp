// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionSystem/CollisionType.h"
#include "CollisionSystem/CollisionCompoent/AttackCollisionComponent.h"



UAttackCollisionComponent::UAttackCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

FVector UAttackCollisionComponent::GetLastUpdateSocketLocation_Implementation(const FName & SocketName)
{
	return LastSocketLocation.FindOrAdd(SocketName);
}

FVector UAttackCollisionComponent::GetCurrentUpdateSocketLocation_Implementation(const FName & SocketName)
{
	return  CurrentSocketLocation.FindOrAdd(SocketName);
}

void  UAttackCollisionComponent::StartTrace_Implementation(const TArray<FName> &TraceFollowName, ECollisionType CollisionCategory)
{
	bStartTrace=true;
	SocketNames=TraceFollowName;
	CollisionType=CollisionCategory;
}
void UAttackCollisionComponent::EndTrace_Implementation()
{

	SocketNames.Empty();
	CurrentSocketLocation.Empty();
	LastSocketLocation.Empty();
	bStartTrace=false;
}

USkeletalMeshComponent* UAttackCollisionComponent::GetSkeletalMeshComponent()
{
	return SkeletalMeshComponent;
}

UStaticMeshComponent* UAttackCollisionComponent::GetStaticMeshComponent()
{
	return StaticMeshComponent;
}

TArray<FName> UAttackCollisionComponent::GetSocketNames()
{
	return SocketNames;
}
void UAttackCollisionComponent::OnAttacktoTagretRecall(TArray<FHitResult> HitResults)
{
	
}

void UAttackCollisionComponent::OnBeAttackedRecall(FAttackedResult AttackedResult)
{
	
}

// Called when the game starts
void UAttackCollisionComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	
}


void UAttackCollisionComponent::SetSkeletalMeshAndStaticMesh(USkeletalMeshComponent* SkeletalMesh,UStaticMeshComponent *StaticMesh)
{
	SkeletalMeshComponent=SkeletalMesh;
	StaticMeshComponent=StaticMesh;
}

void UAttackCollisionComponent::SetSkeletalMesh(USkeletalMeshComponent* SkeletalMesh)
{
	SkeletalMeshComponent=SkeletalMesh;
}
void UAttackCollisionComponent::SetStaticMesh(UStaticMeshComponent* StaticMesh)
{
	StaticMeshComponent=StaticMesh;
}

// Called every frame
void UAttackCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateSocketLocation();
	// ...
}

void UAttackCollisionComponent::UpdateSocketLocation()
{
	if(!bStartTrace) return;
	switch (CollisionType)
	{
	case  ECollisionType::SkeletalMesh :
		UpdateSkeletalMeshSocketLocation();
		break;
	case ECollisionType::StaticMesh:
		UpdateStaticMeshSocketLocation();
		break;
	default:
			break;
	}
	
	
}
void UAttackCollisionComponent::UpdateSkeletalMeshSocketLocation()
{
	if(!SkeletalMeshComponent) return ;
		for(FName & SocketName:SocketNames)
		{
			FVector SocketLocation=SkeletalMeshComponent->GetSocketLocation(SocketName);
			if(LastSocketLocation.FindOrAdd(SocketName).IsNearlyZero())
			{
				LastSocketLocation.FindOrAdd(SocketName)=SocketLocation;
			}
			else
			{
				LastSocketLocation.FindOrAdd(SocketName)=CurrentSocketLocation.FindOrAdd(SocketName);
			}
			CurrentSocketLocation.FindOrAdd(SocketName)=SocketLocation;
		}
}
void UAttackCollisionComponent::UpdateStaticMeshSocketLocation()
{	if(!StaticMeshComponent) return ;
	for(FName & SocketName:SocketNames)
	{
		FVector SocketLocation=StaticMeshComponent->GetSocketLocation(SocketName);
		if(LastSocketLocation.FindOrAdd(SocketName).IsNearlyZero())
		{
			LastSocketLocation.FindOrAdd(SocketName)=SocketLocation;
		}
		else
		{
			LastSocketLocation.FindOrAdd(SocketName)=CurrentSocketLocation.FindOrAdd(SocketName);
		}
		CurrentSocketLocation.FindOrAdd(SocketName)=SocketLocation;
	}
}

