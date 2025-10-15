// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackCollisionComponent.generated.h"

USTRUCT(BlueprintType)
struct FAttackedResult
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FHitResult HitResult;
	UPROPERTY(BlueprintReadWrite)
	AActor * Attacker;
};
enum class ECollisionType : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONSKILLASSETRUNTIME_API UAttackCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAttackCollisionComponent();
	
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	FVector GetLastUpdateSocketLocation(const FName & SocketName);
	virtual FVector GetLastUpdateSocketLocation_Implementation( const FName & SocketName);
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	FVector GetCurrentUpdateSocketLocation(const FName & SocketName);
	virtual FVector GetCurrentUpdateSocketLocation_Implementation(const FName & SocketName);
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void  StartTrace(const TArray<FName> & TraceFollowName,ECollisionType CollisionCategory);
	virtual void StartTrace_Implementation(const TArray<FName> & TraceFollowName,ECollisionType CollisionCategory);
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void EndTrace();
	virtual void EndTrace_Implementation();
	USkeletalMeshComponent * GetSkeletalMeshComponent();
	UStaticMeshComponent * GetStaticMeshComponent();
	TArray<FName> GetSocketNames();
	void OnAttacktoTagretRecall(TArray<FHitResult> HitResults);
	
protected:
	//这个函数用来处理被打击的情况，通常会被OnAttacktoTargetRecall所调用
	void OnBeAttackedRecall(FAttackedResult AttackedResult);
	// Called when the game starts
	virtual void BeginPlay() override;
	TArray<FName> SocketNames;
	TMap<FName,FVector> CurrentSocketLocation;
	TMap<FName,FVector> LastSocketLocation;
	bool bStartTrace=false;
	ECollisionType CollisionType;
	UPROPERTY(BlueprintReadOnly)
	USkeletalMeshComponent * SkeletalMeshComponent;
	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent *StaticMeshComponent;
	UFUNCTION(BlueprintCallable)
	void SetSkeletalMeshAndStaticMesh(USkeletalMeshComponent *  SkeletalMesh,UStaticMeshComponent * StaticMesh);
	UFUNCTION(BlueprintCallable)
	void SetSkeletalMesh(USkeletalMeshComponent *SkeletalMesh);
	UFUNCTION(BlueprintCallable)
	void SetStaticMesh(UStaticMeshComponent * StaticMesh);
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
private:
	void UpdateSocketLocation();
	void UpdateSkeletalMeshSocketLocation();
	void UpdateStaticMeshSocketLocation();
};
