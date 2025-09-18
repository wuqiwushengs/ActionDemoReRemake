// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillExecutorTreeViewModel/SkillExecutorEditorData.h"

#include "ActionSkillAssetRuntime/Public/ActionAbilityResourceFold/SkillExecutorFold/SkillExecutorDescriptor.h"

//醒来之后的提醒，这里这个是用在一个总的RootTree上
USkillExecutorConfig & USkillExecutorEditorData::AddSubTree(const FName Name)
{
	USkillExecutorConfig * SkillExecutorConfig=NewObject<USkillExecutorConfig>(this,FName(),RF_Transactional);
	check(SkillExecutorConfig);
	SkillExecutorConfig->SkillDescriptorName=Name;
	SubTrees.Add(SkillExecutorConfig);
	return *SkillExecutorConfig;
	
}