// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSDataTable.h"
#include "TPSEnum.h"
#include "Engine/GameInstance.h"
#include "TPSGameInstance.generated.h"

/**
 * 
 */

UCLASS()
class TPSPORTFOLIO_API UTPSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UTPSGameInstance();
	~UTPSGameInstance();

private:
	void Initialize_DataTable();
	void LoadDataTable_Factory(FString tablename, UDataTable** ptable);

public:
	FItemTable* GetItemData(int32 key);
	TArray<FItemTable*> GetItemData_ALL();
	FEquipmentTable* GetEquipmentData(int32 key);
	TArray<FEquipmentTable*> GetEquipmentData_ALL();

private:
	UPROPERTY()
	UDataTable* DT_Item;
	UPROPERTY()
	UDataTable* DT_Equipment;
};
