// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class TPSPORTFOLIO_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

//function//////////////////////////////////////////////
private:
	void InitializeMesh();
	void DestroyProjectile();
protected:
	virtual void BeginPlay() override;
	void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	void NotifyActorBeginOverlap(AActor* OtherActor) override;
public:	
	virtual void Tick(float DeltaTime) override;

	void SetData(bool isgravity, bool expolsive, int32 dmg, AActor* owner);
	void SetDirection(FVector direction, float speed);
//value/////////////////////////////////////////////////
private:
	FVector vDirection;
	FVector vStart;
	float fSpeed;
	int32 iDmg;

	UPROPERTY()
	TWeakObjectPtr<AActor> pOwner;
protected:
public:
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> pMesh;
};
