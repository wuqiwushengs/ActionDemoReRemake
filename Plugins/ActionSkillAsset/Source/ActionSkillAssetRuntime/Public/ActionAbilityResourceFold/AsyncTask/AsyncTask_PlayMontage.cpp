// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncTask_PlayMontage.h"

UAsyncTask_PlayMontage* UAsyncTask_PlayMontage::ActionCreateNewMontage(USkeletalMeshComponent * SkeletalMeshComponent, UAnimMontage* MontageToPlay, float Rate,
	FName StartSection, float StartTimeSeconds, bool bAllowInterruptAfterBlendOut)
{
	UAsyncTask_PlayMontage * MyObj=NewObject<UAsyncTask_PlayMontage>();
	MyObj->Mesh=SkeletalMeshComponent;
	MyObj->Montage = MontageToPlay;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->StartTimeSeconds=StartTimeSeconds;
	MyObj->bAllowInterruptAfterBlendOut=bAllowInterruptAfterBlendOut;
	return MyObj;
}

void UAsyncTask_PlayMontage::Activate()
{
	if (Mesh==nullptr)
	{
		return;
	}
	MontageEnded.BindUObject(this,&UAsyncTask_PlayMontage::OnMontageEnded);
	MontageBlendOut.BindUObject(this,&UAsyncTask_PlayMontage::OnMontageBlendingOut);
	if (UAnimInstance * AnimInstance=Mesh->GetAnimInstance();AnimInstance)
	{
		AnimInstance->Montage_Play(Montage,Rate,EMontagePlayReturnType::MontageLength,StartTimeSeconds);
		AnimInstance->Montage_JumpToSection(StartSection,Montage);
		AnimInstance->Montage_SetEndDelegate(MontageEnded,Montage);
		AnimInstance->Montage_SetBlendingOutDelegate(MontageBlendOut,Montage);
		Super::Activate();
	}
	else
	{
		SetReadyToDestroy();
	}
	
}

void UAsyncTask_PlayMontage::OnMontageEnded(UAnimMontage* PlayedMontage, bool binterrupt)
{
	if (binterrupt&&bAllowInterruptAfterBlendOut)
	{
		OnMontageInterrupt.Broadcast();
		SetReadyToDestroy();
	}
	OnMontageComplete.Broadcast();
	

}

void UAsyncTask_PlayMontage::OnMontageBlendingOut(UAnimMontage* PlayedMontage, bool binterrupt)
{
	if (binterrupt&&!bAllowInterruptAfterBlendOut)
	{
		OnMontageInterrupt.Broadcast();
	}
	OnMontageBlendOut.Broadcast();
}
