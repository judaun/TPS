// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
struct FAIStimulus;
enum class EEnemyState : uint8;

namespace bb_key
{
	TCHAR* const TargetActor = TEXT("TargetActor");
	TCHAR* const TargetLocation = TEXT("TargetLocation");
	TCHAR* const PatrolLocation = TEXT("PatrolLocation");
	TCHAR* const EnemyState = TEXT("EnemyState");
	TCHAR* const IsAttack = TEXT("IsAttack");
	TCHAR* const IsLongRange = TEXT("IsLongRange");
}

UCLASS()
class TPSPORTFOLIO_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
	AEnemyController();

private:
	void InitializeBehaviorTree();
	void InitializePerception();
public:
	virtual void OnPossess(APawn* InPawn) override;
	void RunAI();
	void StopAI();
	/* AEnemy의 State가 변경됬을때 Blackboard에 값을 넣어주기 위한 함수 */
	void ChangeEnemyState(EEnemyState estate);
	void ChangeAttackBlackBoard(bool attacking);
	void ChangeLongRange(bool longrange);

	UFUNCTION()
		void OnTargetDetected(AActor* actor, FAIStimulus const stimulus);
	UFUNCTION()
		void OnTargetForgotten(AActor* actor);

private:
	UPROPERTY()
		class UBehaviorTree* BTAsset;
	UPROPERTY()
		class UBlackboardData* BBAsset;
	UPROPERTY()
		class UAISenseConfig_Sight* pSenceConfig;
	UPROPERTY()
		class UAISenseConfig_Hearing* pSenceConfig_Hear;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fAISightRadius = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fAILoseSightRadius = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fAIFieldOfView = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fAISightAge = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fAILastSeenLocation = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fAIHearingRange = 3000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fAIHearingAge = 5.f;

};
