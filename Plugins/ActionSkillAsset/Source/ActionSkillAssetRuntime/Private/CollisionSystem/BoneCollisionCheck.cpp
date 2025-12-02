// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionSystem/BoneCollisionCheck.h"

#include "CollisionSystem/Interface/CollisionSystemInterface.h"



void UBoneCollisionCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	AActor *	CSI=MeshComp->GetOwner();
	if(CSI&&CSI->GetClass()->ImplementsInterface(UCollisionSystemInterface::StaticClass()))
	{	TArray<FName> SocketNames;
		EditCollisionContext.GetCollisionInfo().GenerateKeyArray(SocketNames);
		ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->StartTrace(SocketNames,EditCollisionContext.CollisionType);
	}
}

void UBoneCollisionCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	TArray<FHitResult> FinalHitResult;
	for (TPair<FName,FCollisionInfoSum> CollisionInfo : EditCollisionContext.GetCollisionInfo())
	{
		FinalHitResult.Append(FCollisionContext::SkeletalMeshTraceChannel(
			MeshComp->GetOwner(),CollisionInfo.Key,CollisionInfo.Value));
	}
	AActor *	CSI=MeshComp->GetOwner();
	if(CSI && !FinalHitResult.IsEmpty()&&CSI->GetClass()->ImplementsInterface(UCollisionSystemInterface::StaticClass()))
	{
		ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->OnAttacktoTagretRecall(FinalHitResult);
	}
}
void UBoneCollisionCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	AActor *	CSI=MeshComp->GetOwner();
	if(CSI&&CSI->GetClass()->ImplementsInterface(UCollisionSystemInterface::StaticClass()))
	{
		TArray<FName> SocketNames;
		EditCollisionContext.GetCollisionInfo().GenerateKeyArray(SocketNames);
		ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->EndTrace(EditCollisionContext.CollisionType,SocketNames);
	}
}
#if WITH_EDITOR
void UBoneCollisionCheck::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	Super::OnAnimNotifyCreatedInEditor(ContainingAnimNotifyEvent);
	EditCollisionContext.CollisionType=ECollisionType::SkeletalMesh;
}
void UBoneCollisionCheck::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	EditCollisionContext.SetCollisionInfo();
	if(PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName()==TEXT("CollisionType"))
	{	//强制改成GlobalCollision
		if(EditCollisionContext.CollisionType !=ECollisionType::SkeletalMesh)
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("InstanceCollisionCheck must be SkeletalMesh")));
			EditCollisionContext.CollisionType=ECollisionType::SkeletalMesh;
		}
	}
}
#endif
