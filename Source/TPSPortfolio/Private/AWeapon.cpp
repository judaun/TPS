// Fill out your copyright notice in the Description page of Project Settings.


#include "AWeapon.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	pMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	RootComponent = pMesh;
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FObj_WeaponSkmesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Props/Meshes/Pistol/Desert_Eagle.Desert_Eagle'"));
	if (FObj_WeaponSkmesh.Succeeded())
		pMesh->SetSkeletalMesh(FObj_WeaponSkmesh.Object);

	pMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

