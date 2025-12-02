// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlay/Character/ActionAttackCollisionComponent.h"

#include "CollisionSystem/Interface/CollisionSystemInterface.h"


// Sets default values for this component's properties
UActionAttackCollisionComponent::UActionAttackCollisionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UActionAttackCollisionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UActionAttackCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                    FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UActionAttackCollisionComponent::OnAttacktoTagretRecall(TArray<FHitResult> HitResults)
{
	FAttackedResult  AttackedResult;
	AActor * Self=GetOwner();
	//目前只对攻击到的角色进行处理
	if(HitResults.Num()<=0) return;
	for(FHitResult Result:HitResults)
	{
		if(!Result.GetActor()) continue;
		AttackedResult.Attacker=Self;
		AttackedResult.HitResult=Result;
		AttackedResult.DamageAmount=1.0f;
		if(Result.GetActor()->GetClass()->ImplementsInterface(UCollisionSystemInterface::StaticClass()))
		{
			ICollisionSystemInterface::Execute_GetAttackCollisionComponent(Result.GetActor())->OnBeAttackedRecall(AttackedResult);
		}	
		
	}
}
void UActionAttackCollisionComponent::OnBeAttackedRecall(FAttackedResult AttackedResult)
{
	if(CanHurt)
	{
		ICollisionSystemInterface::Execute_OnBeAttackRecallInternal(GetOwner(),AttackedResult);
	}
	CanHurt=false;
	if(!AttackHandle.IsValid()||!GetOwner()->GetWorld()->GetTimerManager().IsTimerActive(AttackHandle))
	{
		GetOwner()->GetWorld()->GetTimerManager().SetTimer(AttackHandle,[this]()
		{
		   CanHurt=true;		
		},bHurtCoolTime,false);
	}
	
}

