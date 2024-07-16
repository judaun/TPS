// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSDataTable.h"

/**
 * 
 */
class TPSPORTFOLIO_API Equipment
{
public:
	Equipment();
	Equipment(FEquipmentTable* equipdata);
	~Equipment();

public:
	int32 GetItemKey() { return FEquipData.Itemkey; }
	void SetData(FEquipmentTable* equipdata);
	void SetData(FEquipmentTable equipdata);
	FEquipmentTable GetData() { return FEquipData; }
protected:
	FEquipmentTable FEquipData;
};
