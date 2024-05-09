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
class ATPSSoundManager;

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

	void StartSound(FString soundname, float volume = 1.f, bool isloop = false);
	void StartSoundLocation(FString soundname, const UObject* world, FVector location, ESoundAttenuationType atttype, float volume = 1.f, bool isloop = false);
	void StartSoundLocationRandomPitch(FString soundname, const UObject* world, FVector location, ESoundAttenuationType atttype, float volume = 1.f, bool isloop = false);

private:
	UPROPERTY()
	UDataTable* DT_Item;
	UPROPERTY()
	UDataTable* DT_Equipment;

	UPROPERTY()
	ATPSSoundManager* pSoundMng;
};
