// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassObserverProcessor.h"
#include "MassKilledObserver.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API UMassKilledObserver : public UMassObserverProcessor
{
	GENERATED_BODY()
	
public:
	UMassKilledObserver();

protected:
	virtual void ConfigureQueries() override;

	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

protected:
	FMassEntityQuery MassEntityQuery;
};
