// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSEnum.h"
#include "TPSDataTable.h"
/**
 * 
 */
class TPSPORTFOLIO_API Item
{
public:
	Item(FItemTable* itemdata);
	Item(FItemTable* itemdata, int32 itemcnt);
	~Item();
	
public:
	void SetCount(int32 cnt) { iItemCnt = cnt; }
	void AddCount(int32 cnt) { iItemCnt += cnt; }
	void SubtractCount(int32 cnt) { iItemCnt -= cnt; }
	int32 GetCount() { return iItemCnt; }
	int32 GetItemKey() { return fItemData.Itemkey; }
	FItemTable GetItemData() { return fItemData; }
private:
	FItemTable fItemData;
	int32 iItemCnt;
};
