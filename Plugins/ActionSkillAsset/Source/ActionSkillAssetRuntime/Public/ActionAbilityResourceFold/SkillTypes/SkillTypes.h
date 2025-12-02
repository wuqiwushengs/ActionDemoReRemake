#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SkillTypes.generated.h"

class UAbilitySystemComponent;
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
	Press=0,
	Release=1,
};
UENUM(BlueprintType)
enum  class EPreHoldTimeType:uint8
{
	Loop,
	Once,
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

	//为避免输入检测被重叠需要再执行技能时添加一个结构体id用来避免重复的松开同一个按键导致
USTRUCT(BlueprintType)
struct FSkillTitle
{
	GENERATED_BODY()
	FSkillTitle():InputType(EInputType::Press),InputTime(0.0f){}
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
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float MinHoldTime;
	UPROPERTY(BlueprintReadOnly)
	float CurrentHoldTime;
	bool bHoldComplete() { return CurrentHoldTime>=MinHoldTime;}
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

struct SkillAbilityFactory
{
	static void  GiveSkillAbility(FSkillContainer first, FSkillContainer Second,
	UAbilitySystemComponent* OwingAbilitySystemComponent);
};
USTRUCT(BlueprintType)
struct FSkillFunc
{
	GENERATED_BODY()
	virtual  ~FSkillFunc()=default;
	virtual  void GiveAllAbility( UAbilitySystemComponent* OwingAbilitySystemComponent){};
};
USTRUCT(BlueprintType)
struct FTipSkill:public FSkillFunc
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	FSkillContainer TipSkillClass;
	UPROPERTY(EditAnywhere)
	FSkillContainer PostTipContent;
	virtual void GiveAllAbility( UAbilitySystemComponent* OwingAbilitySystemComponent) override;
};
USTRUCT(BlueprintType)
struct FHoldSkill:public FSkillFunc
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	EPreHoldTimeType HoldType;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FHoldSkillInfo HoldSkillInfo;
	//执行阶段
	UPROPERTY(EditAnywhere)
	FSkillContainer HoldSkillClass;
	UPROPERTY(EditAnywhere)
	bool SkipHoldExecuteAfterRelease=false;
	//后摇如果有的话
	UPROPERTY(EditAnywhere)
	FSkillContainer PostHoldContent;
	virtual void GiveAllAbility(UAbilitySystemComponent* OwingAbilitySystemComponent) override;
};
USTRUCT(BlueprintType)
struct FMultiTipSkill:public FSkillFunc
{
	GENERATED_BODY()
	//执行阶段
	UPROPERTY(EditAnywhere)
	FSkillContainer MultiTipSkillClass;
	//后摇如果有的话
	UPROPERTY(EditAnywhere)
	FSkillContainer PostMultiTipContent;
	virtual void GiveAllAbility(UAbilitySystemComponent* OwingAbilitySystemComponent) override;
};
USTRUCT(BlueprintType)
struct FSkillContext
{
	GENERATED_BODY()
	~FSkillContext()=default;
	void InitialSkillContext()
	{
		HoldSkill.HoldSkillInfo.CurrentHoldTime=0.0f;
		DoOnceAttackRelease=false;
	};
	UPROPERTY(EditAnywhere)
	bool  bCanSavetoOffset=true;
	UPROPERTY(EditAnywhere)
	bool bNeedTipSkill=false;
	//不需要所有都加内容，只需要加所需要的即可，单击和其他技能相互冲突，不需要一起添加
	UPROPERTY(EditAnywhere)
	FSkillContainer PreTipContent;
	UPROPERTY(EditAnywhere,meta=(EditCondition="bNeedTipSkill"))
	FTipSkill TipSkill;
	bool bShouldGiveTipAbility() const  { return bNeedTipSkill;}
	bool bShouldLoadTipSkillInitial() const  {return bNeedTipSkill&&!bNeedHoldSkill&&!bNeedMultiTipSkill;}
	UPROPERTY(EditAnywhere)
	bool bNeedHoldSkill=false;
	UPROPERTY(EditAnywhere,meta=(EditCondition="bNeedHoldSkill"))
	FHoldSkill HoldSkill;
	bool bShouldGiveHoldAbility() const  {return bNeedHoldSkill;}
	bool bShouldLoadHoldSkillInitial() const { return bNeedHoldSkill;}
	void SetCurrentHoldTime(float IntervalTime)
	{
		HoldSkill.HoldSkillInfo.CurrentHoldTime=IntervalTime;
	}
	UPROPERTY(EditAnywhere)
	bool bNeedMultiTipSkill=false;
	UPROPERTY(EditAnywhere,meta=(EditCondition="bNeedMultiTipSkill"))
	FMultiTipSkill MultiTipSkill;
	bool bShouldGiveMultiTipSkill() const  { return bNeedMultiTipSkill;}
	bool bShouldLoadMultiTipSkill() const {return bNeedMultiTipSkill&&!bNeedHoldSkill;}
	void TryGiveAllAbility(UAbilitySystemComponent * OwingAbilitySystemComponent);
	bool ShouldStopCurrentPreHoldSkillOnTrigger()
	{
		if (!bNeedMultiTipSkill ||(bNeedMultiTipSkill && HoldSkill.HoldSkillInfo.bHoldComplete())&& HoldSkill.HoldType==EPreHoldTimeType::Loop)
		{
			return true;
		}
		if (!bNeedMultiTipSkill && HoldSkill.HoldType==EPreHoldTimeType::Once && !HoldSkill.HoldSkillInfo.bHoldComplete())
		{
			return true;
		}
		return false;
	}
	UPROPERTY(Transient)
	bool DoOnceAttackRelease=false;
	//获取前摇的技能
	UClass * GetPreSkill()
	{
		return  PreTipContent.GetCurrentSkill();
	};
	//获取中间阶段的技能
	UClass * GetProcessSkill(ESkillType SkillType)
	{
		switch (SkillType)
		{
		case ESkillType::TipSkill:
			return TipSkill.TipSkillClass.GetCurrentSkill();
		case ESkillType::HoldSkill:
			return HoldSkill.HoldSkillClass.GetCurrentSkill();
		case ESkillType::MultiTipSkill:
			return MultiTipSkill.MultiTipSkillClass.GetCurrentSkill();
		default:
			break;
		}
		return nullptr;
	};
	UClass * GetPostSkill(ESkillType SkillType)
	{
		switch (SkillType)
		{
		case ESkillType::TipSkill:
			return TipSkill.PostTipContent.GetCurrentSkill();
		case ESkillType::HoldSkill:
			return HoldSkill.PostHoldContent.GetCurrentSkill();
		case ESkillType::MultiTipSkill:
			return MultiTipSkill.PostMultiTipContent.GetCurrentSkill();
		}
		return nullptr;
	};
};


