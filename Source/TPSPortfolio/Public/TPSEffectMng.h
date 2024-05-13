// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSEffectMng.generated.h"

class UNiagaraSystem;

UCLASS()
class TPSPORTFOLIO_API ATPSEffectMng : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSEffectMng();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void AddEffect(FString keyname, FString effectaddress);
	void AddDecal(FString keyname, FString decaladdress);
	void InitEffectManager();

public:
	/* 로드된 나이아가라 시스템을 월드로 스폰 */
	void SpawnEffect(FString keyname, const UObject* WorldContextObject, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f), bool bAutoDestroy = true);
	/* 로드된 데칼마테리얼을 월드로 스폰 */
	void SpawnDecal(FString keyname, TObjectPtr<UWorld> WorldContextObject, float lifetime, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f), float fadedistancesize = 0.01f);
	/* 엑터에 Attach 하기 위한 나이아가라 시스템 포인터 반환 */
	UNiagaraSystem* GetEffect(FString keyname);

private:
	UPROPERTY()
	TMap<FString, UNiagaraSystem*> mapEffect;
	UPROPERTY()
	TMap<FString, UMaterial*> mapDecalMaterial;
};
