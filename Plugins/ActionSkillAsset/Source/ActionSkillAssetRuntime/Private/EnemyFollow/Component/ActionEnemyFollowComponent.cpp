// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFollow/Component/ActionEnemyFollowComponent.h"

#include "GamePlay/Character/ActionPlayerCharacter.h"


// Sets default values for this component's properties
UActionEnemyFollowComponent::UActionEnemyFollowComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UActionEnemyFollowComponent::BeginPlay()
{
	Super::BeginPlay();
	ControlledCharacter=CastChecked<AActionPlayerCharacter>(GetOwner());
	// ...
	
}


// Called every frame
void UActionEnemyFollowComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<AActor*> UActionEnemyFollowComponent::SphereTraceToFindEnemy(float Radiaus, float IgnoreDistance)
{
	FVector StartLocation=FVector(ControlledCharacter->GetActorLocation().X,ControlledCharacter->GetActorLocation().Y,ControlledCharacter->GetActorLocation().Z+10);
	TArray<AActor*> IgnoreActor;
	IgnoreActor.Add(ControlledCharacter);
	TArray<FHitResult> HitResults;
	TArray<AActor*> EnemyActor;
	UKismetSystemLibrary::CapsuleTraceMulti(ControlledCharacter,StartLocation,ControlledCharacter->GetActorLocation(),Radiaus,70,SphereTraceType,false,IgnoreActor,DrawDebug?EDrawDebugTrace::ForDuration:EDrawDebugTrace::None,HitResults,true);
	for (FHitResult &HitResult:HitResults)
	{
		if (HitResult.bBlockingHit)
		{
			if (HitResult.GetActor()&&HitResult.GetActor()->GetClass()->IsChildOf(EnemySelectedClass)&&(FVector::DistXY(HitResult.GetActor()->GetActorLocation(),ControlledCharacter->GetActorLocation()))>IgnoreDistance)
			{
				EnemyActor.AddUnique(HitResult.GetActor());
			}
		}
	}
	return EnemyActor;
	
}

void UActionEnemyFollowComponent::TraceToInitEnemy()
{
	//两次检测近距离和中距离的敌人
	TArray<AActor*> NearEnemy=SphereTraceToFindEnemy(DefaultDistanceDetectInfo.NearRange,0);
	if (NearEnemy.Num()>0)
	{
		EnemySelect.Add(EDistanceDetect::Near,NearEnemy);
	}
	TArray<AActor* >MiddleEnemy=SphereTraceToFindEnemy(DefaultDistanceDetectInfo.MediumRange,DefaultDistanceDetectInfo.NearRange);
	if (MiddleEnemy.Num()>0)
	{
		EnemySelect.Add(EDistanceDetect::Medium,MiddleEnemy);
	}
}

FVector UActionEnemyFollowComponent::GetCatchActorLocation()
{
	if (CatchActor)
	{
		return CatchActor->GetActorLocation();
	}
	//再次进行一次检测并且获取最终的目标
		return  FVector::ZeroVector;
}

bool UActionEnemyFollowComponent::SelectedFinalActor()
{
	TraceToInitEnemy();
	FVector ActorLocation = ControlledCharacter->GetActorLocation();
	float CurrenDistance=FLT_MAX;
	TMap<AActor*, FRotator> EnemyInRangeInfo;
	if (CheckEnemyInRange(EDistanceDetect::Near, EnemyInRangeInfo))
	{
		AActor * BestActor=nullptr;
		for (const TPair<AActor*, FRotator>& Enemy:EnemyInRangeInfo)
		{
			float distance=FVector::Dist2D(ActorLocation,Enemy.Key->GetActorLocation());
			if (distance<CurrenDistance)
			{
				BestActor=Enemy.Key;
				CurrenDistance=distance;
			}
		}
		if (BestActor)
		{
			SetCaptureActor(BestActor);
			return true;
	    	
		}
	}
	EnemyInRangeInfo.Empty();
	if (CheckEnemyInRange(EDistanceDetect::Medium, EnemyInRangeInfo))
	{
		AActor * BestActor=nullptr;
		for (const TPair<AActor*, FRotator>& Enemy:EnemyInRangeInfo)
		{
			float distance=FVector::Dist2D(ActorLocation,Enemy.Key->GetActorLocation());
			if (distance<CurrenDistance)
			{
				BestActor=Enemy.Key;
				CurrenDistance=distance;
			}
		}
		if (BestActor)
		{
			SetCaptureActor(BestActor);
			return true;
	    	
		}
	}
	return false;
}
void UActionEnemyFollowComponent::ClearAttachedActor()
{
	CatchActor=nullptr;
}

void UActionEnemyFollowComponent::SetCaptureActor(AActor* CaptureActor)
{
	CatchActor=CaptureActor;
}

bool UActionEnemyFollowComponent::CheckEnemyInRange(EDistanceDetect Distance, TMap<AActor*, FRotator>& EnemyInRangeInfo)
{
	EnemyInRangeInfo.Empty();
	//检测这个范围内的敌人
	if (EnemySelect.Find(Distance)&&EnemySelect.Find(Distance)->Num()>0)
	{	TMap<AActor*,FRotator> EnemyAndValue;
		for (AActor*& Enemy:EnemySelect[Distance])
		{
			FRotator EnemyRotator=(Enemy->GetActorLocation()-ControlledCharacter->GetActorLocation()).GetSafeNormal2D().Rotation();
			TPair<AActor*,FRotator> EnemyInfo={Enemy,EnemyRotator};
			EnemyAndValue.Add(EnemyInfo);
		}
		if (EnemyAndValue.Num()>0)
		{
			EnemyInRangeInfo=EnemyAndValue;
			return true;
		}
		return false;
	
	}
	return false;
}

