// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

UENUM(BlueprintType)

enum class ItemType : uint8
{ CONSUMABLE_ITEM, VARUABLE_ITEM, MATERIAL_ITEM, EQUIPABLE_ITEM };

UENUM(BlueprintType)

enum class ItemEffType : uint8
{
	STAT_HP, STAT_ATT, STAT_SPD, VAL_GOLD, MAT_EXP, MAT_GRADE
};