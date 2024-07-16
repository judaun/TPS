// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestructActor.generated.h"

UCLASS()
class TPSPORTFOLIO_API ADestructActor : public AActor
{
	GENERATED_BODY()
	
	ADestructActor();

private:
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
//value///////////////////////////////////////////
private:
protected:
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UGeometryCollectionComponent* GeometryCollectionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UFieldSystemComponent* FieldSystemComponent;
};
