// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimeLineComponent.h"
#include "Explosive.generated.h"

UCLASS()
class TPSPORTFOLIO_API AExplosive : public AActor
{
	GENERATED_BODY()	
public:
	AExplosive();

//func/////////////////////////////////////////////
private:
	void Explode();
protected:
	virtual void BeginPlay() override;
	void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	void NotifyActorBeginOverlap(AActor* OtherActor) override;
public:	
	virtual void Tick(float DeltaTime) override;
	void DeferredInitialize(bool isProjectile);
	void SetExplosiveValue(bool isCollision, float radius, int32 dmg, float explodetime = 0.f);
	void AddImpuse(FVector impulsevector);
//value////////////////////////////////////////////
private:
	UPROPERTY()
	UStaticMeshComponent* pMesh;

	FTimerHandle Explodetimehandle;
	float fExplodeTime;
	float fExplodeRadius;
	int32 iExpodeDmg;
	bool bIsCollisionExplode;
	bool bIsExploding;
protected:
public:
};
