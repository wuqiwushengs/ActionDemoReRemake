#include "GamePlayTags.h"

namespace GamePlayTags
{

	
	//提示连击的标签
	ACTIONSKILLASSETRUNTIME_API UE_DEFINE_GAMEPLAY_TAG(MultiTipSign,"SkillInputSign.MultiTipSign")
	ACTIONSKILLASSETRUNTIME_API UE_DEFINE_GAMEPLAY_TAG(ExecutePreInput,"SkillInputSign.PreInputSign")
	ACTIONSKILLASSETRUNTIME_API UE_DEFINE_GAMEPLAY_TAG(ExecutePreInputImmediately,"SkillInputSign.PreInputImmediatelySign")

	//状态标签
	ACTIONSKILLASSETRUNTIME_API UE_DEFINE_GAMEPLAY_TAG(Ground,"SkillSelectState.Ground")
	ACTIONSKILLASSETRUNTIME_API UE_DEFINE_GAMEPLAY_TAG(InAir,"SkillSelectState.InAir")

	

	//后摇动画的标签
	ACTIONSKILLASSETRUNTIME_API UE_DEFINE_GAMEPLAY_TAG(PostAnim,"Anim.PostAnim")
	
}
