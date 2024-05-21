// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackEnemyState.h"
#include "TPSEnum.h"
#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"

UAttackEnemyState::UAttackEnemyState()
{
	eStatetype = EEnemyState::ATTACK;
}

void UAttackEnemyState::Turn(float deltatime)
{
	if (!IsValid(GetOwner())) return;

	Cast<AEnemy>(GetOwner())->LockOnTarget(deltatime);
}

void UAttackEnemyState::Attack(float deltatime)
{
	if (!IsValid(GetOwner())) return;

	AEnemy* pEnemy = Cast<AEnemy>(GetOwner());
	FEnemyTable FData = pEnemy->GetEnemyData();

	//Ÿ�� ���� ��ġ ��� �� ���������� �񱳸� ���� ���� ȹ��
	FVector vTargetLocation = pEnemy->GetTargetActorPos();
	FVector vTargetMoveDirection = vTargetLocation - vTargetSrcPos;
	vTargetSrcPos = vTargetLocation;

	//���� �����ӷ���Ʈ
	double framerate = double(0.25f) / deltatime;
	//���� ���������κ����� �̵��Ÿ� * ������ ���� = 1�ʵ��� �̵��� �Ÿ� ����
	vTargetForeseeLoctaion = vTargetLocation + (vTargetMoveDirection*framerate);
	
	float fDist = (vTargetLocation - GetOwner()->GetActorLocation()).Length();
	if (fDist <= FData.DetectLength)
	{
		bool isLongRange = FData.ProjectileDamage > 0.f;
		if (!pEnemy->GetWorldTimerManager().IsTimerActive(Firetimehandle))
		{
			pEnemy->GetWorldTimerManager().SetTimer(Firetimehandle, 
			[this, isLongRange]() 
				{ isLongRange ? LongRangeAttack() : MeleeAttack();},
				pEnemy->GetEnemyData().fAttInterval / 1000.f, true, 0.f);
		}
	}
	else
		pEnemy->SetisAttacking(false);
}

void UAttackEnemyState::MeleeAttack()
{
	if (!IsValid(GetOwner())) return;
	AEnemy* pEnemy = Cast<AEnemy>(GetOwner());
	pEnemy->SetisAttacking(true);
	UE_LOG(LogTemp, Log, TEXT("MeleeAttack"));
	FVector vDirection = (vTargetForeseeLoctaion - pEnemy->GetActorLocation()).GetSafeNormal();
	vDirection.Z += 0.5f;
	pEnemy->GetCharacterMovement()->AddImpulse(vDirection* 800.f, true);

	DrawDebugBox(GetWorld(), vTargetForeseeLoctaion, FVector(5.f), FColor::Red,false,3.f);
}

void UAttackEnemyState::LongRangeAttack()
{
	
}

void UAttackEnemyState::Process(float deltatime)
{
	Turn(deltatime);
	Attack(deltatime);
}

void UAttackEnemyState::Enter()
{

}

void UAttackEnemyState::Exit()
{
	if (!IsValid(GetOwner())) return;
	if (Firetimehandle.IsValid())
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(Firetimehandle);
	}
	AEnemy* pEnemy = Cast<AEnemy>(GetOwner());
	pEnemy->SetisAttacking(false);
}
