// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "InputFold/InputDataAsset.h"
#include "ActionInputComponent.generated.h"


UCLASS(ClassGroup=(Action), meta=(BlueprintSpawnableComponent))
class ACTIONSKILLASSETRUNTIME_API UActionInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UActionInputComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	//这里创建输入绑定函数
	template<typename UserObject,typename CallBackFunction >
	void BindAbilityByFunction(TArray<FInputData> & InputData,UserObject * Owner,CallBackFunction FunctionName)
	{
		for (FInputData & Data:InputData)
		{
				BindAction(Data.InputAction,ETriggerEvent::Started,Owner,FunctionName,Data.InputTag,ETriggerEvent::Started);
				BindAction(Data.InputAction,ETriggerEvent::Completed,Owner,FunctionName,Data.InputTag,ETriggerEvent::Completed);
		}
	}
};
