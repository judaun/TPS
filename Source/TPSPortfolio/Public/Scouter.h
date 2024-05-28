// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Scouter.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API AScouter : public AEnemy
{
	GENERATED_BODY()
	
	AScouter(const FObjectInitializer& ObjectInitializer);
//function///////////////////////////////////////////////
private:
	void ServoMotorSound();
protected:
	virtual void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void InitializeDefaultComponent() override;
	void InitializeMeshComponent() override;
public:
	void SetEnemyData(FEnemyTable* enemydata) override;
	void BreakBone() override;
	void ActiveEffect(FVector direction) override;

	UFUNCTION(BlueprintCallable, Category = Enemy)
		bool GetisAttacking() override { return bIsAttacking; }
//value//////////////////////////////////////////////////
private:
	FTimerHandle Soundtimehandle;
protected:
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Enemy,meta= (AllowPrivateAccess = "true"))
	class UActorComponent* pFootIK;

	UPROPERTY(EditAnywhere, Category = "Firing")
	class UNiagaraComponent* pNiagaraCom;

	UPROPERTY()
	UClass* pProjectileClass;
};
