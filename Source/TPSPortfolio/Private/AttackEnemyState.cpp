// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackEnemyState.h"
#include "TPSEnum.h"
#include "Enemy.h"
#include "TPSGameInstance.h"
#include "TPSSoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TPSPortfolioCharacter.h"

UAttackEnemyState::UAttackEnemyState()
{
	eStatetype = EEnemyState::ATTACK;
	bIsTraced = false;
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

	//타겟 현재 위치 기록 및 이전프레임 비교를 통한 방향 획득
	FVector vTargetLocation = pEnemy->GetTargetActorPos();
	FVector vTargetMoveDirection = vTargetLocation - vTargetSrcPos;
	vTargetSrcPos = vTargetLocation;

	//현재 프레임레이트
	double framerate = double(0.25f) / deltatime;
	//이전 프레임으로부터이 이동거리 * 프레임 간격 = 1초동안 이동할 거리 추적
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

	pEnemy->DmgCapsuleActive(true, 2.f);

	UTPSGameInstance* pInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pInstance)
	{
		pInstance->StartSoundLocation(sound_key::MachineJump, GetWorld(),pEnemy->GetActorLocation(), ESoundAttenuationType::SOUND_LOUD);
	}
}

void UAttackEnemyState::LongRangeAttack()
{
	if (!IsValid(GetOwner()) || !bIsTraced) return;
	AEnemy* pEnemy = Cast<AEnemy>(GetOwner());
	auto pMesh = pEnemy->GetMesh();
	if (!IsValid(pMesh)) return;
	//타겟
	FVector vTargetPos = vTargetForeseeLoctaion;
	FVector vMyPos = pMesh->GetSocketLocation(FName(TEXT("Muzzle")));
	FVector vDirection = (vTargetPos - vMyPos).GetSafeNormal();

	pEnemy->SetisAttacking(true);

	float fRandPitch = float(rand() % 500) / 100.f; // 0~9.99.f
	float fRandYaw = float(rand() % 500) / 100.f; // 0~9.99.f
	bool IsEvenPitch = rand() % 2 == 0;
	bool IsEvenYaw = rand() % 2 == 0;

	vDirection = vDirection.RotateAngleAxis(IsEvenPitch ? fRandPitch : -fRandPitch, pEnemy->GetActorRightVector());
	vDirection = vDirection.RotateAngleAxis(IsEvenYaw ? fRandYaw : -fRandYaw, pEnemy->GetActorUpVector());

	//
	pEnemy->ActiveEffect(vDirection);
}

void UAttackEnemyState::LongRangeTrace()
{
	if (!IsValid(GetOwner())) return;
	AEnemy* pEnemy = Cast<AEnemy>(GetOwner());
	auto EnemyData = pEnemy->GetEnemyData();
	if(EnemyData.ProjectileDamage <= 0.f) return;
	auto pMesh = pEnemy->GetMesh();
	if (!IsValid(pMesh)) return;

	//타겟
	FVector vTargetPos = pEnemy->GetTargetActorPos();
	FVector vMyPos = pMesh->GetSocketLocation(FName(TEXT("Muzzle")));
	FVector vDirection = (vTargetPos - vMyPos).GetSafeNormal();

	FHitResult hitResult;

	TArray<AActor*> vecIgnore;
	vecIgnore.Add(pEnemy);

	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), vMyPos, vMyPos + vDirection * EnemyData.DetectLength,
		UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), true, vecIgnore, EDrawDebugTrace::ForOneFrame, hitResult, true))
	{
		if (hitResult.GetActor())
		{
			auto hitActor = hitResult.GetActor();
			if (hitActor->IsA(ATPSPortfolioCharacter::StaticClass()))
			{
				bIsTraced = true;
				return;
			}
		}
	}


	bIsTraced = false;
}

void UAttackEnemyState::Process(float deltatime)
{
	Turn(deltatime);
	Attack(deltatime);
	LongRangeTrace();
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
