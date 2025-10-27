// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraComponentFold/ActionCineCameraComponent.h"

UActionCineCameraComponent::UActionCineCameraComponent():UCineCameraComponent()
{
}

ECameraForm UActionCineCameraComponent::GetCameraForm_Implementation()
{
	return CameraForm;
}
void UActionCineCameraComponent::DrawDebug(UCanvas* Canvas)
{
}
