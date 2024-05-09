// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSAnimInstance.h"
#include "TPSPortfolioCharacter.h"
#include "TPSEnum.h"
#include "Animation/AnimMontage.h"

void UTPSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	auto pPawn = TryGetPawnOwner();
	if (IsValid(pPawn))
		pCharacter = TWeakObjectPtr<ATPSPortfolioCharacter>(Cast<ATPSPortfolioCharacter>(pPawn));

	InitMontage();

}

void UTPSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	PlayAnimMontage();
}

void UTPSAnimInstance::PlayAnimMontage()
{
	if (nullptr == pCharacter) return;
	auto WeaponType = pCharacter->GetWeaponType();
	//비무장상태에서는 무기관련 몽타주 없음
	if (pCharacter->GetIsEquiping())
	{
		PlayEquip(WeaponType);
		return;
	}

	if (WeaponType == EWeaponType::WEAPON_NONE) 
	{
		Montage_Stop(0.5f);
		return;
	}
	
	auto CharacterState = pCharacter->GetCharacterState();
	
	//ReloadMontage
	if(pCharacter->GetisReloading())
	{
		PlayReload(WeaponType);
		return;
	}

	switch (CharacterState)
	{
	case ECharacterState::IDLE:
		PlayIdle(WeaponType);
		break;
	case ECharacterState::RUN:
		PlayRun(WeaponType);
		break;
	case ECharacterState::SPRINT:
		Montage_Stop(0.5f);
		break;
	case ECharacterState::AIM:
		pCharacter->GetisAttacking() ? PlayShot(WeaponType) : PlayAim(WeaponType);
		break;
	case ECharacterState::BRAKE:
		break;
	}
}

void UTPSAnimInstance::InitMontage()
{
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Pistol_2H_Idle.TPS_Pistol_2H_Idle'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::IDLE);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Pistol_2H_Aim.TPS_Pistol_2H_Aim'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::AIM);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Pistol_2H_Recoil.TPS_Pistol_2H_Recoil'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::SHOT);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Pistol_2H_Reload.TPS_Pistol_2H_Reload'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::RELOAD);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Run_EquipGun.TPS_Run_EquipGun'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::RUN);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Pistol_Equip.TPS_Pistol_Equip'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::EQUIP);

	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Rifle_Idle.TPS_Rifle_Idle'"), EWeaponType::WEAPON_RIFLE, EWeaponMontageState::IDLE);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Rifle_Aim.TPS_Rifle_Aim'"), EWeaponType::WEAPON_RIFLE, EWeaponMontageState::AIM);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Rifle_Fire.TPS_Rifle_Fire'"), EWeaponType::WEAPON_RIFLE, EWeaponMontageState::SHOT);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Rifle_Reload.TPS_Rifle_Reload'"), EWeaponType::WEAPON_RIFLE, EWeaponMontageState::RELOAD);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Run_EquipGun.TPS_Run_EquipGun'"), EWeaponType::WEAPON_RIFLE, EWeaponMontageState::RUN);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Rifle_Equip.TPS_Rifle_Equip'"), EWeaponType::WEAPON_RIFLE, EWeaponMontageState::EQUIP);

	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Shotgun_Idle.TPS_Shotgun_Idle'"), EWeaponType::WEAPON_SHOTGUN, EWeaponMontageState::IDLE);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Shotgun_Aim.TPS_Shotgun_Aim'"), EWeaponType::WEAPON_SHOTGUN, EWeaponMontageState::AIM);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Shotgun_Fire.TPS_Shotgun_Fire'"), EWeaponType::WEAPON_SHOTGUN, EWeaponMontageState::SHOT);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Shotgun_Reload.TPS_Shotgun_Reload'"), EWeaponType::WEAPON_SHOTGUN, EWeaponMontageState::RELOAD);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Run_EquipGun.TPS_Run_EquipGun'"), EWeaponType::WEAPON_SHOTGUN, EWeaponMontageState::RUN);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Rifle_Equip.TPS_Rifle_Equip'"), EWeaponType::WEAPON_SHOTGUN, EWeaponMontageState::EQUIP);
}

void UTPSAnimInstance::AddMontage(FString montageaddress, EWeaponType weapontype, EWeaponMontageState statetype)
{
	UAnimMontage* pMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *montageaddress));
	
	if (pMontage)
	{
		switch (weapontype)
		{
		case EWeaponType::WEAPON_HANDGUN:
			map_AnimMontage.Add(MakeTuple(statetype, pMontage));
			break;
		case EWeaponType::WEAPON_RIFLE:
			map_AnimMontageRifle.Add(MakeTuple(statetype, pMontage));
			break;
		case EWeaponType::WEAPON_SHOTGUN:
			map_AnimMontageShotgun.Add(MakeTuple(statetype, pMontage));
			break;
		}
		
	}
}

UAnimMontage* UTPSAnimInstance::GetMontage(EWeaponType weapontype, EWeaponMontageState statetype)
{
	UAnimMontage** map_find = nullptr;

	switch (weapontype)
	{
	case EWeaponType::WEAPON_HANDGUN:
		map_find = map_AnimMontage.Find(statetype);
		break;
	case EWeaponType::WEAPON_RIFLE:
		map_find = map_AnimMontageRifle.Find(statetype);
		break;
	case EWeaponType::WEAPON_SHOTGUN:
		map_find = map_AnimMontageShotgun.Find(statetype);
		break;
	default:
		map_find = map_AnimMontage.Find(statetype);
		break;
	}
	
	if (nullptr == map_find) return nullptr;

	return *(map_find);
}

void UTPSAnimInstance::PlayReload(EWeaponType weapontype)
{
	auto pMontage = GetMontage(weapontype, EWeaponMontageState::RELOAD);
	if (!Montage_IsPlaying(pMontage))
	{
		Montage_Play(pMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);

		FOnMontageBlendingOutStarted BlendOutDele;
		BlendOutDele.BindUObject(this, &UTPSAnimInstance::BlendOutReload);
		Montage_SetBlendingOutDelegate(BlendOutDele);
	}
}

void UTPSAnimInstance::PlayShot(EWeaponType weapontype)
{
	auto pMontage = GetMontage(weapontype, EWeaponMontageState::SHOT);
	//if (!Montage_IsPlaying(pMontage))
	{
		Montage_Play(pMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);

		FOnMontageBlendingOutStarted BlendOutDele;
		BlendOutDele.BindUObject(this, &UTPSAnimInstance::BlendOutShot);
		Montage_SetBlendingOutDelegate(BlendOutDele);
	}
}

void UTPSAnimInstance::PlayEquip(EWeaponType weapontype)
{
	auto pMontage = GetMontage(weapontype, EWeaponMontageState::EQUIP);
	if (!Montage_IsPlaying(pMontage))
	{
		UE_LOG(LogTemp, Log, TEXT("PlayEquip Montage"));
		Montage_Play(pMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);

		FOnMontageBlendingOutStarted BlendOutDele;
		BlendOutDele.BindUObject(this, &UTPSAnimInstance::BlendOutEquip);
		Montage_SetBlendingOutDelegate(BlendOutDele);
	}
}

void UTPSAnimInstance::PlayAim(EWeaponType weapontype)
{
	auto pMontage = GetMontage(weapontype, EWeaponMontageState::AIM);
	if (!Montage_IsPlaying(pMontage))
	{
		Montage_Play(pMontage, 0.f, EMontagePlayReturnType::MontageLength, 0.5f, true);
	}
}

void UTPSAnimInstance::PlayRun(EWeaponType weapontype)
{
	auto pMontage = GetMontage(weapontype, EWeaponMontageState::RUN);
	if (!Montage_IsPlaying(pMontage))
	{
		Montage_Play(pMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
	}
}

void UTPSAnimInstance::PlayIdle(EWeaponType weapontype)
{
	auto pMontage = GetMontage(weapontype, EWeaponMontageState::IDLE);
	if (!Montage_IsPlaying(pMontage))
	{
		Montage_Play(pMontage, 0.f, EMontagePlayReturnType::MontageLength, 0.f, true);
	}
}

void UTPSAnimInstance::BlendOutReload(class UAnimMontage*, bool interrupt)
{
	if (nullptr == pCharacter) return;
	
	pCharacter->ReloadComplete();
}

void UTPSAnimInstance::BlendOutShot(class UAnimMontage*, bool interrupt)
{
	if (nullptr == pCharacter) return;

	pCharacter->SetAttacking(false);
}

void UTPSAnimInstance::BlendOutEquip(class UAnimMontage*, bool interrupt)
{
	if(nullptr == pCharacter) return;
	pCharacter->SetEquiping(false);
	UE_LOG(LogTemp, Log, TEXT("SetEquiping False"));

}

void UTPSAnimInstance::AnimNotify_WeaponSet()
{
	if (nullptr == pCharacter) return;
	pCharacter->NotifyEquip();
}
