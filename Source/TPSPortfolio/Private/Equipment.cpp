// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment.h"

Equipment::Equipment(FEquipmentTable* equipdata)
{
	FEquipData.Itemkey = equipdata->Itemkey;
	FEquipData.Name = equipdata->Name;
	FEquipData.EquipType = equipdata->EquipType;
	FEquipData.WeaponType = equipdata->WeaponType;
	FEquipData.ProjectileType = equipdata->ProjectileType;
	FEquipData.iBaseDmg = equipdata->iBaseDmg;
	FEquipData.fBaseAttInterval = equipdata->fBaseAttInterval;
	FEquipData.fBaseAccuracy = equipdata->fBaseAccuracy;
	FEquipData.fBaseRange = equipdata->fBaseRange;
	FEquipData.iBaseCapacity = equipdata->iBaseCapacity;
	FEquipData.iBaseMagazine = equipdata->iBaseMagazine;
	FEquipData.fBaseReloadTime = equipdata->fBaseReloadTime;
}

Equipment::Equipment()
{

}

Equipment::~Equipment()
{
}

void Equipment::SetData(FEquipmentTable* equipdata)
{
	FEquipData.Itemkey = equipdata->Itemkey;
	FEquipData.Name = equipdata->Name;
	FEquipData.EquipType = equipdata->EquipType;
	FEquipData.WeaponType = equipdata->WeaponType;
	FEquipData.ProjectileType = equipdata->ProjectileType;
	FEquipData.iBaseDmg = equipdata->iBaseDmg;
	FEquipData.fBaseAttInterval = equipdata->fBaseAttInterval;
	FEquipData.fBaseAccuracy = equipdata->fBaseAccuracy;
	FEquipData.fBaseRange = equipdata->fBaseRange;
	FEquipData.iBaseCapacity = equipdata->iBaseCapacity;
	FEquipData.iBaseMagazine = equipdata->iBaseMagazine;
	FEquipData.fBaseReloadTime = equipdata->fBaseReloadTime;
}

void Equipment::SetData(FEquipmentTable equipdata)
{
	FEquipData.Itemkey = equipdata.Itemkey;
	FEquipData.Name = equipdata.Name;
	FEquipData.EquipType = equipdata.EquipType;
	FEquipData.WeaponType = equipdata.WeaponType;
	FEquipData.ProjectileType = equipdata.ProjectileType;
	FEquipData.iBaseDmg = equipdata.iBaseDmg;
	FEquipData.fBaseAttInterval = equipdata.fBaseAttInterval;
	FEquipData.fBaseAccuracy = equipdata.fBaseAccuracy;
	FEquipData.fBaseRange = equipdata.fBaseRange;
	FEquipData.iBaseCapacity = equipdata.iBaseCapacity;
	FEquipData.iBaseMagazine = equipdata.iBaseMagazine;
	FEquipData.fBaseReloadTime = equipdata.fBaseReloadTime;
}