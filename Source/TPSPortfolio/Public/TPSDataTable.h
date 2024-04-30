// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TPSEnum.h"
#include "TPSDataTable.generated.h"
/**
 * 
 */
USTRUCT()
struct FItemTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 Itemkey;

	UPROPERTY(EditAnywhere)
	FString Name;

	UPROPERTY(EditAnywhere)
	ItemType ItemType;

	UPROPERTY(EditAnywhere)
	ItemEffType EffectType;

	UPROPERTY(EditAnywhere)
	int32 Value;

	UPROPERTY(EditAnywhere)
	float PercentageValue;

};

class TPSPORTFOLIO_API TPSDataTable
{
public:
	TPSDataTable();
	~TPSDataTable();


};
