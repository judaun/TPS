// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "GameFramework/Actor.h"
#include "AWeapon.generated.h"

enum class EWeaponType : uint8;
class ATPSPortfolioCharacter;

UCLASS()
class TPSPORTFOLIO_API AWeapon : public AActor , public Equipment
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	AWeapon(FEquipmentTable* equipdata);

private:
	void InitializeMesh(FString weaponaddress);
	FString GetWeaponTypeName(EWeaponType weapontype);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	void SetPlayer(ATPSPortfolioCharacter* character);
	void DeferredInitialize(FEquipmentTable* equipdata);
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void AttackTrace();
	void Reload();
	void MakeShell();
	void AttackStart();
	void AttackStop();
	float GetBulletrate();
	bool IsFullCapacity();
	bool IsPosibleReload();
private:
	USkeletalMeshComponent* pMesh;
	FCollisionQueryParams collisionParams;
	FTimerHandle Firetimehandle;
	TWeakObjectPtr<ATPSPortfolioCharacter> pCharacter;
	int32 iCurrentCapacity;
	int32 iCurrentMagazine;
};
