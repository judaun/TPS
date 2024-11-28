// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToPlayer.h"

#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassNavigationFragments.h"
#include "MoveToPlayerMassTag.h"

void UMoveToPlayer::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.RequireFragment<FTransformFragment>();
	BuildContext.RequireFragment<FMassMoveTargetFragment>();

	BuildContext.AddTag<FMoveToPlayerMassTag>();
}
