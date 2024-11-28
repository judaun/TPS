// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MoveToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API UMoveToPlayer : public UMassEntityTraitBase
{
	GENERATED_BODY()
	
public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
