// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "MissionObj.generated.h"

class UCapsuleComponent;
 
DECLARE_DELEGATE_OneParam(FDele_Mission_Percent, float);
DECLARE_DELEGATE_OneParam(FDele_Mission_Text, FString);

enum MissionState : uint8
{
	MISSION_READY, MISSION_PHASE1, MISSION_PHASE2, MISSION_PHASE3, MISSION_END
};

UCLASS()
class TPSPORTFOLIO_API AMissionObj : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissionObj();

private:
	void ActivateAnimation();
	void ActivateUI();
	void DeActivateUI();
	void SpawnEnemy(FVector location);
	void PhaseProgress();
	void PhaseReset();
	void TimerCheck(float DeltaTime);
	void ExecuteEqs();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Interaction();
	void InteractionUI(bool ison);
	void MissionComplete();
	void EqsFinished(TSharedPtr<FEnvQueryResult> result);
private:
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> MeshComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TSubclassOf<UAnimInstance> AnimationInstance;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<UCapsuleComponent> CapsuleComp;

	UPROPERTY(EditAnywhere, Category = "UIComponent")
	TObjectPtr<class UWidgetComponent> pUIComponent;

	UPROPERTY(EditAnywhere, Category = "UIComponent")
	TObjectPtr<class UWidgetComponent> pInterationUIComponent;

	UPROPERTY(EditAnywhere, Category = "MinimapSprite")
	TObjectPtr<class UPaperSpriteComponent> MinimapSprite;

	UPROPERTY(EditAnywhere, Category = "EQS")
	TObjectPtr<class UEnvQuery> Eqs;

	UPROPERTY()
	FEnvQueryRequest pEqsRequest;

	UPROPERTY()
	FTimerHandle Activetimehandle;

	float fPhaseTime;
	float fUIUpdateTime;
	MissionState curState;

public:
	FDele_Mission_Percent fDele_Percent;
	FDele_Mission_Text fDele_Txt;
};
