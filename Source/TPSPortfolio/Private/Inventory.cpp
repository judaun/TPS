// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "TPSDataTable.h"
#include "Inventory.h"

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

bool UInventory::UseItem(FItemTable* itemdata, int32 itemcnt)
{
	auto TA_Item = mInventory.Find(itemdata->ItemType);
	if (nullptr == TA_Item) return false;

	for (auto arrayelem : *TA_Item)
	{
		if (arrayelem->GetItemKey() != itemdata->Itemkey) continue;	
		if (arrayelem->GetCount() < itemcnt) return false;
		
		arrayelem->SubtractCount(itemcnt);
		return true;
	}

	return false;
}

void UInventory::LoadItem()
{

}

void UInventory::FindItem()
{

}

