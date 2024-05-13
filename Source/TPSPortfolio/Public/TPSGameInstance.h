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
class ATPSEffectMng;
class UNiagaraSystem;

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

	/* 사운드 일반 재생 */
	void StartSound(FString soundname, float volume = 1.f, bool isloop = false);
	/* 사운드 월드 위치로 재생 */
	void StartSoundLocation(FString soundname, const UObject* world, FVector location, ESoundAttenuationType atttype, float volume = 1.f, bool isloop = false);
	/* 사운드 월드 위치에서 랜덤피치(높낮이변화)로 재생 */
	void StartSoundLocationRandomPitch(FString soundname, const UObject* world, FVector location, ESoundAttenuationType atttype, float volume = 1.f, bool isloop = false);

	/* 로드된 나이아가라 시스템을 월드로 스폰 */
	void SpawnEffect(FString keyname, const UObject* WorldContextObject, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f), bool bAutoDestroy = true);
	/* 로드된 데칼마테리얼을 월드로 스폰 */
	void SpawnDecal(FString keyname, TObjectPtr<UWorld> WorldContextObject, float lifetime, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f), float fadedistancesize = 0.01f);
	/* 엑터에 Attach 하기 위한 나이아가라 시스템 포인터 반환 */
	UNiagaraSystem* GetEffect(FString keyname);

private:
	UPROPERTY()
	UDataTable* DT_Item;
	UPROPERTY()
	UDataTable* DT_Equipment;

	UPROPERTY()
	ATPSSoundManager* pSoundMng;

	UPROPERTY()
	ATPSEffectMng* pEffectMng;
};
