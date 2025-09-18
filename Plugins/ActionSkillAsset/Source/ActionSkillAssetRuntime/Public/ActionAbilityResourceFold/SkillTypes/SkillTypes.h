#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SkillTypes.generated.h"

class USkillClipAbilityBase;
class USkillClipAbility_MultiTipSkill;
class USkillClipAbility_HoldSkill;
class USkillClip_PlayMontage;
class USkillClipAbility_TipSkill;
DECLARE_DYNAMIC_DELEGATE(FEndAbilityEvent);
UENUM(BlueprintType)
enum class  ESkillCLipType:uint8
{
	Ability,
	NormalSkillClip,
};
UENUM(BlueprintType)
enum class EInputType: uint8
{
	Press,
	Release,
};
UENUM(BlueprintType)
enum  class EPreHoldTimeType:uint8
{
	Loop,
	Once,
};
	//为避免输入检测被重叠需要再执行技能时添加一个结构体id用来避免重复的松开同一个按键导致
	USTRUCT(BlueprintType)
	struct FSkillTitle
	{
		GENERATED_BODY()
		FSkillTitle()
			:InputType(EInputType::Press)
		{
			
		}
		FSkillTitle(EInputType InInputType,float CurrentTime,FGameplayTag InTag)
		{
			InputType = InInputType;
			SkillInputId=FGuid::NewGuid();
			InputTime=CurrentTime;
			InputTag=InTag;
		}
		FGuid SkillInputId;
		FGameplayTag InputTag;
		EInputType InputType;
		float InputTime;
		bool operator==(const FSkillTitle& InTitle) const
		{
			return  InTitle.InputTag==InputTag&&InTitle.SkillInputId==SkillInputId;
		}
	};

USTRUCT(BlueprintType)
struct FMultiAnimPlayed
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<UAnimMontage*> PlayedMontages;
	TFunction<UAnimMontage *()> MontageRule;
};
USTRUCT(BlueprintType)
struct FHoldSkillInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	float MinHoldTime;
	float CurrentHoldTime;;
};
USTRUCT(BlueprintType)
struct  FSkillContainer
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	ESkillCLipType SelectedSkill;
	UPROPERTY(EditAnywhere,meta=(EditCondition="SelectedSkill==ESkillCLipType::Ability",EditConditionHides))
	TSubclassOf<USkillClipAbilityBase> AbilitySkill;
	UPROPERTY(EditAnywhere,meta=(EditCondition="SelectedSkill==ESkillCLipType::NormalSkillClip",EditConditionHides))
	TSubclassOf<USkillClip_PlayMontage> ClipSkill;
	UClass* GetCurrentSkill();
	bool IsValid();
	static  TVariant<TSubclassOf<USkillClip_PlayMontage>,TSubclassOf<USkillClipAbilityBase>>  MakeVariantSkill(UClass*  Clip);
};


USTRUCT(BlueprintType)
struct FTipSkill
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	FSkillContainer TipSkillClass;
	UPROPERTY(EditAnywhere)
	FSkillContainer PostTipContent;
};
USTRUCT(BlueprintType)
struct FHoldSkill
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	EPreHoldTimeType HoldType;
	UPROPERTY(EditAnywhere)
	FHoldSkillInfo HoldSkillInfo;
	//执行阶段
	UPROPERTY(EditAnywhere)
	FSkillContainer HoldSkillClass;
	//后摇如果有的话
	UPROPERTY(EditAnywhere)
	FSkillContainer PostHoldContent;
};
USTRUCT(BlueprintType)
struct FMultiTipSkill
{
	GENERATED_BODY()
	//执行阶段
	UPROPERTY(EditAnywhere)
	FSkillContainer MultiTipSkillClass;
	//后摇如果有的话
	UPROPERTY(EditAnywhere)
	FSkillContainer PostMultiTipContent;
};

UENUM()
enum ESkillType
{
	TipSkill,
	HoldSkill,
	MultiTipSkill,
	None,
};

UENUM(BlueprintType)
enum class  ESkillWeight:uint8
{
	Weight0=0,
	Weight1=1,
	Weight2=2,
	Weight3=3,
	Weight4=4,
	Weight5=5,
	Weight6=6,
	Weight7=7,
	Weight8=8,
	Weight9=9,
	Weight10=10,
};


