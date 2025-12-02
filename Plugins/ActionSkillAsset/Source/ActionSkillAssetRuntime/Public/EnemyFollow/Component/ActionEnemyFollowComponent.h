// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionEnemyFollowComponent.generated.h"


class UEnvQuery;
class AActionPlayerCharacter;

UENUM()
enum class EDistanceDetect:uint8
{
	Near,
	Medium,
	Far,
};

UENUM()
enum  class EHeightDetect:uint8
{
	Lower,
	Normal,
	Heigher,
};

USTRUCT()
struct FLocationDetect
{
	GENERATED_BODY()
	EDistanceDetect DistanceDetect;
	EHeightDetect HeightDetect;
};
inline uint32 GetTypeHash(const FLocationDetect& Key)
{
	return HashCombine(GetTypeHash(static_cast<uint8>(Key.DistanceDetect)), GetTypeHash(static_cast<uint8>(Key.HeightDetect)));
}
USTRUCT(BlueprintType)
struct FDistanceDetectInfo
{
	GENERATED_BODY()
	//最近位置的范围
	UPROPERTY(EditDefaultsOnly)
	float NearRange=0.0f;
	//中间位置的范围
	UPROPERTY(EditDefaultsOnly)
	float MediumRange=0.0f;
	//最远位置的范围
	UPROPERTY(EditDefaultsOnly)
	float FarRange=0.0f;
};
USTRUCT()
struct FHeightDetectInfo
{
	GENERATED_BODY()
	//定义更低位置的偏差
	UPROPERTY(EditDefaultsOnly)
	float LowerOffset=0.0f;
	//定义更高位置的偏差
	UPROPERTY(EditDefaultsOnly)
	float HeigherOffset=0.0f;
};
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONSKILLASSETRUNTIME_API UActionEnemyFollowComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UActionEnemyFollowComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
//所需的Actor 信息
	UPROPERTY()
	TObjectPtr<AActionPlayerCharacter> ControlledCharacter=nullptr;
	UPROPERTY()
	AActor * CatchActor=nullptr;
	UFUNCTION(BlueprintCallable,BlueprintPure)
	AActor * GetCatchActor() {return CatchActor;};
	
	//距离和高度设置默认信息
	UPROPERTY(EditDefaultsOnly,Category="DetectInfo")
	FDistanceDetectInfo DefaultDistanceDetectInfo;
	UPROPERTY(EditDefaultsOnly,Category="DetectInfo")
	FHeightDetectInfo DefaultHeightDetectInfo;
	UPROPERTY()
	TMap<FLocationDetect,AActor*> TraceActors;
	//当前内容
	TMap<EDistanceDetect,TArray<AActor*>> EnemySelect;
	UPROPERTY()
	TArray<AActor*> AllActor;
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ETraceTypeQuery> SphereTraceType;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACharacter> EnemySelectedClass;
	UPROPERTY(EditDefaultsOnly)
	bool DrawDebug=false;
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> SphereTraceToFindEnemy(float Radiaus,float IgnoreDistance);
	UFUNCTION()
	void TraceToInitEnemy();
	//获取被捕捉actor的位置
	UFUNCTION(BlueprintCallable)
	FVector GetCatchActorLocation();
	UFUNCTION(BlueprintCallable)
	bool SelectedFinalActor();
	UFUNCTION(BlueprintCallable)
	void ClearAttachedActor();
	UFUNCTION(BlueprintCallable)
	void SetCaptureActor(AActor * CaptureActor);
	UFUNCTION(BlueprintCallable)
	bool  CheckEnemyInRange(EDistanceDetect Distance,TMap<AActor*,FRotator> & EnemyInRangeInfo);
};
