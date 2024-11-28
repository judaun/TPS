// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrainingTarget.generated.h"

class UCapsuleComponent;

UCLASS()
class TPSPORTFOLIO_API ATrainingTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrainingTarget();

private:
	void HitAction();
	void Recovery();
	void Dissolving(float culvevalue);
	void SetCollisionAll(bool isOn);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* pMesh;

	UPROPERTY()
	UMaterialInstanceDynamic* pM_Dynamic;
	float fDissolve;
	bool bIsDissolve;
	FTimerHandle dissolvetimehandle;
protected:
public:
};
