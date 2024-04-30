// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

Item::Item(FItemTable* itemdata)
{
	fItemData.Itemkey = itemdata->Itemkey;
	fItemData.Name = itemdata->Name;
	fItemData.ItemType = itemdata->ItemType;
	fItemData.EffectType = itemdata->EffectType;
	fItemData.Value = itemdata->Value;
	fItemData.PercentageValue = itemdata->PercentageValue;
	iItemCnt = 0;
}

Item::Item(FItemTable* itemdata, int32 itemcnt)
{
	fItemData.Itemkey = itemdata->Itemkey;
	fItemData.Name = itemdata->Name;
	fItemData.ItemType = itemdata->ItemType;
	fItemData.EffectType = itemdata->EffectType;
	fItemData.Value = itemdata->Value;
	fItemData.PercentageValue = itemdata->PercentageValue;
	iItemCnt = itemcnt;
}

Item::~Item()
{
}
