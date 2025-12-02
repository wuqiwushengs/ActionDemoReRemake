// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFollow/Notify/CheckFollowEnemy.h"
#include "MotionWarpingComponent.h"
#include "EnemyFollow/Component/ActionEnemyFollowComponent.h"
#include "GamePlay/Character/ActionPlayerCharacter.h"

struct  FWarpInformation
{
public:
	static  const FName WarpName;
};
const  FName FWarpInformation::WarpName=TEXT("EnemyFollow");  
void UCheckFollowEnemy::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                               const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if(MeshComp->GetOwner())
	{
		 AActionPlayerCharacter *Character=Cast<AActionPlayerCharacter>(MeshComp->GetOwner());
		if(!Character) return;
		Character->EnemyFollowComponent->ClearAttachedActor();
		Character->EnemyFollowComponent->SelectedFinalActor();
		if(AActor * CatchActor=Character->EnemyFollowComponent->GetCatchActor();CatchActor)
		{
			FVector Location=Character->GetActorLocation()+WarpDistanceWarp * FRotationMatrix::MakeFromX(CatchActor->GetActorLocation()-Character->GetActorLocation()).Rotator().Vector();
			FRotator Rotation=FRotationMatrix::MakeFromX(CatchActor->GetActorLocation()-Character->GetActorLocation()).Rotator();
			DrawDebugLine(MeshComp->GetOwner()->GetWorld(),Character->GetActorLocation(),CatchActor->GetActorLocation(),FColor::Black,false,3,5,5);
			Rotation.Pitch=0;
			Rotation.Roll=0;
			if(ToTargetDirectly)
			{
				Location=CatchActor->GetActorLocation();
			}
			Character->MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FWarpInformation::WarpName,Location,Rotation);
		}
		else
		{
			FVector MoveDirection=Character->GetCharacterNormalInputData().MoveInputValue.X* FRotationMatrix(Character->GetControlRotation()).GetUnitAxis(EAxis::X)+Character->GetCharacterNormalInputData().MoveInputValue.Y*FRotationMatrix(Character->GetControlRotation()).GetUnitAxis(EAxis::Y);
			FVector FinalLocation=Character->GetActorLocation()+WarpDistanceWarp*MoveDirection;
			FRotator FinalRotation=FRotationMatrix::MakeFromX(MoveDirection).Rotator();
			if(MoveDirection.IsNearlyZero())
			{
				FinalLocation=Character->GetActorLocation() +FRotationMatrix(Character->GetActorRotation()).GetUnitAxis(EAxis::X)*WarpDistanceWarp;
				FinalRotation=Character->GetActorRotation();
			}
			Character->MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FWarpInformation::WarpName,FinalLocation,FinalRotation);
		}
	}
}
