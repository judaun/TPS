// Fill out your copyright notice in the Description page of Project Settings.


#include "SprintCharacterState.h"
#include "TPSPortfolioCharacter.h"
#include "CalculationFunction.h"

SprintCharacterState::SprintCharacterState(ATPSPortfolioCharacter* TpsCharacter)
{
	pCharacter = TpsCharacter;
	eState = ECharacterState::SPRINT;
	fDegree = 0.f;
}

SprintCharacterState::~SprintCharacterState()
{
}

void SprintCharacterState::Update(float DeltaSeconds)
{
	CalculateSpeed(DeltaSeconds);
	Turn(DeltaSeconds);
	Move();
}

void SprintCharacterState::Move()
{
	if (pCharacter == nullptr) return;
	pCharacter->SetFrontAcos(1.f);
	float fSpeed = pCharacter->GetWalkSpeed();
	pCharacter->AddMovementInput(fSpeed > SPRINT_CONDITION ? pCharacter->GetActorForwardVector() : pCharacter->GetChangeVector());
}

void SprintCharacterState::CalculateSpeed(float DeltaSeconds)
{
	if (pCharacter == nullptr) return;

	pCharacter->SetBrakeSpeed(IDLE_DECLEASE);
	pCharacter->SetWalkSpeed(FMath::FInterpConstantTo(pCharacter->GetWalkSpeed(), abs(fDegree) > 30.f ? pCharacter->GetDefaultRunSpeed() : pCharacter->GetDefaultSprintSpeed(), DeltaSeconds, BRAKE_DECLEASE));
}

void SprintCharacterState::Turn(float DeltaSeconds)
{
	if (pCharacter == nullptr) return;

	const FVector vActorForwardDirection = pCharacter->GetActorForwardVector();
	const FVector vChangeDirection = pCharacter->GetChangeVector();
	const FVector vLerpDirection = pCharacter->GetLerpVector();
	float fCross = 0.f;
	float fCurrentWalkSpeed = pCharacter->GetWalkSpeed();
	float fCalcTurnSpeed = fCurrentWalkSpeed > SPRINT_TURN_DCREASE ? TURN_SPEED / (fCurrentWalkSpeed / SPRINT_TURN_DCREASE) : TURN_SPEED;

	//선형보간으로 회전속도 조절
	double rTurnRot = InterpConstantToVector(vActorForwardDirection, vChangeDirection, fCross, fDegree, DeltaSeconds, fCalcTurnSpeed);
	
	//좌우 기울기 움직임 값 적용을 위한 벡터의 점진적 회전 적용
	double rLerpTurnRot = InterpConstantToVector(vLerpDirection, vChangeDirection, DeltaSeconds, TURN_SPEED * 2.f);
	
	pCharacter->SetLerpVector(vLerpDirection.RotateAngleAxis(rLerpTurnRot, FVector::UpVector));
	pCharacter->SetCrossAngle(vActorForwardDirection.Cross(vLerpDirection).Z);
	pCharacter->AddActorWorldRotation(FRotator(0, rTurnRot, 0));
	
	if (abs(fDegree) > MAX_TURN_DGREE && fCurrentWalkSpeed > SPRINT_CONDITION) pCharacter->SetBraking(BRAKE_SPRINT);
}

void SprintCharacterState::Enter()
{
	if (pCharacter == nullptr) return;
	pCharacter->SetIsMoving(true);
}

void SprintCharacterState::Exit()
{
	if (pCharacter == nullptr) return;
	pCharacter->SetIsMoving(false);
}
