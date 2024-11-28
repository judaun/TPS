// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TPSDataTable.h"

/**
 * 
 */

class TPSPORTFOLIO_API UPlayerSkill 
{
public:
	UPlayerSkill();
	UPlayerSkill(FSkillTable* skilldata);
	~UPlayerSkill();
public:
	void SetSkill(FSkillTable* skilldata);
	void SetSkill(FSkillTable skilldata);
	FSkillTable GetSkillData() {return fSkillTable;}
	int32 GetSkillKey() {return fSkillTable.Skillkey;}
protected:
	FSkillTable fSkillTable;

};
