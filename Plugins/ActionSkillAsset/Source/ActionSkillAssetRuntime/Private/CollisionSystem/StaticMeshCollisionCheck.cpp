// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionSystem/StaticMeshCollisionCheck.h"
#include "CollisionSystem/Interface/CollisionSystemInterface.h"

#if WITH_EDITOR
void UStaticMeshCollisionCheck::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	Super::OnAnimNotifyCreatedInEditor(ContainingAnimNotifyEvent);
	EditCollisionContext.CollisionType=ECollisionType::StaticMesh;
}
void UStaticMeshCollisionCheck::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	EditCollisionContext.SetCollisionInfo();
	if(PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName()==TEXT("CollisionType"))
	{	//强制改成GlobalCollision
		if(EditCollisionContext.CollisionType !=ECollisionType::StaticMesh)
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("InstanceCollisionCheck must be StaticMesh")));
			EditCollisionContext.CollisionType=ECollisionType::StaticMesh;
		}
	}
}
#endif
void UStaticMeshCollisionCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	AActor *	CSI=MeshComp->GetOwner();
	if(CSI&&CSI->GetClass()->ImplementsInterface(UCollisionSystemInterface::StaticClass()))
	{	TArray<FName> SocketNames;
		EditCollisionContext.GetCollisionInfo().GenerateKeyArray(SocketNames);
		ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->StartTrace(SocketNames,EditCollisionContext.CollisionType,EditCollisionContext.SocketStaticMesh);
	}
}

void UStaticMeshCollisionCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if(!(MeshComp &&MeshComp->GetOwner())) return;
	TArray<FHitResult> FinalHitResult;
	for (TPair<FName,FCollisionInfoSum> CollisionInfo : EditCollisionContext.GetCollisionInfo())
	{
		FinalHitResult.Append(FCollisionContext::StaticMeshTraceChannel(
			MeshComp->GetOwner(),CollisionInfo.Key,CollisionInfo.Value));
	}
	AActor *	CSI=MeshComp->GetOwner();
	if(CSI && !FinalHitResult.IsEmpty() &&CSI->GetClass()->ImplementsInterface(UCollisionSystemInterface::StaticClass()))
	{
		ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->OnAttacktoTagretRecall(FinalHitResult);
	}
}

void UStaticMeshCollisionCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
									const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	AActor *	CSI=MeshComp->GetOwner();
	if(CSI &&CSI->GetClass()->ImplementsInterface(UCollisionSystemInterface::StaticClass()))
	{
		TArray<FName> SocketNames;
		EditCollisionContext.GetCollisionInfo().GenerateKeyArray(SocketNames);
		ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->EndTrace(SocketNames,EditCollisionContext.CollisionType,EditCollisionContext.SocketStaticMesh);
	}
}


