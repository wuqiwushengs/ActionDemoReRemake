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
	UPROPERTY(BlueprintReadWrite)
	float DamageAmount=0.0f;
};

struct FBaseMeshInfo
{
	TSet<FName> SocketNames;
	TMap<FName,FVector> CurrentSocketLocation;
	TMap<FName,FVector> LastSocketLocation;
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
	FVector GetLastUpdateSocketLocation(const FName & SocketName,ECollisionType CollisionType);
	virtual FVector GetLastUpdateSocketLocation_Implementation( const FName & SocketName,ECollisionType CollisionType);
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	FVector GetCurrentUpdateSocketLocation(const FName & SocketName,ECollisionType CollisionType);
	virtual FVector GetCurrentUpdateSocketLocation_Implementation(const FName & SocketName,ECollisionType CollisionType);
	void  StartTrace(const TArray<FName> & TraceFollowName,ECollisionType CollisionCategory);
	void StartTrace(const TArray<FName> & TraceFollowName,ECollisionType CollisionCategory,UStaticMesh * AttachStaticMesh);
	void EndTrace(ECollisionType CollisionType,const TArray<FName> & TraceFollowName);
	void EndTrace(const TArray<FName> & TraceFollowName,ECollisionType CollisionCategory,UStaticMesh * AttachStaticMesh);
	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent * GetSkeletalMeshComponent();
	TArray<TObjectPtr<UStaticMeshComponent>> GetStaticMeshComponent();
	TArray<FName> GetSocketNames(ECollisionType CollisionType);
	 virtual  void OnAttacktoTagretRecall(TArray<FHitResult> HitResults);
	virtual  void OnBeAttackedRecall(FAttackedResult AttackedResult);
protected:
	//这个函数用来处理被打击的情况，通常会被OnAttacktoTargetRecall所调用
	
	// Called when the game starts
	virtual void BeginPlay() override;
	TMap<ECollisionType,FBaseMeshInfo> TraceInfo;
	int TraceNum;
	UPROPERTY(BlueprintReadOnly)
	 USkeletalMeshComponent * SkeletalMeshComponent;
	TMap<TObjectPtr<UStaticMeshComponent> ,TSet<FName>>StaticMeshComponentArray;
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
