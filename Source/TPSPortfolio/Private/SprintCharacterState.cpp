// Fill out your copyright notice in the Description page of Project Settings.


#include "SprintCharacterState.h"
#include "TPSPortfolioCharacter.h"

SprintCharacterState::SprintCharacterState(ATPSPortfolioCharacter* TpsCharacter)
{
	pCharacter = TpsCharacter;
	eState = eCharacterState::SPRINT;
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

	float fSpeed = pCharacter->GetWalkSpeed();
	pCharacter->AddMovementInput(fSpeed > RUN_CONDITION ? pCharacter->GetActorForwardVector() : pCharacter->GetChangeVector());
}

void SprintCharacterState::CalculateSpeed(float DeltaSeconds)
{
	if (pCharacter == nullptr) return;

	pCharacter->SetBrakeSpeed(IDLE_DECLEASE);
	pCharacter->SetWalkSpeed(FMath::FInterpConstantTo(pCharacter->GetWalkSpeed(), pCharacter->GetDefaultSprintSpeed(), DeltaSeconds, WALKSPEED_DECLEASE));
}

void SprintCharacterState::Turn(float DeltaSeconds)
{
	if (pCharacter == nullptr) return;

	const FVector vActorForwardDirection = pCharacter->GetActorForwardVector();
	const FVector vChangeDirection = pCharacter->GetChangeVector();

	double dArccosRadian = FMath::Acos(vActorForwardDirection.Dot(vChangeDirection));
	float fDegree = FMath::RadiansToDegrees(dArccosRadian);

	float fCurrentWalkSpeed = pCharacter->GetWalkSpeed();

	if (fDegree > MAX_TURN_DGREE && fCurrentWalkSpeed > RUN_CONDITION) pCharacter->SetBraking(fCurrentWalkSpeed > SPRINT_CONDITION ? BRAKE_SPRINT : BRAKE_RUN);

	//외적으로 회전방향 구하기
	if (vActorForwardDirection.Cross(vChangeDirection).Z < 0)
		fDegree *= -1;

	float fCalcTurnSpeed = fCurrentWalkSpeed > SPRINT_TURN_DCREASE ? TURN_SPEED / (fCurrentWalkSpeed / SPRINT_TURN_DCREASE) : TURN_SPEED;

	//선형보간으로 회전속도 조절
	double rTurnRot = FMath::FInterpConstantTo(0, fDegree, DeltaSeconds, fCalcTurnSpeed);

	//----------------------

	//좌우 기울기 움직임 값 적용을 위한 벡터의 점진적 회전 적용
	FVector vLerpDirection = pCharacter->GetLerpVector();
	float fLerpDegree = FMath::RadiansToDegrees(FMath::Acos(vLerpDirection.Dot(vChangeDirection)));
	if (vLerpDirection.Cross(vChangeDirection).Z < 0)
		fLerpDegree *= -1;
	double rLerpTurnRot = FMath::FInterpConstantTo(0, fLerpDegree, DeltaSeconds, TURN_SPEED * 2.f);

	pCharacter->SetLerpVector(vLerpDirection.RotateAngleAxis(rLerpTurnRot, FVector::UpVector));
	pCharacter->SetCrossAngle(vActorForwardDirection.Cross(vLerpDirection).Z);
	pCharacter->AddActorWorldRotation(FRotator(0, rTurnRot, 0));
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
