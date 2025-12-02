// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionSystem/CollisionCompoent/AttackCollisionComponent.h"
#include "ActionAttackCollisionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONSKILLASSETRUNTIME_API UActionAttackCollisionComponent : public UAttackCollisionComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UActionAttackCollisionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnAttacktoTagretRecall(TArray<FHitResult> HitResults) override;
	virtual void OnBeAttackedRecall(FAttackedResult AttackedResult) override;
protected:
	//用来进行
	FTimerHandle AttackHandle;
	UPROPERTY(EditDefaultsOnly)
	float bHurtCoolTime=0.5f;
	UPROPERTY(EditDefaultsOnly)
	bool CanHurt=true;
};
