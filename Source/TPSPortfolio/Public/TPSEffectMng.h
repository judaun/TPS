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
	/* �ε�� ���̾ư��� �ý����� ����� ���� */
	void SpawnEffect(FString keyname, const UObject* WorldContextObject, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f), bool bAutoDestroy = true);
	/* �ε�� ��Į���׸����� ����� ���� */
	void SpawnDecal(FString keyname, TObjectPtr<UWorld> WorldContextObject, float lifetime, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f), float fadedistancesize = 0.01f);
	/* ���Ϳ� Attach �ϱ� ���� ���̾ư��� �ý��� ������ ��ȯ */
	UNiagaraSystem* GetEffect(FString keyname);

private:
	UPROPERTY()
	TMap<FString, UNiagaraSystem*> mapEffect;
	UPROPERTY()
	TMap<FString, UMaterial*> mapDecalMaterial;
};
