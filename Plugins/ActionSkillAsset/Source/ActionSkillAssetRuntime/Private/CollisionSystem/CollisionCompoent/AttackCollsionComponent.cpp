// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionSystem/CollisionType.h"
#include "CollisionSystem/CollisionCompoent/AttackCollisionComponent.h"


UAttackCollisionComponent::UAttackCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

FVector UAttackCollisionComponent::GetLastUpdateSocketLocation_Implementation(const FName & SocketName,ECollisionType CollisionType)
{
	check(CollisionType!=ECollisionType::GlobalCollision);
	return   TraceInfo.Find(CollisionType)?TraceInfo.Find(CollisionType)->LastSocketLocation.FindOrAdd(SocketName):FVector();
}

FVector UAttackCollisionComponent::GetCurrentUpdateSocketLocation_Implementation(const FName & SocketName,ECollisionType CollisionType)
{
	check(CollisionType!=ECollisionType::GlobalCollision);
	return  TraceInfo.Find(CollisionType)?TraceInfo.Find(CollisionType)->CurrentSocketLocation.FindOrAdd(SocketName):FVector();
}

void UAttackCollisionComponent::StartTrace(const TArray<FName>& TraceFollowName, ECollisionType CollisionCategory)
{
	//通用检测不应该使用骨骼信息
	if(CollisionCategory==ECollisionType::GlobalCollision) return ;
	TraceInfo.FindOrAdd(CollisionCategory).SocketNames.Append(TraceFollowName);
	TraceNum+=TraceFollowName.Num();
}

void UAttackCollisionComponent::StartTrace(const TArray<FName>& TraceFollowName, ECollisionType CollisionCategory,
	UStaticMesh* AttachStaticMesh)
{
	
	if(CollisionCategory!=ECollisionType::StaticMesh) return;
	TraceInfo.FindOrAdd(CollisionCategory).SocketNames.Append(TraceFollowName);
	TraceNum+=TraceFollowName.Num();
	for(auto & MeshInfo:StaticMeshComponentArray)
	{
		if(MeshInfo.Key->GetStaticMesh()&&MeshInfo.Key->GetStaticMesh()==AttachStaticMesh)
		{
			MeshInfo.Value.Append(TraceFollowName);
		}
	}
}

void UAttackCollisionComponent::EndTrace(ECollisionType CollisionType, const TArray<FName>& TraceFollowName)
{
	//通用检测不应该取消骨骼信息
	if(CollisionType==ECollisionType::GlobalCollision) return;
	if(TraceInfo.Find(CollisionType))
	{
		for (FName  TraceName: TraceFollowName)
		{
			if(!TraceInfo.Find(CollisionType)->SocketNames.Find(TraceName)) continue;
			TraceInfo.Find(CollisionType)->SocketNames.Remove (TraceName);
			TraceInfo.Find(CollisionType)->CurrentSocketLocation.Remove(TraceName);
			TraceInfo.Find(CollisionType)->LastSocketLocation.Remove(TraceName);
		}
	}
	TraceNum-=TraceFollowName.Num();
}

void UAttackCollisionComponent::EndTrace(const TArray<FName>& TraceFollowName, ECollisionType CollisionCategory,
	UStaticMesh* AttachStaticMesh)
{

	if(CollisionCategory!=ECollisionType::StaticMesh) return;
	if(TraceInfo.Find(CollisionCategory))
	{
		for (FName  TraceName: TraceFollowName)
		{
			if(!TraceInfo.Find(CollisionCategory)->SocketNames.Find(TraceName)) continue;
			TraceInfo.Find(CollisionCategory)->SocketNames.Remove (TraceName);
			TraceInfo.Find(CollisionCategory)->CurrentSocketLocation.Remove(TraceName);
			TraceInfo.Find(CollisionCategory)->LastSocketLocation.Remove(TraceName);
		}
		//移除Mesh映射中的SocketName内容
		for(auto & MeshInfo: StaticMeshComponentArray)
		{
			if(MeshInfo.Key->GetStaticMesh()&& MeshInfo.Key->GetStaticMesh()==AttachStaticMesh)
			{
				for(FName Name:TraceFollowName)
				{
					MeshInfo.Value.Remove(Name);
				}
				break;
			}
		}
	}
	TraceNum-=TraceFollowName.Num();
}

USkeletalMeshComponent* UAttackCollisionComponent::GetSkeletalMeshComponent()
{
	return SkeletalMeshComponent;
}

TArray<TObjectPtr<UStaticMeshComponent>> UAttackCollisionComponent::GetStaticMeshComponent()
{
	TArray<TObjectPtr<UStaticMeshComponent>> MeshArray;
	StaticMeshComponentArray.GetKeys(MeshArray);
	return  MeshArray;
}

TArray<FName> UAttackCollisionComponent::GetSocketNames(ECollisionType CollisionType)
{
	return TraceInfo.Find(CollisionType)?TraceInfo.Find(CollisionType)->SocketNames.Array():TArray<FName>();
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
	StaticMeshComponentArray.Add(StaticMesh);
}

void UAttackCollisionComponent::SetSkeletalMesh(USkeletalMeshComponent* SkeletalMesh)
{
	SkeletalMeshComponent=SkeletalMesh;
}
void UAttackCollisionComponent::SetStaticMesh(UStaticMeshComponent* StaticMesh)
{
	StaticMeshComponentArray.Add(StaticMesh);
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
	
	if(TraceNum<=0) return;
	if(TraceInfo.Find(ECollisionType::SkeletalMesh)&&TraceInfo.Find(ECollisionType::SkeletalMesh)->SocketNames.Num()>0)
	{
		UpdateSkeletalMeshSocketLocation();
	}
	if(TraceInfo.Find(ECollisionType::StaticMesh)&&TraceInfo.Find(ECollisionType::StaticMesh)->SocketNames.Num()>0)
	{
		UpdateStaticMeshSocketLocation();
	}
}
	

void UAttackCollisionComponent::UpdateSkeletalMeshSocketLocation()
{
	if(!SkeletalMeshComponent ) return ;
	TArray<FName> IgnoreName;
		for(FName & SocketName:TraceInfo[ECollisionType::SkeletalMesh].SocketNames)
		{
			if (!SkeletalMeshComponent->GetSocketByName(SocketName) && SkeletalMeshComponent->GetBoneIndex(SocketName) == INDEX_NONE)
			{
				IgnoreName.Add(SocketName);
				continue;
			}

			// 获取插槽位置
			FVector SocketLocation = SkeletalMeshComponent->GetSocketLocation(SocketName);
			// 缓存 TraceInfo 查找结果
			auto* SkeletalMeshTraceInfo = TraceInfo.Find(ECollisionType::SkeletalMesh);
			if (!SkeletalMeshTraceInfo) continue;
			// 更新 LastSocketLocation 和 CurrentSocketLocation
			FVector& LastLocation = SkeletalMeshTraceInfo->LastSocketLocation.FindOrAdd(SocketName);
			FVector& CurrentLocation = SkeletalMeshTraceInfo->CurrentSocketLocation.FindOrAdd(SocketName);
			if (LastLocation.IsNearlyZero())
			{
				LastLocation = SocketLocation;
			}
			else
			{
				LastLocation = CurrentLocation;
			}
			CurrentLocation = SocketLocation;
			
		}
		//移除不符合要求的Name
		if(IgnoreName.Num()>0)
		{
		EndTrace(ECollisionType::SkeletalMesh,IgnoreName);
		}
}
void UAttackCollisionComponent::UpdateStaticMeshSocketLocation()
{	if(StaticMeshComponentArray.Num()<=0 ) return ;
	TArray<FName> IgnoreName;
	for(auto MeshInfo: StaticMeshComponentArray)
	{
		//更新信息
		for(FName SocketName:MeshInfo.Value)
		{
			if(!MeshInfo.Key->GetSocketByName(SocketName))
			{
				IgnoreName.Add(SocketName);
				continue;
			}
			// 获取插槽位置
			FVector SocketLocation = MeshInfo.Key->GetSocketLocation(SocketName);
			// 缓存 TraceInfo 查找结果
			auto* StaticMeshTraceInfo = TraceInfo.Find(ECollisionType::StaticMesh);
			if (!StaticMeshTraceInfo) continue;

			FVector& LastLocation = StaticMeshTraceInfo->LastSocketLocation.FindOrAdd(SocketName);
			FVector& CurrentLocation = StaticMeshTraceInfo->CurrentSocketLocation.FindOrAdd(SocketName);

			// 更新 LastSocketLocation 和 CurrentSocketLocation
			if (LastLocation.IsNearlyZero())
			{
				LastLocation = SocketLocation;
			}
			else
			{
				LastLocation = CurrentLocation;
			}
			CurrentLocation = SocketLocation;
		}
		//移除不符合要求的Name
		if(IgnoreName.Num()>0)
		{
			EndTrace(IgnoreName,ECollisionType::StaticMesh,MeshInfo.Key->GetStaticMesh());
		}
	}
}

