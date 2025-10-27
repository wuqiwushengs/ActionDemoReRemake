// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraComponentFold/ActionCameraComponent.h"

UActionCameraComponent::UActionCameraComponent():UCameraComponent()
{
	
}

ECameraForm UActionCameraComponent::GetCameraForm_Implementation()
{
	return CameraForm;
}
void UActionCameraComponent::DrawDebug(UCanvas* Canvas)
{
	
}
