// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToPlayerMassProcessor.h"

#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MassNavigationFragments.h"
#include "Kismet/GameplayStatics.h"
#include "MoveToPlayerMassTag.h"
#include "TPSGameSingleton.h"
#include "FlowCell.h"
#include "MassSpawner.h"

UMoveToPlayerMassProcessor::UMoveToPlayerMassProcessor()
{
	//인스턴스 자동생성
	bAutoRegisterWithProcessingPhases = true;

	//Excute 전 목표설정을 위해 avoidance movement 추가
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Movement);

}

void UMoveToPlayerMassProcessor::ConfigureQueries()
{
	MassEntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	MassEntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);

	MassEntityQuery.AddTagRequirement<FMoveToPlayerMassTag>(EMassFragmentPresence::All);

	MassEntityQuery.RegisterWithProcessor(*this);
}

void UMoveToPlayerMassProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	const float DeltaTime = FMath::Min(0.1f, Context.GetDeltaTimeSeconds());

	MassEntityQuery.ForEachEntityChunk(EntityManager,Context,[this, DeltaTime](FMassExecutionContext& MassExecutionContext){
		const TArrayView<FMassMoveTargetFragment> MoveTargetList = MassExecutionContext.GetMutableFragmentView<FMassMoveTargetFragment>();
		const TArrayView<FTransformFragment> LocationList = MassExecutionContext.GetMutableFragmentView<FTransformFragment>();

		for (int32 iEntityIndex=0; iEntityIndex < MassExecutionContext.GetNumEntities(); ++iEntityIndex)
		{
			FTransform& CurrentTransform = LocationList[iEntityIndex].GetMutableTransform();
			FMassMoveTargetFragment& MassMoveTragetFrag = MoveTargetList[iEntityIndex];

			UFlowCell* pCell = UTPSGameSingleton::Get().GetCell(CurrentTransform);
			if(nullptr == pCell) continue;
			if(pCell->GetStep() == 0) continue;

			FVector vPos = pCell->GetPos();
			vPos.Z += 25.f;
			FVector vDir = pCell->GetDirection();
			float fDist = pCell->GetDistance();

			if (!pCell->IsAccess())
			{
				UFlowCell* pNearCell = UTPSGameSingleton::Get().GetAccessCell(CurrentTransform);
				if (pNearCell)
				{
					FVector vNearPos = pNearCell->GetPos();
					FVector vEntityPos = CurrentTransform.GetLocation();
					vNearPos.Z += 25.f;
					FVector vNearDir = (vNearPos - vEntityPos).GetSafeNormal();
					vEntityPos += vNearDir * 600.f * DeltaTime;
					CurrentTransform.SetTranslation(vEntityPos);
				}
				else
				{
					CurrentTransform.SetTranslation(FVector(-999.f, -999.f, -999.f));
					continue;
				}
					
				
				continue;
			}

			MassMoveTragetFrag.Center = vPos + (vDir*fDist);
			MassMoveTragetFrag.Forward = vDir;
		}
	});

}
