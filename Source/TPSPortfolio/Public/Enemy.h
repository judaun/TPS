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
	void Dissolving(float culvevalue);
protected:
	virtual void InitializeDefaultComponent();
	virtual void InitializeMeshComponent();
	
public:
	virtual void SetEnemyData(FEnemyTable* enemydata);
	FEnemyTable GetEnemyData() { return EnemyData;}
	virtual void BreakBone();
	void DissolveMaterial();
	void DmgCapsuleActive(bool isactive, float timer=0.f);

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
	
	EEnemyState GetEnemyState();
	void SetSquadPos(FVector squadpos);
	void SetSquad(AEnemy* squadleader);
	bool HasSquad() {return nullptr == SquadLeader ? false:true;}

	virtual void ActiveEffect(FVector direction){}
	int32 GetDmg() {
		return EnemyData.Damage;}
	int32 GetHP() {return iCurHealth;}

	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual bool GetisAttacking() { return bIsAttacking; }
	virtual void SetisAttacking(bool attacking);

	UFUNCTION(BlueprintCallable, Category = Enemy)
	FVector GetSquadLocation() {return vSquadPos;}

//value/////////////////////////////////////////////////////
private:
	UPROPERTY()
	TArray<TObjectPtr<class UEnemyState>> TA_State;
	TWeakObjectPtr<class UEnemyState> wpCurState;

	UPROPERTY()
	UMaterialInstanceDynamic* pM_Dynamic;
	float fDissolve;
	bool bIsDissolve;
	FTimerHandle dissolvetimehandle;

	FTimerHandle FDmgcapsuletimehandle;

	FVector vSquadPos;

	UPROPERTY()
	TWeakObjectPtr<AEnemy> SquadLeader;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Minimap, meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* MinimapSprite;

	UPROPERTY()
		class UPaperSprite* pMarkSprite;
protected:
	UPROPERTY()
	TWeakObjectPtr<AActor> wpTargetActor;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> DamageCapsuleComponent;

	FEnemyTable EnemyData;
	int32 iCurHealth;
	bool bIsAttacking;
};
