// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSAnimInstance.h"
#include "TPSPortfolioCharacter.h"
#include "TPSEnum.h"
#include "Animation/AnimMontage.h"
#include "TPSGameInstance.h"

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

	if (pCharacter->GetIsHeal())
	{
		PlayHeal();
		return;
	}
	
	if (pCharacter->GetIsHit())
	{
		PlayHit();
		return;
	}

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

	if (pCharacter->GetIsCrawl()|| pCharacter->GetIsCrawltoIdle())
	{
		Montage_Stop(0.1f);
		return;
	}

	if (pCharacter->GetisAttacking())
	{
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
		PlaySprint(WeaponType);
		break;
	case ECharacterState::AIM:
		PlayAim(WeaponType);
		break;
	case ECharacterState::BRAKE:
	case ECharacterState::EVADE:
	default: Montage_Stop(0.3f);
		break;
	}
}

void UTPSAnimInstance::PlayAttack(EWeaponType weapontype, bool ismelee)
{
	ismelee ? PlayMelee(weapontype) : PlayShot(weapontype);
}

void UTPSAnimInstance::InitMontage()
{
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Pistol_2H_Idle.TPS_Pistol_2H_Idle'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::IDLE);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Pistol_2H_Aim.TPS_Pistol_2H_Aim'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::AIM);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Pistol_Fire.TPS_Pistol_Fire'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::SHOT);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Pistol_2H_Reload.TPS_Pistol_2H_Reload'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::RELOAD);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Run_Pistol.TPS_Run_Pistol'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::RUN);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Pistol_Equip.TPS_Pistol_Equip'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::EQUIP);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Pistol_Melee.TPS_Pistol_Melee'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::MELEE);

	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_hit.TPS_hit'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::HIT);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Healing.TPS_Healing'"), EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::HEAL);

	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Rifle_Idle.TPS_Rifle_Idle'"), EWeaponType::WEAPON_RIFLE, EWeaponMontageState::IDLE);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Rifle_Aim.TPS_Rifle_Aim'"), EWeaponType::WEAPON_RIFLE, EWeaponMontageState::AIM);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Rifle_Fire.TPS_Rifle_Fire'"), EWeaponType::WEAPON_RIFLE, EWeaponMontageState::SHOT);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Rifle_Reload.TPS_Rifle_Reload'"), EWeaponType::WEAPON_RIFLE, EWeaponMontageState::RELOAD);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Run_EquipGun.TPS_Run_EquipGun'"), EWeaponType::WEAPON_RIFLE, EWeaponMontageState::RUN);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Rifle_Equip.TPS_Rifle_Equip'"), EWeaponType::WEAPON_RIFLE, EWeaponMontageState::EQUIP);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Rifle_Melee.TPS_Rifle_Melee'"), EWeaponType::WEAPON_RIFLE, EWeaponMontageState::MELEE);

	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Shotgun_Idle.TPS_Shotgun_Idle'"), EWeaponType::WEAPON_SHOTGUN, EWeaponMontageState::IDLE);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Shotgun_Aim.TPS_Shotgun_Aim'"), EWeaponType::WEAPON_SHOTGUN, EWeaponMontageState::AIM);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Shotgun_Fire.TPS_Shotgun_Fire'"), EWeaponType::WEAPON_SHOTGUN, EWeaponMontageState::SHOT);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Shotgun_Reload.TPS_Shotgun_Reload'"), EWeaponType::WEAPON_SHOTGUN, EWeaponMontageState::RELOAD);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Run_EquipGun.TPS_Run_EquipGun'"), EWeaponType::WEAPON_SHOTGUN, EWeaponMontageState::RUN);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/TPS_Shotgun_Equip.TPS_Shotgun_Equip'"), EWeaponType::WEAPON_SHOTGUN, EWeaponMontageState::EQUIP);
	AddMontage(TEXT("/Script/Engine.AnimMontage'/Game/Characters/PlayerCharacters/Vepley/Animation/Lyra/TPS_Shotgun_Melee.TPS_Shotgun_Melee'"), EWeaponType::WEAPON_SHOTGUN, EWeaponMontageState::MELEE);
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
		Montage_Play(pMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, false);

		FOnMontageBlendingOutStarted BlendOutDele;
		BlendOutDele.BindUObject(this, &UTPSAnimInstance::BlendOutShot);
		Montage_SetBlendingOutDelegate(BlendOutDele);
	}
}

void UTPSAnimInstance::PlayEquip(EWeaponType weapontype)
{
	auto pMontage = GetMontage(EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::EQUIP);
	if (!Montage_IsPlaying(pMontage))
	{
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

void UTPSAnimInstance::PlaySprint(EWeaponType weapontype)
{
	auto pMontage = GetMontage(weapontype, EWeaponMontageState::RUN);
	if (!Montage_IsPlaying(pMontage))
	{
		Montage_Play(pMontage, 1.2f, EMontagePlayReturnType::MontageLength, 0.f, true);
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

void UTPSAnimInstance::PlayMelee(EWeaponType weapontype)
{
	auto pMontage = GetMontage(weapontype, EWeaponMontageState::MELEE);
	if (!Montage_IsPlaying(pMontage))
	{
		Montage_Play(pMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
		FOnMontageBlendingOutStarted BlendOutDele;
		BlendOutDele.BindUObject(this, &UTPSAnimInstance::BlendOutMelee);
		Montage_SetBlendingOutDelegate(BlendOutDele);
	}
}

void UTPSAnimInstance::PlayHit()
{
	auto pMontage = GetMontage(EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::HIT);
	if (!Montage_IsPlaying(pMontage))
	{
		Montage_Play(pMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
		FOnMontageBlendingOutStarted BlendOutDele;
		BlendOutDele.BindUObject(this, &UTPSAnimInstance::BlendOutHit);
		Montage_SetBlendingOutDelegate(BlendOutDele);
	}
}

void UTPSAnimInstance::PlayHeal()
{
	auto pMontage = GetMontage(EWeaponType::WEAPON_HANDGUN, EWeaponMontageState::HEAL);
	if (!Montage_IsPlaying(pMontage))
	{
		Montage_Play(pMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
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
	Montage_Stop(0.5f);
}

void UTPSAnimInstance::BlendOutMelee(class UAnimMontage*, bool interrupt)
{
	if (nullptr == pCharacter) return;

	pCharacter->SetAttacking(false);
}

void UTPSAnimInstance::BlendOutHit(class UAnimMontage*, bool interrupt)
{
	if (nullptr == pCharacter) return;
	pCharacter->SetHit(false);
}

void UTPSAnimInstance::AnimNotify_WeaponSet()
{
	if (nullptr == pCharacter) return;
	pCharacter->NotifyEquip();
	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pGameInstance)
	{
		int32 irand = rand() % 3 + 1;
		FString strSoundName = FString::Printf(TEXT("Backpack%i"), irand);
		pGameInstance->StartSoundLocation(*strSoundName, GetWorld(), pCharacter->GetActorLocation(), ESoundAttenuationType::SOUND_SILENCE);
	}
}

void UTPSAnimInstance::AnimNotify_JumpAway()
{
	if (nullptr == pCharacter) return;
	pCharacter->SetJumpAway();
}

void UTPSAnimInstance::AnimNotify_Landing()
{
	if (nullptr == pCharacter) return;
	pCharacter->EvadeComplete();
}

void UTPSAnimInstance::AnimNotify_CrawlEnd()
{
	if (nullptr == pCharacter) return;
	pCharacter->SetCrawlEnd();
	UE_LOG(LogTemp, Log, TEXT("EndCrawl"));
}

void UTPSAnimInstance::AnimNotify_Footstep_L()
{
	if (nullptr == pCharacter || nullptr == pCharacter->GetMesh()) return;

	FVector vLocation = pCharacter->GetMesh()->GetSocketLocation(FName("L_Ankle"));
	int32 irand = rand() % 5 + 1;
	FString strSoundName = FString::Printf(TEXT("FootStep_Ground_%i"), irand);
	FRotator rotDacal = pCharacter->GetActorRotation() - pCharacter->GetMesh()->GetRelativeRotation() - pCharacter->GetFootRotator(true);

	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pGameInstance)
	{
		pGameInstance->SpawnEffect(TEXT("Dust"), GetWorld(), vLocation, pCharacter->GetActorRotation());
		pGameInstance->StartSoundLocation(strSoundName, GetWorld(), vLocation, ESoundAttenuationType::SOUND_SILENCE, 0.2f);
		pGameInstance->SpawnDecal(TEXT("FootStep_L"), GetWorld(), 3.f, vLocation, rotDacal, FVector(15.f));
	}
		
}

void UTPSAnimInstance::AnimNotify_Footstep_R()
{
	if (nullptr == pCharacter || nullptr == pCharacter->GetMesh()) return;

	FVector vLocation = pCharacter->GetMesh()->GetSocketLocation(FName("R_Ankle"));
	int32 irand = rand() % 5 + 1;
	FString strSoundName = FString::Printf(TEXT("FootStep_Ground_%i"), irand);
	FRotator rotDacal = pCharacter->GetActorRotation() - pCharacter->GetMesh()->GetRelativeRotation() - pCharacter->GetFootRotator(true);

	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pGameInstance)
	{
		pGameInstance->SpawnEffect(TEXT("Dust"), GetWorld(), vLocation, pCharacter->GetActorRotation());
		pGameInstance->StartSoundLocation(strSoundName, GetWorld(), vLocation, ESoundAttenuationType::SOUND_SILENCE, 0.2f);
		pGameInstance->SpawnDecal(TEXT("FootStep_L"), GetWorld(), 3.f, vLocation, rotDacal, FVector(15.f));
	}
}

void UTPSAnimInstance::AnimNotify_Swosh()
{
	if (nullptr == pCharacter) return;
	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pGameInstance)
	{
		pGameInstance->StartSoundLocationRandomPitch(TEXT("Swosh"), GetWorld(), pCharacter->GetActorLocation(), ESoundAttenuationType::SOUND_SILENCE, 0.5f);
	}
}

void UTPSAnimInstance::AnimNotify_Healing()
{
	if (nullptr == pCharacter) return;
	pCharacter->HealEnd();
}
