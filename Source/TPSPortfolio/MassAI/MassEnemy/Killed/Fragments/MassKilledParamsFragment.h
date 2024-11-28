// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MassEntityTypes.h"
#include "MassKilledParamsFragment.generated.h"
/**
 * 
 */
 USTRUCT()
 struct FMassEnemyKilledParamsFragment : public FMassSharedFragment 
 {
	GENERATED_BODY()

 public:
	UPROPERTY(EditAnywhere, Category="Particle")
	TObjectPtr<UParticleSystem> pParticleToPlayWhenKilled;

	UPROPERTY(EditAnywhere, Category="Particle")
	FVector vParticlePlayOffset = FVector(0.f,0.f,60.f);
 };
