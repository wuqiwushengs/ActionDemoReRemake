#include "ActionCameraTypes.h"

#include "CameraManagerFold/ActionPlayerCameraManager.h"

AActionPlayerCameraManager* CameraGlobalFunc::TryGetPlayerCameraManager(const AActor* Owner)
{	check(Owner)
	if(!Owner) return nullptr;
	APlayerController * Controller=Cast<APlayerController>(Owner->GetInstigatorController());
	if(Controller)
	{
		if(AActionPlayerCameraManager * APCM=Cast<AActionPlayerCameraManager>(Controller->PlayerCameraManager); APCM)
		{
			return  APCM;
		}
		return  nullptr;
	}
	UE_LOG(LogTemp,Warning,TEXT("Can't find Real CameraManager"))
	return nullptr;
}


