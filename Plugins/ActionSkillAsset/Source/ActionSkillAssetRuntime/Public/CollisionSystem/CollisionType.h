#pragma once
#include "CoreMinimal.h"
#include "Components/BillboardComponent.h"
#include "Engine/TargetPoint.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CollisionType.generated.h"


class ATargetPoint;

UENUM(BlueprintType)
enum class ECollisionShapeInfo:uint8
{
	Line,
	Box,
	Sphere,
	Capsule
};
USTRUCT(BlueprintType)
struct FCollisionShapeInfo
{
	GENERATED_BODY()
	/**
	 * 
	 */
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	ECollisionShapeInfo CollisionShape=ECollisionShapeInfo::Box;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(EditCondition="CollisionShape== ECollisionShapeInfo::Box",EditConditionHides))
	FVector3f BoxCollisionInfo=FVector3f::ZeroVector;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(EditCondition="CollisionShape== ECollisionShapeInfo::Sphere",EditConditionHides))
	float SphereRadiusInfo=0.0f;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(EditCondition="CollisionShape== ECollisionShapeInfo::Capsule",EditConditionHides))
	FVector2D CapsuleInfo=FVector2D::ZeroVector;
#if WITH_EDITOR
	void SetCollisionShape();
#endif
	FCollisionShape GetCollisionShape();
private:
	//最后选定的的CollisionShape
	FCollisionShape SelectedCollisionShape;
	
};
UENUM(BlueprintType)
enum class  ECollisionType:uint8
{
	SkeletalMesh,
	StaticMesh,
	GlobalCollision
};
//这个用在动画通知当中，用于设置瞬时碰撞检测的内容
USTRUCT(BlueprintType)
struct FGlobalCollisionPoint
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FVector StartPoint=FVector::ZeroVector;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FVector EndPoint=FVector::ZeroVector;
};
USTRUCT(BlueprintType)
struct FGlobalCollisionTargetPoint
{
	GENERATED_BODY()
	UPROPERTY(Transient)
	ATargetPoint * StartTargetPoint;
	UPROPERTY(Transient)
	ATargetPoint * EndTargetPoint;
};
USTRUCT(BlueprintType)
struct FCollisionInfoSum
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FCollisionShapeInfo  ShapeInfo;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel>  Channel;
	//专门用在瞬时内容当中
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGlobalCollisionPoint CollisionPoints;
	//专门用在瞬时内容当中
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UTexture2D * Icon;
	
#if WITH_EDITORONLY_DATA
	FGlobalCollisionTargetPoint CollisionTargetPoint;
#endif
#if WITH_EDITOR
	void SetTargetPoint()
	{
		if(CollisionTargetPoint.StartTargetPoint)
		{
			CollisionTargetPoint.StartTargetPoint->SetActorLocation(CollisionPoints.StartPoint);
		}
		if(CollisionTargetPoint.EndTargetPoint)
		{
			CollisionTargetPoint.EndTargetPoint->SetActorLocation(CollisionPoints.EndPoint);
		}
	}
	FGlobalCollisionTargetPoint RefreshTargetPointAndSave(UWorld * ViewWorld)
	{
		DeleteGlobalCollisionTargetPoint();
		if(ViewWorld)
		{	//start
			AActor * Start= ViewWorld->SpawnActor(ATargetPoint::StaticClass(),&CollisionPoints.StartPoint);
			CollisionTargetPoint.StartTargetPoint=Cast<ATargetPoint>(Start);
			if(Icon)
			{
				CollisionTargetPoint.StartTargetPoint->GetSpriteComponent()->SetSprite(Icon);
			}
			//End
			AActor * End= ViewWorld->SpawnActor(ATargetPoint::StaticClass(),&CollisionPoints.EndPoint);
			CollisionTargetPoint.EndTargetPoint=Cast<ATargetPoint>(End);
			if(Icon)
			{
			CollisionTargetPoint.EndTargetPoint->GetSpriteComponent()->SetSprite(Icon);
			}
		}
		return CollisionTargetPoint;
	};
	void DeleteGlobalCollisionTargetPoint()
	{
			if(CollisionTargetPoint.StartTargetPoint)
			{
				CollisionTargetPoint.StartTargetPoint->Destroy();
			}
			if (CollisionTargetPoint.EndTargetPoint)
			{
				CollisionTargetPoint.EndTargetPoint->Destroy();
			}
	}
#endif
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bReturnPhysicalMaterial;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool  bTraceComplex;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bIgnoreSelf;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bMultiTrace;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FLinearColor LinearColor;

#pragma region DebugInfo
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TEnumAsByte<EDrawDebugTrace::Type> DebugType;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float DebugTime;
#pragma endregion
	FCollisionQueryParams GetCollisionQueryParams()
	{
		return CollisionQueryParams;
	};
	FCollisionShape GetCollisionShape()
	{
		return ShapeInfo.GetCollisionShape();
	}
	void SetCollisionParameters()
	{
		CollisionQueryParams.bTraceComplex=bTraceComplex;
		CollisionQueryParams.bReturnPhysicalMaterial=bReturnPhysicalMaterial;
		
	}
private:
	FCollisionQueryParams  CollisionQueryParams;
};

USTRUCT(BlueprintType)
struct FCollisionContext
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	ECollisionType CollisionType=ECollisionType::GlobalCollision;
	//这两个用在连续的碰撞检测上
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(EditCondition="CollisionType==ECollisionType::SkeletalMesh",EditConditionHides))
	TMap<FName,FCollisionInfoSum> BoneCollisionInfo;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(EditCondition="CollisionType==ECollisionType::StaticMesh",EditConditionHides))
	TMap<FName,FCollisionInfoSum> StaticMeshSocketCollisionInfo;
	//用在静态网格检测当中
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(EditCondition="CollisionType==ECollisionType::StaticMesh",EditConditionHides))
	UStaticMesh * SocketStaticMesh;
	//这个用在瞬时的碰撞检测上
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(EditCondition="CollisionType==ECollisionType::GlobalCollision",EditConditionHides))
	TMap<FName,FCollisionInfoSum> GlobalCollision;
	//这个用在瞬时的碰撞检测上
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UWorld * PreviewWorld;
	void SetPreviewWorld(UWorld * ViewWorld)
	{	
	 	PreviewWorld=ViewWorld;
	}
	void SetCollisionInfo()
	{
		switch (CollisionType)
		{
		case ECollisionType::SkeletalMesh :
			SetCollisionQueryAndCollisionShape(BoneCollisionInfo);
			break;
		case ECollisionType::StaticMesh :
			SetCollisionQueryAndCollisionShape(StaticMeshSocketCollisionInfo);
			break;
		case ECollisionType::GlobalCollision:
			SetCollisionQueryAndCollisionShape(GlobalCollision);
			break;
		}
	}
	UPROPERTY()
	TArray<FGlobalCollisionTargetPoint> Points;
	void  RefreshTargetPointAndSaveAll()
	{
		for (FGlobalCollisionTargetPoint & Point: Points)
		{
			if( Point.StartTargetPoint)
			{
				 Point.StartTargetPoint->Destroy();
			}
			if ( Point.EndTargetPoint)
			{
				 Point.EndTargetPoint->Destroy();
			}
		}
		Points.Empty();
		for (TPair<FName,FCollisionInfoSum> & GlobalInfo:GlobalCollision)
		{
			
			Points.Add(GlobalInfo.Value.RefreshTargetPointAndSave(PreviewWorld));
		}
	}
	void  RefreshTargetIcon()
	{
		for (TPair<FName,FCollisionInfoSum> & GlobalInfo:GlobalCollision)
		{
			if(GlobalInfo.Value.CollisionTargetPoint.StartTargetPoint)
			{
				GlobalInfo.Value.CollisionTargetPoint.StartTargetPoint->GetSpriteComponent()->SetSprite(GlobalInfo.Value.Icon);
			}
			if(GlobalInfo.Value.CollisionTargetPoint.EndTargetPoint)
			{
				GlobalInfo.Value.CollisionTargetPoint.EndTargetPoint->GetSpriteComponent()->SetSprite(GlobalInfo.Value.Icon);
			}
		}
	};
#endif
	static TArray<FHitResult> GlobalTraceChannel(AActor * Tracer,FName SocketName,FCollisionInfoSum CollisionInfoSum,FVector Rootoffset,FRotator Rotationoffset);
	
	static TArray<FHitResult> SkeletalMeshTraceChannel(AActor * Tracer,FName SocketName,FCollisionInfoSum CollisionInfoSum);
	
	static TArray<FHitResult> StaticMeshTraceChannel(AActor * Tracer,FName SocketName,FCollisionInfoSum CollisionInfoSum);
	TMap<FName,FCollisionInfoSum> GetCollisionInfo()
	{
		switch (CollisionType)
		{
		case ECollisionType::SkeletalMesh :
			return BoneCollisionInfo;
		case ECollisionType::StaticMesh:
			return StaticMeshSocketCollisionInfo;
		case ECollisionType::GlobalCollision:
			return GlobalCollision;
		}
		return TMap<FName,FCollisionInfoSum>();
	}
#if WITH_EDITOR
	static void DebugTraceSweep(AActor * Tracer, const TArray<FHitResult> & HitResults,const FCollisionInfoSum & CollisionInfoSum,const FVector & Start, const FVector & End,bool bHit,FLinearColor DebugColor=FLinearColor::Red,FRotator RotationOffset=FRotator());
	static void DebugTraceSweepSingle(AActor * Tracer,FHitResult & HitResults, const FCollisionInfoSum& CollisionInfoSum,const FVector & Start, const FVector & End,bool bHit,FLinearColor DebugColor=FLinearColor::Red,FRotator RotationOffset=FRotator());
#endif
private:
#if WITH_EDITOR
	void SetCollisionQueryAndCollisionShape(TMap<FName,FCollisionInfoSum> CollisionInfos)
	{
		for(TPair<FName,FCollisionInfoSum> & CollisionInfo: CollisionInfos)
		{	//设置所有的内容
			CollisionInfo.Value.SetCollisionParameters();
			CollisionInfo.Value.ShapeInfo.SetCollisionShape();
		}
	}
#endif
	
};
