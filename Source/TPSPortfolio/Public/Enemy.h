// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSDataTable.h"
#include "Enemy.generated.h"


class UEnemyState;
enum class EEnemyState : uint8;

UCLASS()
class TPSPORTFOLIO_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy(const FObjectInitializer& ObjectInitializer);

//function/////////////////////////////////////////////////
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
private:
	template< class T >
	void InitializeState(FString compname);
protected:
	virtual void InitializeDefaultComponent();
	virtual void InitializeMeshComponent();
	
public:
	virtual void SetEnemyData(FEnemyTable* enemydata);
	FEnemyTable GetEnemyData() { return EnemyData;}

	virtual void BreakBone();

	/* Enemy�� ���� �� ȸ�������� ���� Ÿ�� ���� ���� */
	void SetTargetActor(AActor* actor);
	/* Ÿ�� ��ġ ��ȯ */
	FVector GetTargetActorPos();
	/* ���� ����� Ÿ�� ���� �ʱ�ȭ */
	void ClearTarget();
	/* ��ü ȸ������ ���� */
	void LockOnTarget(float deltatime);
	/* EnemyState ���� */
	void ChangeState(EEnemyState changestate);
	/* EnemyState ��� ���� */
	void UpdateState(float deltatime);

	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual bool GetisAttacking() { return bIsAttacking; }
	virtual void SetisAttacking(bool attacking);
//value/////////////////////////////////////////////////////
private:
	UPROPERTY()
	TArray<TObjectPtr<class UEnemyState>> TA_State;
	TWeakObjectPtr<class UEnemyState> wpCurState;
protected:
	UPROPERTY()
	TWeakObjectPtr<AActor> wpTargetActor;

	FEnemyTable EnemyData;
	int32 iCurHealth;
	bool bIsAttacking;
};
