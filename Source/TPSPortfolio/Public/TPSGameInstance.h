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

	/* ���� �Ϲ� ��� */
	void StartSound(FString soundname, float volume = 1.f, bool isloop = false);
	/* ���� ���� ��ġ�� ��� */
	void StartSoundLocation(FString soundname, const UObject* world, FVector location, ESoundAttenuationType atttype, float volume = 1.f, bool isloop = false);
	/* ���� ���� ��ġ���� ������ġ(�����̺�ȭ)�� ��� */
	void StartSoundLocationRandomPitch(FString soundname, const UObject* world, FVector location, ESoundAttenuationType atttype, float volume = 1.f, bool isloop = false);

	/* �ε�� ���̾ư��� �ý����� ����� ���� */
	void SpawnEffect(FString keyname, const UObject* WorldContextObject, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f), bool bAutoDestroy = true);
	/* �ε�� ��Į���׸����� ����� ���� */
	void SpawnDecal(FString keyname, TObjectPtr<UWorld> WorldContextObject, float lifetime, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f), float fadedistancesize = 0.01f);
	/* ���Ϳ� Attach �ϱ� ���� ���̾ư��� �ý��� ������ ��ȯ */
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
