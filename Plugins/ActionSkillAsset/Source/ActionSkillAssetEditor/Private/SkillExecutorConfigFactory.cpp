// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillExecutorConfigFactory.h"

#include "ActionSkillAssetRuntime/Public/ActionAbilityResourceFold/SkillExecutorFold/SkillExecutorDescriptor.h"

USkillExecutorConfigFactory::USkillExecutorConfigFactory(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	SupportedClass=USkillExecutorDescriptorAsset::StaticClass();
	bCreateNew=true;
	bEditAfterNew=true;
}

UObject* USkillExecutorConfigFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName,
                                                       EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	TObjectPtr<USkillExecutorDescriptorAsset> SkillExecutorAsset=NewObject<USkillExecutorDescriptorAsset>(InParent,InClass,InName,Flags|RF_Transactional); 
	return SkillExecutorAsset;
}
