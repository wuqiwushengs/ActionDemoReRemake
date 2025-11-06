
#include "CollisionSystem/CollisionType.h"

#include "KismetTraceUtils.h"
#include "CollisionSystem/Interface/CollisionSystemInterface.h"
#include "Evaluation/Blending/MovieSceneBlendType.h"

void FCollisionShapeInfo::SetCollisionShape()
{
	switch(CollisionShape)
	{
	case ECollisionShapeInfo::Box :
		SelectedCollisionShape.ShapeType=ECollisionShape::Type::Box;
		break;
	case ECollisionShapeInfo::Capsule:
		SelectedCollisionShape.ShapeType=ECollisionShape::Type::Capsule;
		break;
	case ECollisionShapeInfo::Line:
		SelectedCollisionShape.ShapeType=ECollisionShape::Type::Line;
		break;
	case ECollisionShapeInfo::Sphere:
		SelectedCollisionShape.ShapeType=ECollisionShape::Type::Sphere;
		break;
	}
	switch (CollisionShape)
	{
	case ECollisionShapeInfo::Line:
		break;
	case ECollisionShapeInfo::Box :
		SelectedCollisionShape.SetBox(BoxCollisionInfo);
		break;
	case ECollisionShapeInfo::Capsule :
		SelectedCollisionShape.SetCapsule(CapsuleInfo.X,CapsuleInfo.Y);
		break;
	case ECollisionShapeInfo::Sphere :
		SelectedCollisionShape.SetSphere(SphereRadiusInfo);
		break;
	}
}

FCollisionShape FCollisionShapeInfo::GetCollisionShape()
{
	return SelectedCollisionShape;
}

TArray<FHitResult> FCollisionContext::GlobalTraceChannel(AActor* Tracer, FName SocketName,
	FCollisionInfoSum CollisionInfoSum,FVector Rootoffset,FRotator Rotationoffset)
{		TArray<FHitResult> HitResults;
		FHitResult SingleHitResult;
		FCollisionQueryParams QueryParam=CollisionInfoSum.GetCollisionQueryParams();
		if(Tracer&&CollisionInfoSum.bIgnoreSelf) QueryParam.AddIgnoredActor(Tracer);
	FVector BaseVector=Tracer->GetActorLocation()+Rootoffset;
	FRotator BaseRotator=Tracer->GetActorRotation()+Rotationoffset;
#if WITH_EDITOR
	if (GIsEditor&&!IsRunningGame()&&!Tracer->GetWorld()->IsGameWorld())
	{
		BaseVector=Tracer->GetActorLocation();
		BaseRotator=Tracer->GetActorRotation();
	}
#endif
		
	    FVector Start=  BaseVector+BaseRotator.RotateVector(CollisionInfoSum.CollisionPoints.StartPoint);
		FVector End=BaseVector+BaseRotator.RotateVector(CollisionInfoSum.CollisionPoints.EndPoint);
		if(CollisionInfoSum.bMultiTrace)
		{
			bool bHit=Tracer->GetWorld()->SweepMultiByChannel(HitResults,Start
				, End,FQuat(),CollisionInfoSum.Channel,CollisionInfoSum.ShapeInfo.
				GetCollisionShape(),QueryParam);
			DebugTraceSweep(Tracer,HitResults,CollisionInfoSum,Start
				,End,bHit,CollisionInfoSum.LinearColor);
			
		}
		else
		{
			bool bHit=Tracer->GetWorld()->SweepSingleByChannel(SingleHitResult,Start
				,End,FQuat(),CollisionInfoSum.Channel,CollisionInfoSum.ShapeInfo.
				GetCollisionShape(),QueryParam);
			DebugTraceSweepSingle(Tracer,SingleHitResult,CollisionInfoSum,Start
				,End,bHit,CollisionInfoSum.LinearColor);
			if(SingleHitResult.bBlockingHit)
			{
				HitResults.Add(SingleHitResult);
			}
		}
		return  HitResults;
}

TArray<FHitResult> FCollisionContext::SkeletalMeshTraceChannel(AActor* Tracer, FName SocketName,
                                                               FCollisionInfoSum CollisionInfoSum)
{
	AActor *	CSI=Tracer;
		TArray<FHitResult> HitResults;
		FCollisionQueryParams QueryParam=CollisionInfoSum.GetCollisionQueryParams();
		if(Tracer &&CollisionInfoSum.bIgnoreSelf) QueryParam.AddIgnoredActor(Tracer);
	ICollisionSystemInterface * CSIReal=Cast<ICollisionSystemInterface>(CSI);
		if(CSI &&CSIReal&& ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI))
		{
			FVector Start= ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->GetLastUpdateSocketLocation(SocketName,ECollisionType::SkeletalMesh);
			FVector End= ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->GetCurrentUpdateSocketLocation(SocketName,ECollisionType::SkeletalMesh);
			if(Start.Equals(End)||Start.Length()<10||End.Length()<10) return HitResults;
			if(CollisionInfoSum.bMultiTrace)
			{
				TArray<FHitResult> Results;
				bool bHit=Tracer->GetWorld()->SweepMultiByChannel(Results,Start,End,FQuat(),CollisionInfoSum.Channel,CollisionInfoSum.GetCollisionShape(),QueryParam);
				if(Results.Num()>0)
				{
					HitResults.Append(Results);
				}
				DebugTraceSweep(Tracer,HitResults,CollisionInfoSum,Start,End,bHit,CollisionInfoSum.LinearColor);
			}
			else
			{
				FHitResult Result;
				bool bHit=Tracer->GetWorld()->SweepSingleByChannel(Result,Start,End,FQuat(),CollisionInfoSum.Channel,CollisionInfoSum.GetCollisionShape(),QueryParam);
				DebugTraceSweepSingle(Tracer,Result,CollisionInfoSum,Start,End,bHit,CollisionInfoSum.LinearColor);
				if(Result.bBlockingHit)
				{
					HitResults.Add(Result);
				}
			}
		}
		return HitResults;
}

TArray<FHitResult> FCollisionContext::StaticMeshTraceChannel(AActor* Tracer, FName SocketName,
	FCollisionInfoSum CollisionInfoSum)
{
		AActor *	CSI=Tracer;
		TArray<FHitResult> HitResults;
		FCollisionQueryParams QueryParam=CollisionInfoSum.GetCollisionQueryParams();
		if(Tracer && CollisionInfoSum.bIgnoreSelf ) QueryParam.AddIgnoredActor(Tracer);
	ICollisionSystemInterface * CSIReal=Cast<ICollisionSystemInterface>(CSI);
		if(CSI && CSIReal&& ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI))
		{
			FVector Start= ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->GetLastUpdateSocketLocation(SocketName,ECollisionType::StaticMesh);
			FVector End= ICollisionSystemInterface::Execute_GetAttackCollisionComponent(CSI)->GetCurrentUpdateSocketLocation(SocketName,ECollisionType::StaticMesh);
			if(Start.Equals(End)||Start.Length()<10||End.Length()<10) return HitResults;
			if(CollisionInfoSum.bMultiTrace)
			{
				TArray<FHitResult> Results;
				bool bHit=Tracer->GetWorld()->SweepMultiByChannel(Results,Start,End,FQuat(),CollisionInfoSum.Channel,CollisionInfoSum.GetCollisionShape(),QueryParam);
				if(Results.Num()>0)
				{
					HitResults.Append(Results);
				}
				DebugTraceSweep(Tracer,HitResults,CollisionInfoSum,Start,End,bHit,CollisionInfoSum.LinearColor);
			}
			else
			{
				FHitResult Result;
				bool bHit=Tracer->GetWorld()->SweepSingleByChannel(Result,Start,End,FQuat(),CollisionInfoSum.Channel,CollisionInfoSum.GetCollisionShape(),QueryParam);
				DebugTraceSweepSingle(Tracer,Result,CollisionInfoSum,Start,End,bHit,CollisionInfoSum.LinearColor);
				if(Result.bBlockingHit)
				{
					HitResults.Add(Result);
				}
			}
		}
		return HitResults;
}
void FCollisionContext::DebugTraceSweep(AActor * Tracer,const TArray<FHitResult>& HitResults, const FCollisionInfoSum& CollisionInfoSum,const FVector & Start, const FVector & End,bool bHit,FLinearColor DebugColor)
{	
	
	switch (CollisionInfoSum.ShapeInfo.CollisionShape)
	{
	case ECollisionShapeInfo::Box:
		{
			FVector BoxExtend = FVector(CollisionInfoSum.ShapeInfo.BoxCollisionInfo);
			DrawDebugBoxTraceMulti(Tracer->GetWorld(),Start,End,BoxExtend,FRotator(),CollisionInfoSum.DebugType,bHit,HitResults,
				DebugColor,FLinearColor::Green,CollisionInfoSum.DebugTime);
			break;
		}
	case ECollisionShapeInfo::Capsule:
		DrawDebugCapsuleTraceMulti(Tracer->GetWorld(),Start,End,CollisionInfoSum.ShapeInfo.CapsuleInfo.X,
			CollisionInfoSum.ShapeInfo.CapsuleInfo.Y,CollisionInfoSum.DebugType,bHit,HitResults,DebugColor,FLinearColor::Green,CollisionInfoSum.DebugTime);
		break;
	case ECollisionShapeInfo::Line:
		DrawDebugLineTraceMulti(Tracer->GetWorld(),Start,End,CollisionInfoSum.DebugType,bHit,HitResults,DebugColor,FLinearColor::Green,CollisionInfoSum.DebugTime);
		break;
	case ECollisionShapeInfo::Sphere:
		DrawDebugSphereTraceMulti(Tracer->GetWorld(),Start,End,CollisionInfoSum.ShapeInfo.SphereRadiusInfo,
			CollisionInfoSum.DebugType,bHit,HitResults,DebugColor,FLinearColor::Green,CollisionInfoSum.DebugTime);
		break;
	}
}
void FCollisionContext::DebugTraceSweepSingle(AActor * Tracer,FHitResult & HitResults, const FCollisionInfoSum& CollisionInfoSum,const FVector & Start, const FVector & End,bool bHit,FLinearColor DebugColor)
{	
	switch (CollisionInfoSum.ShapeInfo.CollisionShape)
	{
	case ECollisionShapeInfo::Box:
		{
			FVector BoxExtend = FVector(CollisionInfoSum.ShapeInfo.BoxCollisionInfo);
			DrawDebugBoxTraceSingle(Tracer->GetWorld(),Start,End,BoxExtend,FRotator(),CollisionInfoSum.DebugType,bHit,HitResults,
				FLinearColor::Red,FLinearColor::Green,CollisionInfoSum.DebugTime);
			break;
		}
	case ECollisionShapeInfo::Capsule:
		DrawDebugCapsuleTraceSingle(Tracer->GetWorld(),Start,End,CollisionInfoSum.ShapeInfo.CapsuleInfo.X,
			CollisionInfoSum.ShapeInfo.CapsuleInfo.Y,CollisionInfoSum.DebugType,bHit,HitResults,DebugColor,FLinearColor::Green,CollisionInfoSum.DebugTime);
		break;
	case ECollisionShapeInfo::Line:
		DrawDebugLineTraceSingle(Tracer->GetWorld(),Start,End,CollisionInfoSum.DebugType,bHit,HitResults,DebugColor,FLinearColor::Green,CollisionInfoSum.DebugTime);
		break;
	case ECollisionShapeInfo::Sphere:
		DrawDebugSphereTraceSingle(Tracer->GetWorld(),Start,End,CollisionInfoSum.ShapeInfo.SphereRadiusInfo,
			CollisionInfoSum.DebugType,bHit,HitResults,DebugColor,FLinearColor::Green,CollisionInfoSum.DebugTime);
		break;
	}
}
