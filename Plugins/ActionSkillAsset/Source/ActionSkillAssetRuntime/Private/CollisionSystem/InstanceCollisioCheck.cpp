// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionSystem/InstanceCollisioCheck.h"

#include "CollisionSystem/Interface/CollisionSystemInterface.h"
#include "UObject/ObjectSaveContext.h"

void UInstanceCollisioCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
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
	if(EditCollisionContext.GetCollisionInfo().Num()<=0) return;
	TArray<FHitResult> FinalHitResult;
	for (TPair<FName,FCollisionInfoSum> CollisionInfo : EditCollisionContext.GetCollisionInfo())
	{
		
	FinalHitResult.Append(FCollisionContext::GlobalTraceChannel(
		MeshComp->GetOwner(),CollisionInfo.Key,CollisionInfo.Value,TraceBaseOffset,RotationBaseOffset));
	}
	AActor *	CSI=MeshComp->GetOwner();
	if(CSI && !FinalHitResult.IsEmpty()&&CSI->GetClass()->ImplementsInterface(UCollisionSystemInterface::StaticClass()))
	{
		TArray<FName> SocketNames;
		ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->StartTrace(SocketNames,EditCollisionContext.CollisionType);
		ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->OnAttacktoTagretRecall(FinalHitResult);
		ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->EndTrace(EditCollisionContext.CollisionType,SocketNames);
	}
}
#if WITH_EDITOR
void UInstanceCollisioCheck::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{	//主要是要修改这里要对更改数值时进行同步处理。
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
#endif


