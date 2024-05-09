// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class TPSPORTFOLIO_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);
	void ShotShell(bool isshotshell) { bIsShotShell = isshotshell; }
private:
	UPROPERTY(VisibleAnywhere, Category = "MeshComponents")
	UStaticMeshComponent* pMesh;

	FVector fBulletPos;
	float fDestroyTime;

	bool bIshitsound;
	bool bIsShotShell;
};
