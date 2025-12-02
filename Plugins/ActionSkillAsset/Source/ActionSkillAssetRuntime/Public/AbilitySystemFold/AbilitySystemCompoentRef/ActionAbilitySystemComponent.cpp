// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilitySystemComponent.h"
#include "InputAction.h"
#include "Misc/Guid.h"
#include "ActionAbilityResourceFold/SkillManager.h"
#include "Engine/Canvas.h"
#include "GamePlay/Character/ActionPlayerCharacter.h"
#include "GamePlayTag/GamePlayTags.h"
#include "InputFold/InputDataAsset.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
UActionAbilitySystemComponent::UActionAbilitySystemComponent()
{// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SkillManager=CreateDefaultSubobject<USkillManager>("SkillManager");
	// ...
}

void UActionAbilitySystemComponent::DisplayDebug(class UCanvas* Canvas, const class FDebugDisplayInfo& DebugDisplay,
	float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;
	for(auto var:InputTagsInBuff)
	{
		DisplayDebugManager.DrawString(FString::Printf(TEXT("CurrentFov: %s"),*var.InputTag.ToString()));
	}
	for (auto var:InputDataMap)
	{
		DisplayDebugManager.DrawString(FString::Printf(TEXT("InputTag %s is Press %d"),*var.Key.ToString(),var.Value.InputType==EInputType::Press));
	}
}

// Called when the game starts
void UActionAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//绑定输入执行函数
	InputExecuteDelegate.BindDynamic(this,&UActionAbilitySystemComponent::OnInputFinal);
	OnInputStateChanged.AddUObject(this,&UActionAbilitySystemComponent::OnTurnNormalInputToCheckStillHasPressSkill);
	this->RegisterGameplayTagEvent(GamePlayTags::ExecutePreInput,EGameplayTagEventType::NewOrRemoved).AddUObject(this,&UActionAbilitySystemComponent::OnPreSkillExecute);
	SetCurrentStateTag(GamePlayTags::Ground);
	SkillManager->Initialize(this);
	InputDataMap.Empty();
	
}


// Called every frame
void UActionAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType,ThisTickFunction);
	// 确保组件的 Tick 始终保持启用
	// ...
}

UInputDataAsset* UActionAbilitySystemComponent::GetInputDataAsset()
{
	return  Cast<AActionPlayerCharacter>(GetOwnerActor())->GetInputDataAsset();
}

void UActionAbilitySystemComponent::AbilityInputDataLocalProcessing(const FInputActionInstance& InputInfo,FGameplayTag InputData, UInputDataAsset * InputDataAsset,ETriggerEvent TriggerEvent)
{
	
	switch (TriggerEvent)
	{
	case ETriggerEvent::Started :
		OnAbilityInputStart(InputInfo,InputData,InputDataAsset);
		break;
	case ETriggerEvent::Completed:
			OnAbilityInputEnd(InputInfo,InputData,InputDataAsset);
		break;
	}
}

void UActionAbilitySystemComponent::OnAbilityInputStart(const FInputActionInstance& InputInfo,FGameplayTag InputTag, UInputDataAsset * InputDataAsset)
{
	FInputData InputData=InputDataAsset->GetAbilityInputData(InputTag);
	float WorldTime=InputInfo.GetSourceAction()?InputInfo.GetLastTriggeredWorldTime():GWorld->GetTimeSeconds();
	FSkillTitle NewSkillTitle=FSkillTitle(EInputType::Press,WorldTime,InputTag);
	//清除标记并且创建新的标记
	InputDataMap.FindOrAdd(InputTag)=NewSkillTitle;
	FAbilityInputInfo AbilityInfo=FAbilityInputInfo(InputTag,WorldTime,InputTagsInBuff.Num()<=0?0:WorldTime-InputTagsInBuff[0].InputWorldTime,InputData.InputType);
	if(CheckImmediatelyInputIsAllowed(InputTag))
	{
		InputTagsInBuff.Empty();
		bool IfBound=InputExecuteDelegate.ExecuteIfBound(AbilityInfo);
		SetCurrentInputState(EInputState::DisableInputState);
		return;
	}
	switch (CurrentInputState)
	{
	case  EInputState::PreInputState:
		{
#pragma region PreInputState
			//预输入内容
			if(!CheckPreInputIsAllowed(InputTag)) break;
			InputTagsInBuff.Add(AbilityInfo);
			//检测是否有执行的tag如果有那么就直接执行
			if(this->GetOwnedGameplayTags().HasTag(GamePlayTags::ExecutePreInput))
			{
				FAbilityInputInfo FinalInputInfo;
				if(FindExecuteAbilityInputInfo(InputTagsInBuff,FinalInputInfo))
				{
					bool IfBound=InputExecuteDelegate.ExecuteIfBound(FinalInputInfo);
				
				}
				SetCurrentInputState(EInputState::DisableInputState);
				InputTagsInBuff.Empty();
			}
			break;
		} 
#pragma endregion PreInputState
	case  EInputState::NormalInputState:
		{
#pragma region  NormalInputState
			
			if (CheckInputLengthToSetInputLock(AbilityInfo.InputIntervalTime))
			{
				InputTagsInBuff.Add(AbilityInfo);
			}
			if (InputTagsInBuff.Num()>0&&!GetWorld()->GetTimerManager().IsTimerActive(FinalInputTimer))
			{
				int32 Index=InputTagsInBuff.Num()-1;
				FTimerDelegate FinalExecute;
				FinalExecute.BindLambda([this,InputInfo,InputDataAsset,Index]()
				{
					//这里有个问题需要修改一下index这个数没必要
					SetInputLock(InputInfo,InputDataAsset,InputTagsInBuff,Index);
					CheckFinalInputAndCheckExecute();
					
				});
				GetWorld()->GetTimerManager().SetTimer(FinalInputTimer,FinalExecute,AbilityInputBuffTime,false);
			}
			break;
		}
#pragma endregion
	case  EInputState::DisableInputState:
			//用来在锁定后处理连续打击类型的技能
			//TODO::这里未来可能会出现变成禁止输入导致没有办法
			//用于连击时
		USkillExecutorConfig * SkillConfig=SkillManager->SelectedSkillExecutorConfig;
		if(SkillConfig && SkillConfig->ExecutorDescriptor.Executor && SkillConfig->ExecutorDescriptor.Executor->IsActive)
		{
			if(NewSkillTitle.InputTag!=SkillManager->SelectedSkillExecutorConfig->ExecutorDescriptor.TriggeredTag) return;
			USkillExecutor * Executor=SkillConfig->ExecutorDescriptor.Executor;
			Executor->OnSkillTrigger(NewSkillTitle);
			
		}
			break;
	}
	
}

void UActionAbilitySystemComponent::OnAbilityInputEnd(const FInputActionInstance& InputInfo,FGameplayTag InputTag, UInputDataAsset * InputDataAsset)
{
	//移除对应标记
	USkillExecutorConfig * SkillConfig=SkillManager->SelectedSkillExecutorConfig;
	//寻找对应标记并且更换为释放。
	FSkillTitle  *  Title=InputDataMap.Find(InputTag);
	Title->InputType=EInputType::Release;
	//检查是否激活执行释放
	if(SkillConfig && SkillConfig->ExecutorDescriptor.Executor && SkillConfig->ExecutorDescriptor.Executor->IsActive)
	{
		if(Title->InputTag!=SkillManager->SelectedSkillExecutorConfig->ExecutorDescriptor.TriggeredTag) return;
 		USkillExecutor * Executor=SkillConfig->ExecutorDescriptor.Executor;
		Executor->OnSkillTrigger(*Title);
	}
	//这里更改了一下将所有输入映射不删除只是改成按下或者释放。
}
void UActionAbilitySystemComponent::OnTurnNormalInputToCheckStillHasPressSkill(EInputState OldState ,EInputState NewState)
{
	if(NewState!=EInputState::NormalInputState) return;
	if(InputDataMap.Num()<=0) return;
	//应该还是担心自动的内容
	if(SkillManager->SelectedSkillExecutorConfig&&SkillManager->SelectedSkillExecutorConfig->ExecutorDescriptor.Executor->IsActive) return;
	FInputData FinalInputData;
	for(TPair<FGameplayTag,FSkillTitle> & InputData:InputDataMap)
	{
		if(InputData.Value.InputType==EInputType::Press )
		{
			//找那个权重更大
			if(FinalInputData.IsValid())
			{
				FinalInputData=(GetInputDataAsset()->GetAbilityInputData(InputData.Key).InputType>FinalInputData.InputType&& GetInputDataAsset()->GetAbilityInputData(InputData.Key).AllowedInputActiveAfterInputStateChangeToNormal)?GetInputDataAsset()->GetAbilityInputData(InputData.Key):FinalInputData;
			}
			else
			{
				FinalInputData=GetInputDataAsset()->GetAbilityInputData(InputData.Key).AllowedInputActiveAfterInputStateChangeToNormal?GetInputDataAsset()->GetAbilityInputData(InputData.Key):FinalInputData;
			}
		}
	}
	if(FinalInputData.IsValid())
	{
		OnAbilityInputStart(FInputActionInstance(),FinalInputData.InputTag,GetInputDataAsset());
	}
}
USkillManager* UActionAbilitySystemComponent::GetSkillManager()
{
	return SkillManager;
}

AActionPlayerCharacter* UActionAbilitySystemComponent::GetActionPlayerCharacter()
{
	return Cast<AActionPlayerCharacter>(GetOwnerActor());
}

FCharacterNormalInputData UActionAbilitySystemComponent::GetControlledCharacterNormalInputData()
{
	ICharacterInfoInterface *CII=Cast<ICharacterInfoInterface>(GetActionPlayerCharacter());
	return CII->GetCharacterNormalInputData();
}
void UActionAbilitySystemComponent::SetCurrentStateTag(FGameplayTag NewStateTag)
{
	FGameplayTag LastStateTag=CurrentStateTag;
	CurrentStateTag=NewStateTag;
	OnStateTagChanged.Broadcast(LastStateTag,NewStateTag);
	
}
EInputState UActionAbilitySystemComponent::GetCurrentInputState()
{
	return CurrentInputState;
}
void UActionAbilitySystemComponent::SetCurrentInputState(EInputState NewInputState)
{
 	if (CurrentInputState!=NewInputState)
 	{
 		LastInputState=CurrentInputState;
 		CurrentInputState=NewInputState;
 		OnInputStateChanged.Broadcast(LastInputState,CurrentInputState);
 	}
}

bool UActionAbilitySystemComponent::CheckInputLengthToSetInputLock(float InputLength)
{
	if (InputLength>=AbilityInputBuffTime)
	{
		return  false;
	}
	return true;
}
void UActionAbilitySystemComponent::SetInputLock(const FInputActionInstance& ActionInstance,
	UInputDataAsset* InputDataAsset, TArray<FAbilityInputInfo>& AbilityInputInfos, int32 index)
{	
	SetCurrentInputState(EInputState::DisableInputState);
	FGameplayTag InputTag=AbilityInputInfos[index].InputTag;
	InputLockDelegate.ExecuteIfBound(ActionInstance,InputDataAsset,InputTag);
}
void UActionAbilitySystemComponent::SetInputUnLock()
{
	SetCurrentInputState(EInputState::NormalInputState);
		bool IfBound=InputUnlockDelegate.ExecuteIfBound();
	
}
void UActionAbilitySystemComponent::CheckFinalInputAndCheckExecute()
{
	FAbilityInputInfo FinalInputInfo;
	if(FindExecuteAbilityInputInfo(InputTagsInBuff,FinalInputInfo))
	{
			bool IfBound=InputExecuteDelegate.ExecuteIfBound(FinalInputInfo);
			}
	InputTagsInBuff.Empty();
	GetWorld()->GetTimerManager().ClearTimer(FinalInputTimer);
	
}
bool UActionAbilitySystemComponent::FindExecuteAbilityInputInfo(const TArray<FAbilityInputInfo>& InputTagsBuff,FAbilityInputInfo& OutInputInfo)
{
	//根据权重来进行检查
	int32 ExeInputIndex=INDEX_NONE;
	if(InputTagsInBuff.Num()<=0) return false;
	for (int index=0;FAbilityInputInfo InputInfo:InputTagsInBuff)
	{
		if(ExeInputIndex==INDEX_NONE)
		{
			ExeInputIndex=index;
		}
		if(InputInfo.InputWeight>InputTagsBuff[ExeInputIndex].InputWeight)
		{
			ExeInputIndex=index;
		}
		index++;
	}
	OutInputInfo=InputTagsBuff[ExeInputIndex];
	return true;
	
}
//
void UActionAbilitySystemComponent::SetPreInputDisable(const FGameplayTagContainer& DisableTagContainer)
{
	TArray<FGameplayTag> DisableTags=DisableTagContainer.GetGameplayTagArray();
	for (FGameplayTag Tag:DisableTags)
	{
		if(AllowedPreInputTag.Find(Tag))
		{
			AllowedPreInputTag[Tag]=0;
		}
	}
}

void UActionAbilitySystemComponent::SetPreInputImmediately(const FGameplayTagContainer& Immediately)
{
	TArray<FGameplayTag> ImmediatelyTags=Immediately.GetGameplayTagArray();
	for (FGameplayTag Tag:ImmediatelyTags)
	{
		if(AllowedPreInputTag.Find(Tag))
		{
			AllowedPreInputTag[Tag]=2;
		}
	}
}

bool UActionAbilitySystemComponent::CheckImmediatelyInputIsAllowed(FGameplayTag InputTag)
{
	if (AllowedPreInputTag.Find(InputTag))
	{
		return  AllowedPreInputTag[InputTag]==2;
	}
	return false;
}

void UActionAbilitySystemComponent::TurnPreInputToDefault()
{
	for (TPair<FGameplayTag,int> & Tag : AllowedPreInputTag)
	{
		Tag.Value=1;
	}
}
bool UActionAbilitySystemComponent::CheckPreInputIsAllowed(FGameplayTag InputTag)
{
	if (AllowedPreInputTag.Find(InputTag))
	{
		return  AllowedPreInputTag[InputTag]==1;
	}
	return false;
}
void UActionAbilitySystemComponent::OnPreSkillExecute(FGameplayTag ExeTag, int Count)
{
	if(InputTagsInBuff.Num()<=0) return;
	if(Count>=1 &&ExeTag==GamePlayTags::ExecutePreInput)
	{
		//如果计数>=1说明进入了执行预输入阶段。
		FAbilityInputInfo FinalInputInfo;
		if(FindExecuteAbilityInputInfo(InputTagsInBuff,FinalInputInfo))
		{
			InputExecuteDelegate.ExecuteIfBound(FinalInputInfo);
			
		}
		SetCurrentInputState(EInputState::DisableInputState);
		InputTagsInBuff.Empty();
	}
}

void UActionAbilitySystemComponent::OnInputFinal(const FAbilityInputInfo& InputInfo)
{
	FSkillTitle SkillTitle = *InputDataMap.Find(InputInfo.InputTag);
	// 尝试执行技能
	if (SkillManager->TurnToNextSkillExecutor(ESkillReleaseType::Manual, SkillTitle)) return;
	InputTagsInBuff.RemoveAll([&](const FAbilityInputInfo& inInputInfo)
	{return inInputInfo.InputTag == InputInfo.InputTag;});
	while (InputTagsInBuff.Num() > 0)
	{
		FAbilityInputInfo HighestWeightInputInfo;
		if (!FindExecuteAbilityInputInfo(InputTagsInBuff, HighestWeightInputInfo))
		{
			break; // 如果找不到权重最高的，直接退出
		}
		FSkillTitle HighestSkillTitle=*InputDataMap.Find(HighestWeightInputInfo.InputTag);
		// 如果执行失败，从缓冲区中移除当前权重最高的输入
		if (SkillManager->TurnToNextSkillExecutor(ESkillReleaseType::Manual,HighestSkillTitle)) return;
		InputTagsInBuff.RemoveAll([&](const FAbilityInputInfo& inInputInfo)
		{
			return inInputInfo.InputTag == HighestWeightInputInfo.InputTag;
		});
		
	}
	SetCurrentInputState(EInputState::NormalInputState);
}




