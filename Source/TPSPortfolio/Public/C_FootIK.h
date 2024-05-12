// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_FootIK.generated.h"

struct FIKHitData
{
	float fFootOffset;
	FVector vHitNormal;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSPORTFOLIO_API UC_FootIK : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_FootIK();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FIKHitData FootIKTrace(float tracedistance, FName socketname);
	void UpdateRotator(FIKHitData hitdata, FRotator* rotdata, float deltaseconds);
	void UpdateCapsuleHeight(float Deltatime, float hipoffset);
	void UpdateFootOffset(float Deltatime, float targetvalue, float* effvalue, float interpspeed);
public:
	UFUNCTION(BlueprintCallable, Category = FootIK)
	FRotator GetFootIK(bool left) { return left ? rotFoot_L : rotFoot_R; }
	
	UFUNCTION(BlueprintCallable, Category = FootIK)
	float GetHipOffset() { return fHipOffset; }
	
	UFUNCTION(BlueprintCallable, Category = FootIK)
	float GetFootOffset(bool left) { return left ? fFootOffset_L : fFootOffset_R; }
private:
	FRotator rotFoot_L;
	FRotator rotFoot_R;
	float fHipOffset;
	float fHalfCapsuleHeight;
	float fFootOffset_L;
	float fFootOffset_R;
};
