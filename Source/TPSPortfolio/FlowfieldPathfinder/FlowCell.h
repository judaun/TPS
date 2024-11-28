// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FlowCell.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API UFlowCell : public UObject
{
	GENERATED_BODY()

private:
protected:
public:
	/*VolumeUpdate시 Cell의 중심점 갱신용 함수 : vPos*/
	void SetPos(FVector pos, float distance) {vPos = pos; fDistance = distance;}
	FVector GetPos() {return vPos;}
	/*스탭계산 및 방문 체크*/
	void SetStep(int32 steps, bool checked) { iSteps = steps; isChecked = checked;}
	int32 GetStep() {return iSteps;}
	float GetDistance() {return fDistance;}
	/*Flowfield Pathfind계산 시 갱신용 함수  : vDir*/
	void UpdateDirection(FVector dir) { vDir = dir; }
	/*Step계산 중 계산범위에 넣을지 체크*/
	bool IsCheckedCell() 
	{
		if(!isAccess) return true;
		return isChecked;
	}

	bool IsAccess() {return isAccess;}
	/*해당 Cell의 FlowVector 반환*/
	FVector GetDirection() {return vDir;}

	/*해당 Cell의 이동 가능 조건*/
	void UpdateCellAccess(bool ison) {isAccess = ison; }

private:
	FVector vPos;
	FVector vDir;

	float fDistance;
	int32 iSteps;
	bool isChecked = false;
	bool isAccess = false;
};
