// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MassNavigationProcessor.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API UMassNavigationProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UMassNavigationProcessor();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;
};
