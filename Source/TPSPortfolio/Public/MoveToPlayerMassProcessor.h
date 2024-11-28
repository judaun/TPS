// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MoveToPlayerMassProcessor.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API UMoveToPlayerMassProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
	UMoveToPlayerMassProcessor();

protected:
	// Filter which entities we want to handle
	virtual void ConfigureQueries() override;

	// Iterate through our entities
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	// The filter that we select our entities from
	FMassEntityQuery MassEntityQuery;
};
