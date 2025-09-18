// Fill out your copyright notice in the Description page of Project Settings.


#include "InputDataAsset.h"

UInputAction* UInputDataAsset::GetNativeInputActionByTag(FGameplayTag InputTag)
{
	if (InputTag.IsValid()&&!NativeInputData.IsEmpty())
	{
		for (FInputData & Data: NativeInputData)
		{
			if (Data.InputTag==InputTag)
			{
			
				return Data.InputAction ;
			}
		}
	}
	UE_LOG(LogTemp,Warning,TEXT("Can't Find Native InputAction"))
	return nullptr;; 
}
TArray<FInputData> UInputDataAsset::GetNativeInputActions()
{
	return NativeInputData;
}
FInputData& UInputDataAsset::GetNativeInputData(FGameplayTag InputTag)
{
	for (FInputData &  Data: NativeInputData)
	{
		if (Data.InputTag!=InputTag) continue;
		return  Data;
	}
	UE_LOG(LogTemp,Warning,TEXT("Can't Find Conrrect AbilityInputdata"));
	return   NativeInputData[0];
}
UInputAction* UInputDataAsset::GetAbilityInputActionByTag(FGameplayTag InputTag)
{
	if (!AbilityInputData.IsEmpty()&&InputTag.IsValid())
	{
		for (FInputData & Data: NativeInputData)
		{
			if (Data.InputTag==InputTag) 
			{
				return  Data.InputAction;
			}
		}
	}
	UE_LOG(LogTemp,Warning,TEXT("Can't Find Native InputAction"))
	return nullptr;; 
}
TArray<FInputData> UInputDataAsset::GetAbilityInputActions()
{
	return  AbilityInputData;
}
FInputData& UInputDataAsset::GetAbilityInputData(FGameplayTag InputTag)  
{
	for (FInputData &  Data: AbilityInputData)
	{
		if (Data.InputTag!=InputTag) continue;
		return  Data;
	}

	UE_LOG(LogTemp,Warning,TEXT("Can't Find Conrrect AbilityInputdata"));
	return AbilityInputData[0];
}
