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

UCLASS()
class TPSPORTFOLIO_API UTPSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;
public:
	void PlayAnimMontage();

private:
	void InitMontage();

	void AddMontage(FString montageaddress, EWeaponType weapontype, EWeaponMontageState statetype);
	UAnimMontage* GetMontage(EWeaponType weapontype, EWeaponMontageState statetype);

	void PlayReload(EWeaponType weapontype);
	void PlayShot(EWeaponType weapontype);
	void PlayEquip(EWeaponType weapontype);
	void PlayAim(EWeaponType weapontype);
	void PlayRun(EWeaponType weapontype);
	void PlayIdle(EWeaponType weapontype);



	void BlendOutReload(class UAnimMontage*, bool interrupt);
	void BlendOutShot(class UAnimMontage*, bool interrupt);
	void BlendOutEquip(class UAnimMontage*, bool interrupt);

	UFUNCTION()
	void AnimNotify_WeaponSet();

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
};
