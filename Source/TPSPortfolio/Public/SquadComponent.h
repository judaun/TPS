// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SquadComponent.generated.h"

struct stSquadPos 
{
	TWeakObjectPtr<class AEnemy> pPosOwner;
	FVector vOffset;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSPORTFOLIO_API USquadComponent : public UActorComponent
{
	GENERATED_BODY()

	USquadComponent();
//func///////////////////////////////////////////////////////////
private:
	bool IsEmptySquadPos();
	bool SetSquadPos(AActor* pactor);
	void UpdateSquadPos();
	bool IsMySquad(AActor* pactor);
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ReleaseSquadPos();
	void FindSquadMember();
//value//////////////////////////////////////////////////////////
private:
	TArray<stSquadPos> ta_Squad;

	UPROPERTY()
	FTimerHandle FindSquadTimehandle;
protected:
public:
		
};
