// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory.h"
#include "TPSGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "TPSDataTable.h"
#include "Item.h"
#include "TPSEnum.h"
#include "Equipment.h"
#include "AWeapon.h"
#include "PlayerSkill.h"
#include "TPSPortfolioCharacter.h"
#include "Stratagem.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	UE_LOG(LogTemp, Log, TEXT("Inventory Set"));
	// ...
}


UInventory::~UInventory()
{
	for (auto map_elem : mInventory)
	{
		for (auto vec_elem : map_elem.Value)
		{
			vec_elem.Reset();
		}
	}
	mInventory.Reset();

	for (auto map_elem : mEquipInventory)
	{
		for (auto vec_elem : map_elem.Value)
		{
			vec_elem.Reset();
		}
	}
	mEquipInventory.Reset();

	for (auto map_elem : mSkillInventory)
	{
		for (auto vec_elem : map_elem.Value)
		{
			vec_elem.Reset();
		}
	}
	mSkillInventory.Reset();
}

// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...
	InitializeInventory();
}


// Called every frame
void UInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventory::InitializeInventory()
{
	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (nullptr == pGameInstance)
		return;

	TArray<FItemTable*> Itemdata = pGameInstance->GetItemData_ALL();
	for (auto vec_elem : Itemdata)
	{
		AddItem(vec_elem,0);
	}

	TArray<FEquipmentTable*> Equipdata = pGameInstance->GetEquipmentData_ALL();
	for (auto vec_elem : Equipdata)
	{
		AddEquip(vec_elem);
	}

	TArray<FSkillTable*> Skilldata = pGameInstance->GetSkill_ALL();
	for (auto vec_elem : Skilldata)
	{
		AddSkill(vec_elem);
	}
}

void UInventory::AddItem(FItemTable* itemdata, int32 itemcnt)
{
	auto TA_Item = mInventory.Find(itemdata->ItemType);
	if (nullptr == TA_Item)
	{
		//Inventory에 TArray 구축 전
		TArray<TSharedPtr<Item>> newArray;
		newArray.Emplace(MakeShared<Item>(itemdata, itemcnt));
		mInventory.Add(itemdata->ItemType, newArray);
	}
	else
	{
		//Inventory에 TArray가 구축되어있음

		for (auto arrayelem : *TA_Item)
		{
			if (arrayelem->GetItemKey() == itemdata->Itemkey)
			{
				//Inventory에 해당 아이템이 있음
				arrayelem->AddCount(itemcnt);
				return;
			}
		}

		//Inventory에 ItemKey에 해당하는 아이템이 없음(모든 아이템이 갯수0으로 세팅됨)
		TA_Item->Emplace(MakeShared<Item>(itemdata, itemcnt));
	}
}

int32 UInventory::AddItem(int32 itemkey, int32 itemcnt)
{
	for (auto& map_elem : mInventory)
	{
		for (auto& array_elem : map_elem.Value)
		{
			if (array_elem->GetItemKey() == itemkey)
			{
				array_elem->AddCount(itemcnt);
				return array_elem->GetCount();
			}
		}
	}
	return -1;
}

bool UInventory::UseItem(FItemTable* itemdata, int32 itemcnt)
{
	auto TA_Item = mInventory.Find(itemdata->ItemType);
	if (nullptr == TA_Item) return false;
	if(!IsValid(GetOwner())) return false;

	for (auto arrayelem : *TA_Item)
	{
		if (arrayelem->GetItemKey() != itemdata->Itemkey) continue;	
		if (arrayelem->GetCount() < itemcnt) return false;
		
		Cast<ATPSPortfolioCharacter>(GetOwner())->SetEffectItem(itemdata->EffectType, itemdata->PercentageValue, itemdata->Value);
		arrayelem->SubtractCount(itemcnt);

		return true;
	}

	return false;
}


int32 UInventory::UseItem(int32 itemkey, int32 itemcnt)
{
	if (!IsValid(GetOwner())) return -1;
	for (auto& map_elem : mInventory)
	{
		for (auto& array_elem : map_elem.Value)
		{
			if (array_elem->GetItemKey() == itemkey)
			{
				
				FItemTable itemData = array_elem->GetItemData();
				Cast<ATPSPortfolioCharacter>(GetOwner())->SetEffectItem(itemData.EffectType, itemData.PercentageValue, itemData.Value);
				array_elem->SubtractCount(itemcnt);

				return array_elem->GetCount();
			}
		}
	}

	return -1;
}

void UInventory::AddEquip(FEquipmentTable* equipdata)
{
	auto TA_Equip = mEquipInventory.Find(equipdata->EquipType);
	if (nullptr == TA_Equip)
	{
		//Inventory에 TArray 구축 전
		TArray<TSharedPtr<Equipment>> newArray;
		newArray.Emplace(MakeShared<Equipment>(equipdata));
		mEquipInventory.Add(equipdata->EquipType, newArray);
	}
	else
	{
		TA_Equip->Emplace(MakeShared<Equipment>(equipdata));
	}
}

AWeapon* UInventory::LoadWeapon(int32 weaponindex, bool issub)
{
	auto TA_Equip = mEquipInventory.Find(issub ? EEquipmentType::EQUIP_SUB_WEAPON : EEquipmentType::EQUIP_MAIN_WEAPON);
	if (nullptr == TA_Equip) return nullptr;
	
	for (auto& elem_Array : *TA_Equip)
	{
		if (elem_Array->GetItemKey() == weaponindex)
		{
			FTransform SpawnTransform(FRotator::ZeroRotator, FVector::ZeroVector);
			auto pWeapon = GetWorld()->SpawnActorDeferred<AWeapon>(AWeapon::StaticClass(), SpawnTransform);
			if (pWeapon)
			{
				pWeapon->DeferredInitialize(elem_Array->GetData(), issub);
				pWeapon->FinishSpawning(SpawnTransform);
			}

			return pWeapon;
		}
	}
	return nullptr;
}

void UInventory::UnLoadWeapon(FEquipmentTable* equipdata)
{

}

void UInventory::AddSkill(FSkillTable* skilldata)
{
	auto TA_Skill = mSkillInventory.Find(ESkillType::SKILL_ORBIT);
	if (nullptr == TA_Skill)
	{
		TArray<TSharedPtr<UPlayerSkill>> newArr;
		newArr.Emplace(MakeShared<UPlayerSkill>(skilldata));
		mSkillInventory.Emplace(ESkillType::SKILL_ORBIT, newArr);

	}
	else
	{
		TA_Skill->Emplace(MakeShared<UPlayerSkill>(skilldata));
	}
}

UPlayerSkill* UInventory::LoadSkill(int32 skillindex)
{
	auto TA_Skill = mSkillInventory.Find(ESkillType::SKILL_ORBIT);
	if(nullptr == TA_Skill) return nullptr;

	for (auto& elem_Arr : *TA_Skill)
	{
		if (elem_Arr->GetSkillKey() == skillindex)
		{
			return elem_Arr.Get();
		}
	}
	return nullptr;
}

AStratagem* UInventory::LoadStratagem(int32 skillidx)
{
	auto TA_Skill = mSkillInventory.Find(ESkillType::SKILL_ORBIT);
	if (nullptr == TA_Skill) return nullptr;

	for (auto& elem_Arr : *TA_Skill)
	{
		if (elem_Arr->GetSkillKey() == skillidx)
		{
			FTransform SpawnTransform(FRotator::ZeroRotator, FVector::ZeroVector);
			auto pStratagem = GetWorld()->SpawnActorDeferred<AStratagem>(AStratagem::StaticClass(), SpawnTransform);
			if (pStratagem)
			{
				pStratagem->SetData(elem_Arr->GetSkillData());
				pStratagem->DeferredInitialize();
				pStratagem->FinishSpawning(SpawnTransform);
			}

			return pStratagem;
		}
	}

	return nullptr;
}

