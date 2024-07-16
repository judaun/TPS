// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FieldItemSpawner.generated.h"

UCLASS()
class TPSPORTFOLIO_API AFieldItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFieldItemSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
	int32 itemidx;

	UPROPERTY(EditAnywhere)
	int32 itemCnt;

	UPROPERTY(EditAnywhere)
	bool isEquip;
};
