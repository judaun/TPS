// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Exploder.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API AExploder : public AEnemy
{
	GENERATED_BODY()

	AExploder(const FObjectInitializer& ObjectInitializer);
//function///////////////////////////////////////
private:
protected:
	virtual void BeginPlay() override;
	void InitializeDefaultComponent() override;
	void InitializeMeshComponent() override;
public:
	void SetEnemyData(FEnemyTable* enemydata) override;

	UFUNCTION(BlueprintCallable, Category = Enemy)
	bool GetisAttacking() override { return bIsAttacking; }
//value//////////////////////////////////////////
private:
protected:
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Enemy, meta = (AllowPrivateAccess = "true"))
		class UActorComponent* pFootIK;

	
};
