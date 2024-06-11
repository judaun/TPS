// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.generated.h"

class Item;
class Equipment;
class AWeapon;
struct FItemTable;
struct FEquipmentTable;
enum class EItemType : uint8;
enum class EEquipmentType : uint8;

namespace itemkey
{
	int32 const HealBox10 = 1;
	int32 const HealBox30 = 2;
	int32 const HealBox50 = 3;
}

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSPORTFOLIO_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventory();
	~UInventory();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	void InitializeInventory();

public:
	void AddItem(FItemTable* itemdata, int32 itemcnt);
	int32 AddItem(int32 itemkey, int32 itemcnt);
	bool UseItem(FItemTable* itemdata, int32 itemcnt = 1);
	int32 UseItem(int32 itemkey, int32 itemcnt);

	void AddEquip(FEquipmentTable* equipdata);
	AWeapon* LoadWeapon(int32 weaponindex);
	void UnLoadWeapon(FEquipmentTable* equipdata);
private:
	TMap<EItemType, TArray<TSharedPtr<Item>>> mInventory;
	TMap<EEquipmentType, TArray<TSharedPtr<Equipment>>> mEquipInventory;
};
