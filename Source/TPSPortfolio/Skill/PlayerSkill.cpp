// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/PlayerSkill.h"

UPlayerSkill::UPlayerSkill()
{

}

UPlayerSkill::UPlayerSkill(FSkillTable* skilldata)
{
	SetSkill(skilldata);
}

UPlayerSkill::~UPlayerSkill()
{

}

void UPlayerSkill::SetSkill(FSkillTable* skilldata)
{
	fSkillTable.Command = skilldata->Command;
	fSkillTable.Cooldown = skilldata->Cooldown;
	fSkillTable.Countdown = skilldata->Countdown;
	fSkillTable.Damage = skilldata->Damage;
	fSkillTable.Delay = skilldata->Delay;
	fSkillTable.EffectRadius = skilldata->EffectRadius;
	fSkillTable.EffectTime = skilldata->EffectTime;
	fSkillTable.Name = skilldata->Name;
	fSkillTable.Radius = skilldata->Radius;
	fSkillTable.Skillkey = skilldata->Skillkey;
}

void UPlayerSkill::SetSkill(FSkillTable skilldata)
{
	fSkillTable.Command = skilldata.Command;
	fSkillTable.Cooldown = skilldata.Cooldown;
	fSkillTable.Countdown = skilldata.Countdown;
	fSkillTable.Damage = skilldata.Damage;
	fSkillTable.Delay = skilldata.Delay;
	fSkillTable.EffectRadius = skilldata.EffectRadius;
	fSkillTable.EffectTime = skilldata.EffectTime;
	fSkillTable.Name = skilldata.Name;
	fSkillTable.Radius = skilldata.Radius;
	fSkillTable.Skillkey = skilldata.Skillkey;
}
