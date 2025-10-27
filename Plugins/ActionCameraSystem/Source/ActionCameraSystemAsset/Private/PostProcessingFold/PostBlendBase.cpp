// Fill out your copyright notice in the Description page of Project Settings.


#include "PostProcessingFold/PostBlendBase.h"


void UPostBlendBase::ChangeBlendableSettings(FSceneView& View, float Weight) const
{
	/*View.FinalPostProcessSettings->*/
}

void UPostBlendBase::OverrideBlendableSettings(class FSceneView& View, float Weight) const
{
	ChangeBlendableSettings(View,Weight);
}
