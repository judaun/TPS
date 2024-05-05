// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet"));
	RootComponent = pMesh;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FObj_Stmesh(TEXT("/Game/Props/Meshes/ETC/BulletFX.BulletFX"));
	if (FObj_Stmesh.Succeeded())
	{
		pMesh->SetStaticMesh(FObj_Stmesh.Object);
		pMesh->SetSimulatePhysics(true);
		pMesh->SetCollisionProfileName(TEXT("MeshEffect"));
	}
	fDestroyTime = 10.f;
	
	pMesh->SetRelativeScale3D(FVector(0.15f, 0.15f, 0.15f));
	
	
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	//RootComponent->AddRelativeLocation(fBulletPos*10.f);
	//pMesh->AddVelocityChangeImpulseAtLocation(fBulletPos*10.f, FVector::ZeroVector);
	
	int32 irand = rand();
	fBulletPos = (GetActorRightVector()*2.f + GetActorUpVector()).GetSafeNormal();
	pMesh->AddImpulseAtLocation(fBulletPos*((int)irand % 5 + 10),FVector((int)irand%100-50, (int)irand % 100 - 50,0.f));
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	fDestroyTime -= DeltaTime;
	if (fDestroyTime < 0.f)
		Destroy();

	
}

