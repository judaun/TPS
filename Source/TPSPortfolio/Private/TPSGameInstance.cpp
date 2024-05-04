// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGameInstance.h"

UTPSGameInstance::UTPSGameInstance()
{
	Initialize_DataTable();
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
