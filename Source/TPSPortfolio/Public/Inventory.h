// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "Equipment.h"
#include "TPSEnum.h"
#include "Inventory.generated.h"

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
	bool UseItem(FItemTable* itemdata, int32 itemcnt = 1);

	void LoadItem();
	void FindItem();

private:
	TMap<ItemType, TArray<TSharedPtr<Item>>> mInventory;
};
