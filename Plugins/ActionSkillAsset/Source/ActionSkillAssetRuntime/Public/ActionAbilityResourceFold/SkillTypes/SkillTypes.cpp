#include "SkillTypes.h"

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
