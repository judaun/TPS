// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TPSAnimInstance.generated.h"

/**
 * 
 */
class ATPSPortfolioCharacter;
enum class EWeaponType : uint8;
enum class ECharacterState : uint8;
enum class EWeaponMontageState : uint8;
class UAnimMontage;

DECLARE_MULTICAST_DELEGATE(FOnWeaponSet);
DECLARE_MULTICAST_DELEGATE(FOnJumpAway);
DECLARE_MULTICAST_DELEGATE(FOnLanding);
DECLARE_MULTICAST_DELEGATE(FOnCrawlEnd);
DECLARE_MULTICAST_DELEGATE(FOnFootstep_L);
DECLARE_MULTICAST_DELEGATE(FOnFootstep_R);
DECLARE_MULTICAST_DELEGATE(FOnSwosh);

UCLASS()
class TPSPORTFOLIO_API UTPSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;
public:
	void PlayAnimMontage();
	void PlayAttack(EWeaponType weapontype, bool ismelee);
private:
	void InitMontage();

	void AddMontage(FString montageaddress, EWeaponType weapontype, EWeaponMontageState statetype);
	UAnimMontage* GetMontage(EWeaponType weapontype, EWeaponMontageState statetype);

	void PlayReload(EWeaponType weapontype);
	void PlayShot(EWeaponType weapontype);
	void PlayEquip(EWeaponType weapontype);
	void PlayAim(EWeaponType weapontype);
	void PlayRun(EWeaponType weapontype);
	void PlaySprint(EWeaponType weapontype);
	void PlayIdle(EWeaponType weapontype);
	void PlayMelee(EWeaponType weapontype);

	void BlendOutReload(class UAnimMontage*, bool interrupt);
	void BlendOutShot(class UAnimMontage*, bool interrupt);
	void BlendOutEquip(class UAnimMontage*, bool interrupt);
	void BlendOutMelee(class UAnimMontage*, bool interrupt);

	UFUNCTION()
	void AnimNotify_WeaponSet();
	UFUNCTION()
	void AnimNotify_JumpAway();
	UFUNCTION()
	void AnimNotify_Landing();
	UFUNCTION()
	void AnimNotify_CrawlEnd();
	UFUNCTION()
	void AnimNotify_Footstep_L();
	UFUNCTION()
	void AnimNotify_Footstep_R();
	UFUNCTION()
	void AnimNotify_Swosh();
private:
	UPROPERTY()
	TWeakObjectPtr<ATPSPortfolioCharacter> pCharacter;

	UPROPERTY()
	TMap<EWeaponMontageState, UAnimMontage*> map_AnimMontage;
	UPROPERTY()
	TMap<EWeaponMontageState, UAnimMontage*> map_AnimMontageRifle;
	UPROPERTY()
	TMap<EWeaponMontageState, UAnimMontage*> map_AnimMontageShotgun;

	FOnWeaponSet OnWeaponSet;
	FOnJumpAway OnJumpAway;
	FOnLanding OnLanding;
	FOnCrawlEnd OnCrawlEnd;
	FOnFootstep_L OnFootstep_L;
	FOnFootstep_R OnFootstep_R;
	FOnSwosh OnSwosh;
};
