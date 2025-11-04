#pragma once
#include "CoreMinimal.h"
#include "CameraDataFold/BlendData.h"
#include  "PostBlendStack.generated.h"
struct FBlendCurveInfo;
class UPostBlendbaseMode;



UCLASS()
class ACTIONCAMERASYSTEMASSET_API UPostBlendStack:public UObject
{
public:
	GENERATED_BODY()
	void DisplayDebug(UCanvas *Canvas);
	//专门用于处理后处理的内容
	void StartPostProcessingChange(FPostProcessSettings InPostProcessSettings);
	void AddBlendMode(TSubclassOf<UPostBlendbaseMode> BlendMode);
	void UpdateBlendInfo(float DeltaTime);
	void UpdatePostProcessObject(float DeltaTime);
	FPostProcessSettings GetFinalPostProcessSettings() { return  FinalPostProcessSettings;}
private:
	UPROPERTY()
	TArray<UPostBlendbaseMode *> BlendbaseModesStack;
	UPROPERTY()
	TArray<UPostBlendbaseMode*> BlendbaseModesInstance;
	
	UPROPERTY()
	FPostProcessSettings FinalPostProcessSettings;
	void UpdateBlendStack(float DeltaTime);
	void UpdateBlendMode(float DeltaTime);
};
