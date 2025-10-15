// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionSystem/ContinueGlobalCollisionCheck.h"

#include "CollisionSystem/Interface/CollisionSystemInterface.h"

void UContinueGlobalCollisionCheck::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	Super::OnAnimNotifyCreatedInEditor(ContainingAnimNotifyEvent);
	EditCollisionContext.CollisionType=ECollisionType::GlobalCollision;
}
void UContinueGlobalCollisionCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if(!MeshComp->GetOwner()) return;
	//这里实际上处理的是GlobalCollision
#if WITH_EDITOR
	if(MeshComp->GetOwner()->GetWorld() &&!Cast<ICollisionSystemInterface>(MeshComp->GetOwner()))
	{
		if (GIsEditor&&!IsRunningGame()&&!MeshComp->GetOwner()->GetWorld()->IsGameWorld() )
		{
			EditCollisionContext.SetPreviewWorld(MeshComp->GetOwner()->GetWorld());
			EditCollisionContext.RefreshTargetPointAndSaveAll();
		}
 		
	}
#endif
	AActor *	CSI=MeshComp->GetOwner();
	ICollisionSystemInterface * CSIReal=Cast<ICollisionSystemInterface>(CSI);
	if(CSI&&CSIReal)
	{
		TArray<FName> SocketNames;
		ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->StartTrace(SocketNames,EditCollisionContext.CollisionType);
	}
}

void UContinueGlobalCollisionCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if(EditCollisionContext.GetCollisionInfo().Num()<=0) return;
	TArray<FHitResult> FinalHitResult;
	for (TPair<FName,FCollisionInfoSum> CollisionInfo : EditCollisionContext.GetCollisionInfo())
	{
		
		FinalHitResult.Append(FCollisionContext::GlobalTraceChannel(
			MeshComp->GetOwner(),CollisionInfo.Key,CollisionInfo.Value,TraceBaseOffset,RotationBaseOffset));
	}
	AActor *	CSI=MeshComp->GetOwner();
	ICollisionSystemInterface * CSIReal=Cast<ICollisionSystemInterface>(CSI);
	
	if(CSI && !FinalHitResult.IsEmpty()&&CSIReal)
	{
		ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->OnAttacktoTagretRecall(FinalHitResult);
		
	}
}

void UContinueGlobalCollisionCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	AActor *	CSI=MeshComp->GetOwner();
	ICollisionSystemInterface * CSIReal=Cast<ICollisionSystemInterface>(CSI);
	if(CSI &&CSIReal)
	{
		ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->EndTrace();
	}
}

void UContinueGlobalCollisionCheck::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	EditCollisionContext.SetCollisionInfo();
	if(PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName()==TEXT("CollisionType"))
	{	//强制改成GlobalCollision
		if(EditCollisionContext.CollisionType !=ECollisionType::GlobalCollision)
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("InstanceCollisionCheck must be GlobalCollision")));
			EditCollisionContext.CollisionType=ECollisionType::GlobalCollision;
		}
	}
	if(PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName()==TEXT("Icon"))
	{
		EditCollisionContext.RefreshTargetIcon();
	}
	//修改变量
	if(PropertyChangedEvent.Property->GetName()==TEXT("X") || PropertyChangedEvent.Property->GetName()==TEXT("Y")|| PropertyChangedEvent.Property->GetName()==TEXT("Z")
		&& PropertyChangedEvent.ChangeType==EPropertyChangeType::ValueSet)
	{
		for (TPair<FName,FCollisionInfoSum> & CollisionInfo :EditCollisionContext.GlobalCollision)
		{	if(!EditCollisionContext.PreviewWorld) break;
			CollisionInfo.Value.SetTargetPoint();
		}
	}
	//更改数组。
	if(PropertyChangedEvent.ChangeType&(EPropertyChangeType::ArrayAdd |EPropertyChangeType::ArrayRemove|EPropertyChangeType::ArrayClear))
	{
		EditCollisionContext.RefreshTargetPointAndSaveAll();
	}
}
