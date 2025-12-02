// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "CollisionSystem/Interface/CollisionSystemInterface.h"
#include "GameFramework/Character.h"
#include "InputFold/InputType.h"
#include "Interface/CharacterInfoInterface.h"
#include "ActionPlayerCharacter.generated.h"

class UActionGlobalAttributeSet;
class UMotionWarpingComponent;
class UActionEnemyFollowComponent;
class UActionAttackCollisionComponent;
struct FCharacterNormalInputData;
enum class ETriggerEvent : uint8;
class AActionPlayController;
class UInputDataAsset;
struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;
struct FGameplayTag;
struct FInputActionInstance;
class UActionAbilitySystemComponent;

//已经实现了基础的移动和视角更改如果需要更改的话请
UCLASS()
class ACTIONSKILLASSETRUNTIME_API AActionPlayerCharacter : public ACharacter,public  IAbilitySystemInterface,public ICollisionSystemInterface,public ICharacterInfoInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AActionPlayerCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UActionAbilitySystemComponent * GetActionAbilitySystemComponent() const ;
	UInputDataAsset * GetInputDataAsset() const ;
	virtual FCharacterNormalInputData GetCharacterNormalInputData() override;
	UFUNCTION(BlueprintCallable,BlueprintPure)	
	virtual FCharacterNormalInputData GetNormalInputData() { return CharacterNormalInputData;};
protected:
	virtual UAttackCollisionComponent * GetAttackCollisionComponent_Implementation() override;
	virtual void OnBeAttackRecallInternal_Implementation(FAttackedResult AttackedResult) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UActionAbilitySystemComponent * ActionAbilitySystemComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UActionAttackCollisionComponent * AttackCollisionComponent;
	UPROPERTY()
	AActionPlayController * PlayController;
	UPROPERTY(BlueprintReadOnly)
	FCharacterNormalInputData CharacterNormalInputData;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
#pragma  region NormalInput
	UPROPERTY(EditDefaultsOnly)
	//选择攻击插槽的组的名字
	FName AttackSlotGroupName;
	void OnInputMove(const FInputActionValue& InputActionValue);
	virtual void ProcessMove(const FInputActionValue& InputActionValue);
	void OnInputLook( const FInputActionValue& InputActionValue);
	virtual void ProcessLook(const FInputActionValue& InputActionValue);
	void CheckPostAnimPlayAndStop();
#pragma endregion 
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void OnAbilityInputTrigger(const FInputActionInstance& InputInfo ,FGameplayTag InputData,ETriggerEvent TriggerEvent) ;

	/*EnemyFollow*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UActionEnemyFollowComponent * EnemyFollowComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UMotionWarpingComponent *   MotionWarpingComponent;
	TObjectPtr<UActionGlobalAttributeSet> CharacterNormalAttribute;
};
