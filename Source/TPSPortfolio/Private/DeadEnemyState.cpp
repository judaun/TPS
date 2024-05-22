// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadEnemyState.h"
#include "TPSEnum.h"
#include "TPSGameInstance.h"
#include "TPSEffectMng.h"
#include "TPSCamaraMng.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy.h"

UDeadEnemyState::UDeadEnemyState()
{
	eStatetype = EEnemyState::DEAD;
}

void UDeadEnemyState::Explosion()
{
	AEnemy* pOwnerEnemy = Cast<AEnemy>(GetOwner());
	//카메라 셰이크를 위한 거리 보정값 및 이펙트 스폰 변수
	FVector vSrc = pOwnerEnemy->GetActorLocation();
	FVector vDst = GetWorld()->GetFirstPlayerController()->GetFocalLocation();
	float fDist = (vSrc - vDst).Length();
	float fShakeScale = 1.f - (fDist / MAXDISTSHAKE);


	UTPSGameInstance* pInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pInstance)
	{
		pInstance->SpawnEffect(Eff_key::DistortionBoom, GetWorld(), GetOwner()->GetActorLocation(), FRotator::ZeroRotator, FVector(2.f), true);
		if (fShakeScale > 0.f)
			pInstance->ClientCameraShake(shake_key::Boom_Small, GetWorld()->GetFirstPlayerController(), fShakeScale);
	}


	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	DrawDebugSphere(GetWorld(), pOwnerEnemy->GetActorLocation(),500.f,12,FColor::Red,false,3.f);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), 6000.f, pOwnerEnemy->GetActorLocation(), 500.f, nullptr, ActorsToIgnore, GetOwner(), pOwnerEnemy->GetController(), false, ECollisionChannel::ECC_WorldDynamic);
}

void UDeadEnemyState::Process(float deltatime)
{

}

void UDeadEnemyState::Enter()
{
	if(!IsValid(GetOwner())) return;
	
	AEnemy* pOwnerEnemy = Cast<AEnemy>(GetOwner());
	
	//메쉬 분리
	pOwnerEnemy->BreakBone();
	
	if (pOwnerEnemy->GetEnemyData().EnemyType == EEnemyType::ENEMY_ASSAULT)
	{
		Explosion();
	}
	
}

void UDeadEnemyState::Exit()
{

}
