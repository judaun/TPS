// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

UENUM(BlueprintType)

enum class EItemType : uint8
{ CONSUMABLE_ITEM, VARUABLE_ITEM, MATERIAL_ITEM, EQUIPABLE_ITEM };

UENUM(BlueprintType)

enum class EItemEffType : uint8
{
	STAT_HP, STAT_ATT, STAT_SPD, VAL_GOLD, MAT_EXP, MAT_GRADE
};

UENUM(BlueprintType)

enum class EEquipmentType : uint8
{
	EQUIP_MAIN_WEAPON, EQUIP_SUB_WEAPON, EQUIP_ARMOR, EQUIP_ASSISTANCE,
};

UENUM(BlueprintType)

enum class EWeaponType : uint8
{
	WEAPON_HANDGUN, WEAPON_RIFLE, WEAPON_SHOTGUN, WEAPON_MELEE, WEAPON_THROW, WEAPON_NONE
};

UENUM(BlueprintType)

enum class EProjectileType : uint8
{
	PROJECTILE_TRACE, PROJECTILE_PROJECTILE, PROJECTILE_NONE,
};

UENUM(BlueprintType)

enum class ESoundAttenuationType : uint8
{
	SOUND_LOUD, SOUND_MEDIUM, SOUND_SILENCE, SOUND_2D,
};

UENUM(BlueprintType)
enum class ECharacterState : uint8 { IDLE, RUN, SPRINT, BRAKE, AIM, EVADE };

UENUM(BlueprintType)
enum class EEnemyState : uint8
{ 
	SPAWN, IDLE, MOVE, ATTACK, DEAD, 
};

UENUM(BlueprintType)
enum class EWeaponMontageState : uint8 
{ 
	IDLE, RUN, AIM, SHOT, RELOAD, EQUIP, MELEE, HIT, HEAL,
};

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	ENEMY_ASSAULT, ENEMY_TANK, ENEMY_SCOUT,
};