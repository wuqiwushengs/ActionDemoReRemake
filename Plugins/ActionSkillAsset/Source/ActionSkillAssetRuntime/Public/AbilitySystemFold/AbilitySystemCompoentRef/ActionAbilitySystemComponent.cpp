// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilitySystemComponent.h"
#include "InputAction.h"
#include "Misc/Guid.h"
#include "ActionAbilityResourceFold/SkillManager.h"
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
// Called when the game starts
void UActionAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("UActionAbilitySystemComponent instance: %p"), this);
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
		/*FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle,
			[this,InputInfo,InputData,InputDataAsset]()
			{
				OnAbilityInputEnd(InputInfo,InputData,InputDataAsset);
			},0.1f,false);*/
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
	UE_LOG(LogTemp,Warning,TEXT("Start InputTag %s"),*InputTag.ToString())
	switch (CurrentInputState)
	{
	case  EInputState::PreInputState:
		{
#pragma region PreInputState
			UE_LOG(LogTemp,Warning,TEXT("PreInput"))
			if(!CheckIsAllowed(InputTag)) break;
			FAbilityInputInfo AbilityInfo=FAbilityInputInfo(InputTag,WorldTime,InputTagsInBuff.Num()<=0?0:WorldTime-InputTagsInBuff[0].InputWorldTime,InputData.InputType);
			InputTagsInBuff.Add(AbilityInfo);
			if(this->GetOwnedGameplayTags().HasTag(GamePlayTags::ExecutePreInput))
			{
				FAbilityInputInfo FinalInputInfo;
				if(FindExecuteAbilityInputInfo(InputTagsInBuff,FinalInputInfo))
				{
					bool IfBound=InputExecuteDelegate.ExecuteIfBound(FinalInputInfo);
					UE_LOG(LogTemp,Warning,TEXT("InputExecuteDelegate   Bind= %s"),IfBound ? TEXT("true") : TEXT("false"))
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
			UE_LOG(LogTemp,Warning,TEXT("NormalInput"))
			FAbilityInputInfo AbilityInputInfo=FAbilityInputInfo(InputTag,WorldTime,InputTagsInBuff.Num()<=0?0:WorldTime-InputTagsInBuff[0].InputWorldTime,InputData.InputType);
			if (CheckInputLengthToSetInputLock(AbilityInputInfo.InputIntervalTime))
			{
				InputTagsInBuff.Add(AbilityInputInfo);
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
				UE_LOG(LogTemp,Warning,TEXT("DisableInput"))
		USkillExecutorConfig * SkillConfig=SkillManager->SelectedSkillExecutorConfig;
		if(SkillConfig && SkillConfig->ExecutorDescriptor.Executor && SkillConfig->ExecutorDescriptor.Executor->IsActive)
		{
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
		USkillExecutor * Executor=SkillConfig->ExecutorDescriptor.Executor;
		Executor->OnSkillTrigger(*Title);
	}
	//这里更改了一下将所有输入映射不删除只是改成按下或者释放。
}
void UActionAbilitySystemComponent::OnTurnNormalInputToCheckStillHasPressSkill(EInputState OldState ,EInputState NewState)
{
	if(NewState!=EInputState::NormalInputState) return;
	if(InputDataMap.Num()<=0) return;

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
	UE_LOG(LogTemp,Warning,TEXT("NewInputState is Equal With NewInputState"));
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
		UE_LOG(LogTemp,Warning,TEXT("InputExecuteDelegate   Bind= %s"),IfBound ? TEXT("true") : TEXT("false"))
}
void UActionAbilitySystemComponent::CheckFinalInputAndCheckExecute()
{
	FAbilityInputInfo FinalInputInfo;
	if(FindExecuteAbilityInputInfo(InputTagsInBuff,FinalInputInfo))
	{
			bool IfBound=InputExecuteDelegate.ExecuteIfBound(FinalInputInfo);
			UE_LOG(LogTemp,Warning,TEXT("InputExecuteDelegate   Bind= %s"),IfBound ? TEXT("true") : TEXT("false"))
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
void UActionAbilitySystemComponent::TurnPreInputToDefault()
{
	for (TPair<FGameplayTag,int> & Tag : AllowedPreInputTag)
	{
		Tag.Value=1;
	}
}
bool UActionAbilitySystemComponent::CheckIsAllowed(FGameplayTag InputTag)
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
			UE_LOG(LogTemp,Warning,TEXT("PreSkillExecute"))
		}
		SetCurrentInputState(EInputState::DisableInputState);
		InputTagsInBuff.Empty();
	}
}

void UActionAbilitySystemComponent::OnInputFinal(const FAbilityInputInfo& InputInfo)
{
	UE_LOG(LogTemp,Warning,TEXT("InputFinalExecute"))
	FSkillTitle SkillTitle=*InputDataMap.Find(InputInfo.InputTag);
	//这里因为断点可能出现执行的问题因此使用缓存Map,但是可能会在Complete哪里出现一些问题。
	SkillManager->TurnToNextSkillExecutor(ESkillReleaseType::Manual,SkillTitle);
}




