// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionComponent.h"
#include "Enemy.h"
#include "TPSEnum.h"
#include "TPSTag.h"



AEnemyController::AEnemyController()
{
	InitializeBehaviorTree();
	InitializePerception();
}

void AEnemyController::InitializeBehaviorTree()
{
	const ConstructorHelpers::FObjectFinder<UBehaviorTree> FObj_BT(TEXT("/Script/AIModule.BehaviorTree'/Game/ThirdPerson/AI/BT_Enemy.BT_Enemy'"));
	if (FObj_BT.Succeeded())
	{
		BTAsset = FObj_BT.Object;
	}

	const ConstructorHelpers::FObjectFinder<UBlackboardData> FObj_BB(TEXT("/Script/AIModule.BlackboardData'/Game/ThirdPerson/AI/BB_Enemy.BB_Enemy'"));
	if (FObj_BB.Succeeded())
	{
		BBAsset = FObj_BB.Object;
	}
}

void AEnemyController::InitializePerception()
{
	UAIPerceptionComponent* pComponent = CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*pComponent);

#pragma region Sight Config
	pSenceConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight_Config"));
	pSenceConfig->SightRadius = fAISightRadius;
	pSenceConfig->LoseSightRadius = fAILoseSightRadius;
	pSenceConfig->PeripheralVisionAngleDegrees = fAIFieldOfView;
	pSenceConfig->SetMaxAge(fAISightAge);
	pSenceConfig->AutoSuccessRangeFromLastSeenLocation = fAILastSeenLocation;

	pSenceConfig->DetectionByAffiliation.bDetectEnemies = true;
	pSenceConfig->DetectionByAffiliation.bDetectFriendlies = true;
	pSenceConfig->DetectionByAffiliation.bDetectNeutrals = true;

	//우선순위 결정
	GetPerceptionComponent()->SetDominantSense(*pSenceConfig->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*pSenceConfig);
#pragma endregion
#pragma region Heard Config
	pSenceConfig_Hear = CreateOptionalDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Heard_Config"));
	pSenceConfig_Hear->HearingRange = fAIHearingRange;
	pSenceConfig_Hear->SetMaxAge(fAIHearingAge);
	pSenceConfig_Hear->DetectionByAffiliation.bDetectEnemies = true;
	pSenceConfig_Hear->DetectionByAffiliation.bDetectFriendlies = true;
	pSenceConfig_Hear->DetectionByAffiliation.bDetectNeutrals = true;
	GetPerceptionComponent()->ConfigureSense(*pSenceConfig_Hear);
#pragma endregion

	FScriptDelegate DelePerceptiontionUpdate;
	DelePerceptiontionUpdate.BindUFunction(this, FName("OnTargetDetected"));
	GetPerceptionComponent()->OnTargetPerceptionUpdated.Add(DelePerceptiontionUpdate);

	//Update문에서 도는 모든 감지된 Stimuli를 판별할 수 있는 방법이 있지만 위의 방법이 더 효율적인듯
	//FScriptDelegate DelePerceptionUpdate;
	//DelePerceptionUpdate.BindUFunction(this, FName("OnUpdated"));
	//GetPerceptionComponent()->OnPerceptionUpdated.Add(DelePerceptionUpdate);

}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RunAI();
}

void AEnemyController::RunAI()
{
	UBlackboardComponent* BTcomp = Blackboard.Get();
	if (UseBlackboard(BBAsset, BTcomp))
	{
		RunBehaviorTree(BTAsset);
	}
}

void AEnemyController::StopAI()
{
	UBehaviorTreeComponent* BTcomp = Cast<UBehaviorTreeComponent>(BrainComponent);
	if(nullptr == BTcomp) return;

	BTcomp->StopTree(EBTStopMode::Safe);
}

void AEnemyController::ChangeEnemyState(EEnemyState estate)
{
	UBlackboardComponent* BTcomp = Blackboard.Get();
	BTcomp->SetValueAsEnum(bb_key::EnemyState, (uint8)estate);
}

void AEnemyController::ChangeAttackBlackBoard(bool attacking)
{
	UBlackboardComponent* BTcomp = Blackboard.Get();
	BTcomp->SetValueAsBool(bb_key::IsAttack, attacking);
}

void AEnemyController::OnTargetDetected(AActor* actor, FAIStimulus const stimulus)
{
	APawn* pCurPawn = GetPawn();
	if(!IsValid(pCurPawn)) return;
	AEnemy* pEnemy = Cast<AEnemy>(pCurPawn);
	if(!IsValid(pEnemy)) return;
	UBlackboardComponent* BTcomp = Blackboard.Get();

	if (stimulus.WasSuccessfullySensed())
	{
		if (stimulus.Type == pSenceConfig->GetSenseID())
		{
			BTcomp->SetValueAsObject(bb_key::TargetActor, actor);
			pEnemy->SetTargetActor(actor);
		}
		else if (stimulus.Type == pSenceConfig_Hear->GetSenseID())
		{
			BTcomp->SetValueAsVector(bb_key::TargetLocation, actor->GetActorLocation());
		}
	}	
	else
	{
		if (stimulus.Type == pSenceConfig->GetSenseID())
		{
			/* 시야 탐지를 소실했을때 현재 대상 엑터의 위치를 탐지 지역으로 추가해주어 한번 더 추적 */
			BTcomp->ClearValue(bb_key::TargetActor);
			pEnemy->ClearTarget();
			if(IsValid(actor))
			BTcomp->SetValueAsVector(bb_key::TargetLocation, actor->GetActorLocation());
		}
		else if (stimulus.Type == pSenceConfig_Hear->GetSenseID())
		{
			BTcomp->ClearValue(bb_key::TargetLocation);
		}
	}
}

void AEnemyController::OnTargetForgotten(AActor* actor)
{
	APawn* pCurPawn = GetPawn();
	if (!IsValid(pCurPawn)) return;
	AEnemy* pEnemy = Cast<AEnemy>(pCurPawn);
	if (!IsValid(pEnemy)) return;

	UE_LOG(LogTemp, Log, TEXT("TargetForgotten : ChangeState"));
}
//
//void AEnemyController::OnUpdated(const TArray<AActor*>& UpdatedActors)
//{
//	for (size_t i = 0; i < UpdatedActors.Num(); ++i)
//	{
//		FActorPerceptionBlueprintInfo FPerceptionInfo;
//		GetPerceptionComponent()->GetActorsPerception(UpdatedActors[i],FPerceptionInfo);
//		for (size_t j = 0; j < FPerceptionInfo.LastSensedStimuli.Num(); ++j)
//		{
//			const FAIStimulus FStim = FPerceptionInfo.LastSensedStimuli[j];
//			if (FStim.Tag == ai_tag::noise_tag)
//			{
//
//			}
//		}
//	}
//}
