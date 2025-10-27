#include "ActionUICameraActor.h"

#include "CameraComponentFold/UActionUiCameraComponent.h"

AActionUICameraActor::AActionUICameraActor()
{

	UiCameraComponent=CreateDefaultSubobject<UActionUiCameraComponent>("UiCameraComponent");
}
