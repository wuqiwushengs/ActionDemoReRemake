#pragma once
#include "CoreMinimal.h"
#include "ActionUICameraActor.generated.h"
class UActionUiCameraComponent;

UCLASS()
class ACTIONCAMERASYSTEMASSET_API AActionUICameraActor:public AActor
{
	GENERATED_BODY()
public:
	AActionUICameraActor();
	UPROPERTY()
	UActionUiCameraComponent * UiCameraComponent;
};
