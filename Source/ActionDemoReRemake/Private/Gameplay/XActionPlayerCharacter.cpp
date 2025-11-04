// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionDemoReRemake/Public/Gameplay/XActionPlayerCharacter.h"
#include "CameraManagerFold/ActionPlayerCameraManager.h"
#include "GamePlay/ActionPlayController.h"

// Sets default values
AXActionPlayerCharacter::AXActionPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AXActionPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	AActionPlayerCameraManager* CameraManager=	Cast<AActionPlayerCameraManager>(PlayController->PlayerCameraManager);
	if(CameraManager)
	{
		CameraManager->CameraModeBindSingleDelegate.BindUObject(this, &AXActionPlayerCharacter::GetSelectedCameraMode);
	}
}
TSubclassOf<UActionCameraMode> AXActionPlayerCharacter::GetSelectedCameraMode()
{
	if(AimCamera)
	{
		if(AimCameraMode)
		{
			return AimCameraMode;
		}
		return nullptr;
	}
	else
	{

		if(TestCameraMode)
		{
			return TestCameraMode;
		}
		return nullptr;
	}
	
}


// Called every frame
void AXActionPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AXActionPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

