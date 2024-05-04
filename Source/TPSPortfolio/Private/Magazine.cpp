// Fill out your copyright notice in the Description page of Project Settings.


#include "Magazine.h"

// Sets default values
AMagazine::AMagazine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Magazine"));
	RootComponent = pMesh;

	bIsDestroy = false;
	fDestroyTime = 10.f;
}

void AMagazine::DeferredInitialize(FString magazineaddress)
{
	UStaticMesh* newMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *magazineaddress));
	if (newMesh && pMesh)
		pMesh->SetStaticMesh(newMesh);

	pMesh->SetCollisionProfileName(TEXT("MeshEffect"));
}

void AMagazine::MagazineOut()
{
	bIsDestroy = true;
	if (!IsValid(pMesh)) return;

	pMesh->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
void AMagazine::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMagazine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsDestroy)
	{
		fDestroyTime -= DeltaTime;
		if (fDestroyTime < 0.f)
			Destroy();
	}
}

