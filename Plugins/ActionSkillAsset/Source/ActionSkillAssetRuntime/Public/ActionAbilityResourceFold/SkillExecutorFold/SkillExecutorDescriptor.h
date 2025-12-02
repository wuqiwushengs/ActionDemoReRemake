#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ActionAbilityResourceFold/SkillTypes/SkillTypes.h"
#include "SkillExecutorDescriptor.generated.h"


class UExecutorCondition;
enum class ESkillWeight : uint8;
class USkillExecutorEditorData;
class USkillExecutor;

UENUM(BlueprintType)
enum class  ESkillReleaseType:uint8
{
	//自动执行无论当前是什么什么情况都会直接执行
	Auto,
	//手动执行
	Manual,
};

USTRUCT(BlueprintType)
struct FSkillExecutorDescriptor
{
	GENERATED_BODY()
	FSkillExecutorDescriptor();
	//技能执行者
	UPROPERTY(EditDefaultsOnly,Instanced)
	TObjectPtr<USkillExecutor> Executor;
	UPROPERTY(EditDefaultsOnly,Instanced)
	TObjectPtr<UExecutorCondition> Condition;
	//激活的Tag
	UPROPERTY(EditDefaultsOnly,meta=(EditCondition=" ReleaseType==ESkillReleaseType::Manual",EditConditionHides))
	FGameplayTag TriggeredTag;
	//需要的Tag
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer OwnerRequestTag;
	UPROPERTY(EditDefaultsOnly)
	ESkillReleaseType ReleaseType=ESkillReleaseType::Manual;
	UPROPERTY(EditDefaultsOnly)
	ESkillWeight SkillWeight=ESkillWeight::Weight1;
	bool CanExecute(FGameplayTagContainer ConditionTags) const
	{
		 return ConditionTags.HasAllExact(OwnerRequestTag);
	};
};

UCLASS(Blueprintable,EditInlineNew)
class ACTIONSKILLASSETRUNTIME_API  USkillExecutorConfig:public UObject
{
	GENERATED_BODY()
public:
	USkillExecutorConfig()
		:ID(FGuid::NewGuid())
	{};
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FSkillExecutorDescriptor ExecutorDescriptor;
	UPROPERTY()
	TArray<TObjectPtr<USkillExecutorConfig>> Children;
	UPROPERTY()
	TObjectPtr<USkillExecutorConfig> Parent;
	UPROPERTY(EditDefaultsOnly)
	FName SkillDescriptorName;
	UPROPERTY(meta=(IgnoreForMemberInitializationTest))
	FGuid ID;
	UPROPERTY(meta=(ExcludeFromHash))
	bool bExpand=true;
	UPROPERTY()
	bool bEnabled=true;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor StateColor=FLinearColor::Gray;
	USkillExecutorConfig * FindRootSkillExecutorConfig(USkillExecutorConfig * ChildConfig)
	{	
		if (ChildConfig && ChildConfig->Parent)
		{
		  return FindRootSkillExecutorConfig(ChildConfig->Parent);
		}
		return ChildConfig;
	};
};

UCLASS(Blueprintable)
class ACTIONSKILLASSETRUNTIME_API USkillExecutorDescriptorAsset:public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,Instanced,BlueprintReadWrite)
	TArray<USkillExecutorConfig*> ExecutorConfigs;
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UObject> EditorData;
#endif
};

UCLASS(Blueprintable)
class ACTIONSKILLASSETRUNTIME_API USkillExecutorDescriptorAssetSum : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag,USkillExecutorDescriptorAsset * > SKillExecutorAssets;
	FGameplayTag GetDesiredStateTag(USkillExecutorConfig * InSkillExecutorConfig)
	{
		USkillExecutorDescriptorAsset* Asset=InSkillExecutorConfig->GetTypedOuter<USkillExecutorDescriptorAsset>();
		FGameplayTag DesiredStateTag=*SKillExecutorAssets.FindKey(Asset);
		return DesiredStateTag;
	};
};

UCLASS(Blueprintable)
class ACTIONSKILLASSETRUNTIME_API USkillExecutorDescriptorAssetSumLink : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag,USkillExecutorDescriptorAssetSum *> SkillExecutorSumLink;
	FGameplayTag GetDesiredLinkTag(USkillExecutorDescriptorAssetSum * SkillExecutorSum)
	{
		FGameplayTag DesiredStateTag=*SkillExecutorSumLink.FindKey(SkillExecutorSum);
		return DesiredStateTag;
	}
};
