// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGameInstance.h"
#include "TPSSoundManager.h"
#include "TPSEnemyMng.h"
#include "TPSEffectMng.h"
#include "NiagaraFunctionLibrary.h"
#include "TPSEnum.h"

UTPSGameInstance::UTPSGameInstance()
{
	Initialize_DataTable();
	pSoundMng = CreateDefaultSubobject<ATPSSoundManager>(TEXT("SoundManager"));
	pEffectMng = CreateDefaultSubobject<ATPSEffectMng>(TEXT("EffectManager"));
	pEnemyMng = CreateDefaultSubobject<ATPSEnemyMng>(TEXT("EnemyManager"));
	srand((uint32)time(NULL));
}

UTPSGameInstance::~UTPSGameInstance()
{
}

void UTPSGameInstance::Initialize_DataTable()
{
	LoadDataTable_Factory(TEXT("DataTable'/Game/Table/DT_Item.DT_Item'"), &DT_Item);
	LoadDataTable_Factory(TEXT("DataTable'/Game/Table/DT_Equipment.DT_Equipment'"), &DT_Equipment);
}

void UTPSGameInstance::LoadDataTable_Factory(FString tablename, UDataTable** ptable)
{
	///Script/Engine.DataTable'/Game/Table/DT_Item.DT_Item'
	ConstructorHelpers::FObjectFinder<UDataTable> DT_OBJ(*tablename);
	if (DT_OBJ.Succeeded())
	{
		*ptable = DT_OBJ.Object;
		UE_LOG(LogTemp, Log, TEXT("%s Table Load Success"), *tablename);
	}
}

FItemTable* UTPSGameInstance::GetItemData(int32 key)
{
	if (!IsValid(DT_Item)) return nullptr;

	return DT_Item->FindRow<FItemTable>(*FString::FromInt(key), TEXT(""));
}

TArray<FItemTable*> UTPSGameInstance::GetItemData_ALL()
{
	TArray<FItemTable*> TA_ItemData;
	DT_Item->GetAllRows(TEXT(""), TA_ItemData);
	return TA_ItemData;
}

FEquipmentTable* UTPSGameInstance::GetEquipmentData(int32 key)
{
	if (!IsValid(DT_Equipment)) return nullptr;

	return DT_Equipment->FindRow<FEquipmentTable>(*FString::FromInt(key), TEXT(""));
}

TArray<FEquipmentTable*> UTPSGameInstance::GetEquipmentData_ALL()
{
	TArray<FEquipmentTable*> TA_EquipData;
	DT_Equipment->GetAllRows(TEXT(""), TA_EquipData);
	return TA_EquipData;
}

void UTPSGameInstance::StartSound(FString soundname, float volume, bool isloop /*= false*/)
{
	if (nullptr == pSoundMng) return;

	pSoundMng->StartSound(*soundname, volume, isloop);
}

void UTPSGameInstance::StartSoundLocation(FString soundname, const UObject* world, FVector location, ESoundAttenuationType atttype, float volume, bool isloop /*= false*/)
{
	if (nullptr == pSoundMng) return;

	pSoundMng->StartSoundLocation(*soundname, world, location, atttype, volume, isloop);

}

void UTPSGameInstance::StartSoundLocationRandomPitch(FString soundname, const UObject* world, FVector location, ESoundAttenuationType atttype, float volume /*= 1.f*/, bool isloop /*= false*/)
{
	if (nullptr == pSoundMng) return;

	pSoundMng->StartSoundLocationRandomPitch(*soundname, world, location, atttype, volume, isloop);

}

void UTPSGameInstance::SpawnEffect(FString keyname, const UObject* WorldContextObject, FVector Location, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Scale /*= FVector(1.f)*/, bool bAutoDestroy /*= true*/)
{
	if (nullptr == pEffectMng) return;
	pEffectMng->SpawnEffect(keyname, WorldContextObject, Location, Rotation, Scale, bAutoDestroy);
}

void UTPSGameInstance::SpawnDecal(FString keyname, TObjectPtr<UWorld> WorldContextObject, float lifetime, FVector Location, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Scale /*= FVector(1.f)*/, float fadedistancesize /*= 0.01f*/)
{
	if (nullptr == pEffectMng) return;
	pEffectMng->SpawnDecal(keyname, WorldContextObject, lifetime, Location, Rotation, Scale, fadedistancesize);
}

void UTPSGameInstance::SpawnEnemy(int32 key, const UWorld* world, FVector location, FRotator rotator)
{
	if(nullptr == pEnemyMng) return;
	pEnemyMng->SpawnEnemy(key,world,location,rotator);
}

UNiagaraSystem* UTPSGameInstance::GetEffect(FString keyname)
{
	if (nullptr == pEffectMng) return nullptr;
	return pEffectMng->GetEffect(keyname);
}
