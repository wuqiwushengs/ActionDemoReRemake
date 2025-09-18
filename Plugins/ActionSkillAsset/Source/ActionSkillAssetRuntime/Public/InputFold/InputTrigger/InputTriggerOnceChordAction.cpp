// Fill out your copyright notice in the Description page of Project Settings.


#include "InputTriggerOnceChordAction.h"

#include "EnhancedPlayerInput.h"
#include "InputAction.h"

ETriggerState UInputTriggerOnceChordAction::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput,
                                                                     FInputActionValue ModifiedValue, float DeltaTime)
{
	// Inherit state from the chorded action
	const FInputActionInstance* PreEventData = PlayerInput->FindActionInstanceData(PreInputAction);
	const FInputActionInstance* NextEventData = PlayerInput->FindActionInstanceData(PostInputAction);
	//没有没激活并且在前面是进行一次是否激活的判断
	if (!bTriggered&&CheckPreInputIsOnGoing(PreEventData) && NextEventData->GetTriggerEvent() == ETriggerEvent::None)
	{
		//这个标志着前一个输入必须执行一段时间才会激活下一个输入
		if (PreEventData->GetTriggeredTime()>NextEventData->GetTriggeredTime()+TriggerTimeOffset)
		{
			bTriggered=true;
			return ETriggerState::Triggered;
		}
	}
	//随后锁住不允许判断，只有当没有数值时才允许下一次判断，其他时候都为None
	bTriggered=true;
	if (!IsActuated(ModifiedValue))
	{
		bTriggered=false;
	}
	return ETriggerState::None;
}

bool UInputTriggerOnceChordAction::CheckPreInputIsOnGoing(const FInputActionInstance * PreEventData)
{
	switch (PreEventData->GetTriggerEvent())
	{
	case ETriggerEvent::None: return false;
		default: return true;
	}
}
