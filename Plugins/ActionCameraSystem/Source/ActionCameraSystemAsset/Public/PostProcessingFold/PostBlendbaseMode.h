#pragma once 
#include "CoreMinimal.h"
#include "CameraDataFold/BlendData.h"
#include "PostBlendbaseMode.generated.h"


struct FPostBlendableInfo;
enum class ECameraMontageBlendType : uint8;

UCLASS(Blueprintable)
class ACTIONCAMERASYSTEMASSET_API UPostBlendbaseMode: public UObject
{
public:
	GENERATED_BODY()
	UPostBlendbaseMode();
	void  UpdatePostBlendable(float DeltaTime);
	void DisplayDebug(UCanvas * Canvas);
	ECameraMontageBlendType GetPostProcessBlendType() { return PostProcessBlendType;}
	FGuid GetID() { return  id;}
	TArray<FWeightedBlendable> GetFinalWeightedBlendable();
	void OnActive();
	void DeActive();
	void SetBlendType(ECameraMontageBlendType BlendType);
private:
	void UpdateBlendInfo(float DeltaTime);
	UPROPERTY(EditDefaultsOnly)
	TArray<FPostBlendableInfo> PostBlendObject;
	UPROPERTY(EditDefaultsOnly)
	FBlendData BlendData;
	UPROPERTY(EditDefaultsOnly)
	bool bLoop=false;
	UPROPERTY(EditDefaultsOnly)
	float Duration=0.0f;
	float CurrentAlpha=0.0f;
	float CurrentWeight=0.0f;
	float PlayedTime=0.0f;
	FGuid id;
	ECameraMontageBlendType PostProcessBlendType;
};
