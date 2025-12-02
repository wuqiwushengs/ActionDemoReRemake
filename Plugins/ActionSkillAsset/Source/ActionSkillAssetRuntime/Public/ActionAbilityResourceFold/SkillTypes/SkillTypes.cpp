#include "SkillTypes.h"

#include "AbilitySystemComponent.h"
#include "ActionAbilityResourceFold/SkillClip/SkillClip_PlayMontage.h"
#include "ActionAbilityResourceFold/SkillClip/SkillClipAbility/SkillClipAbilityBase.h"

UClass* FSkillContainer::GetCurrentSkill()
{
	
		switch (SelectedSkill)
		{
		case ESkillCLipType::Ability:
			return AbilitySkill;
		case ESkillCLipType::NormalSkillClip:
			return ClipSkill;
		}
	UE_LOG(LogTemp, Warning, TEXT("MakeVariantSkill: Clip is not a valid skill type!"));
	return nullptr;
}
bool FSkillContainer::IsValid()
{
	switch (SelectedSkill)
	{
	case ESkillCLipType::Ability:
		if (AbilitySkill) return true;
		return  false;
	case ESkillCLipType::NormalSkillClip:
		if (ClipSkill) return true;
		return  false;
	}
	return false;
}
TVariant<TSubclassOf<USkillClip_PlayMontage>,TSubclassOf<USkillClipAbilityBase>> FSkillContainer::
MakeVariantSkill(UClass*  Clip)
{
	TVariant<TSubclassOf<USkillClip_PlayMontage>, TSubclassOf<USkillClipAbilityBase>> Variant;
	if(!Clip) return Variant;
	if (Clip->IsChildOf(USkillClipAbilityBase::StaticClass()))
	{
		Variant.Set<TSubclassOf<USkillClipAbilityBase>>(Clip);
	}
	else if (Clip->IsChildOf(USkillClip_PlayMontage::StaticClass()))
	{
		Variant.Set<TSubclassOf<USkillClip_PlayMontage>>(Clip);
	}
	return Variant;
}

void FTipSkill::GiveAllAbility( UAbilitySystemComponent* OwingAbilitySystemComponent)
{
	SkillAbilityFactory::GiveSkillAbility(this->TipSkillClass,this->PostTipContent,OwingAbilitySystemComponent);
}

void FHoldSkill::GiveAllAbility(UAbilitySystemComponent* OwingAbilitySystemComponent)
{
	SkillAbilityFactory::GiveSkillAbility(this->HoldSkillClass,this->PostHoldContent,OwingAbilitySystemComponent);
}

void FMultiTipSkill::GiveAllAbility(UAbilitySystemComponent* OwingAbilitySystemComponent)
{
	SkillAbilityFactory::GiveSkillAbility(this->MultiTipSkillClass,this->PostMultiTipContent,OwingAbilitySystemComponent);
}

void FSkillContext::TryGiveAllAbility(UAbilitySystemComponent * OwingAbilitySystemComponent)
{
	SkillAbilityFactory::GiveSkillAbility(PreTipContent,FSkillContainer(),OwingAbilitySystemComponent);
	if(bShouldGiveTipAbility())
	{
		TipSkill.GiveAllAbility(OwingAbilitySystemComponent);
	}
	if(bShouldGiveHoldAbility())
	{
		HoldSkill.GiveAllAbility(OwingAbilitySystemComponent);
	}
	if(bShouldGiveMultiTipSkill())
	{
		MultiTipSkill.GiveAllAbility(OwingAbilitySystemComponent);
	}
}

void  SkillAbilityFactory::GiveSkillAbility(FSkillContainer first, FSkillContainer Second,
                                            UAbilitySystemComponent* OwingAbilitySystemComponent)
{
	if(	first.IsValid()&&first.SelectedSkill==ESkillCLipType::Ability)
	{
		FGameplayAbilitySpec Spec(first.AbilitySkill);
		OwingAbilitySystemComponent->GiveAbility(Spec);
	}
	if(	 Second.IsValid()&&  Second.SelectedSkill==ESkillCLipType::Ability)
	{
		FGameplayAbilitySpec Spec( Second.AbilitySkill);
		OwingAbilitySystemComponent->GiveAbility(Spec);
	}
}



