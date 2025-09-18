// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGameMode.h"

#include "GamePlay/ActionPlayController.h"
#include "GamePlay/Character/ActionPlayerCharacter.h"

AActionGameMode::AActionGameMode()
{
	DefaultPawnClass=AActionPlayerCharacter::StaticClass();
	PlayerControllerClass=AActionPlayController::StaticClass();
}
