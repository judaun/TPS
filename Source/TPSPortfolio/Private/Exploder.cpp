// Fill out your copyright notice in the Description page of Project Settings.


#include "Exploder.h"
#include "QuadFootIK.h"
#include "Components/CapsuleComponent.h"

AExploder::AExploder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsAttacking = false;
}

void AExploder::BeginPlay()
{
	Super::BeginPlay();

	
}

void AExploder::InitializeDefaultComponent()
{
	Super::InitializeDefaultComponent();
	
	GetCapsuleComponent()->SetCapsuleSize(40.f, 40.0f);

	GetCapsuleComponent()->SetHiddenInGame(false);
	pFootIK = NewObject<UQuadFootIK>(this, UQuadFootIK::StaticClass(), TEXT("FootIK"));
	pFootIK->RegisterComponent();
}

void AExploder::InitializeMeshComponent()
{
	FString strMeshName = FString::Printf(TEXT("/Script/Engine.SkeletalMesh'/Game/Enemy/%s/Meshes/SK_%s.SK_%s'"), *EnemyData.Name, *EnemyData.Name, *EnemyData.Name);
	USkeletalMesh* pMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), NULL, *strMeshName));
	FString strAnimName = FString::Printf(TEXT("Blueprint'/Game/Enemy/%s/ABP_%s.ABP_%s_C'"), *EnemyData.Name, *EnemyData.Name, *EnemyData.Name);
	UClass* pAnimclass = StaticLoadClass(UAnimInstance::StaticClass(), NULL, *strAnimName);
	//UAnimBlueprint* pBPAnim = LoadAnim
	if (pMesh)
	{
		GetMesh()->SetSkeletalMesh(pMesh);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -45.f), FRotator(0.f, -90.f, 0.f));
		if(pAnimclass)GetMesh()->SetAnimInstanceClass(pAnimclass);
	}
}

void AExploder::SetEnemyData(FEnemyTable* enemydata)
{
	Super::SetEnemyData(enemydata);
	InitializeDefaultComponent();
	InitializeMeshComponent();
}
