// Fill out your copyright notice in the Description page of Project Settings.


#include "RunCharacterState.h"
#include "TPSPortfolioCharacter.h"

RunCharacterState::RunCharacterState(ATPSPortfolioCharacter* TpsCharacter)
{
	pCharacter = TpsCharacter;
	eState = eCharacterState::RUN;
}

RunCharacterState::~RunCharacterState()
{
}

void RunCharacterState::Update(float DeltaSeconds)
{
	CalculateSpeed(DeltaSeconds);
	Turn(DeltaSeconds);
	Move();
}

void RunCharacterState::Move()
{
	if (pCharacter == nullptr) return;

	pCharacter->AddMovementInput(pCharacter->GetWalkSpeed() > RUN_CONDITION ? pCharacter->GetActorForwardVector() : pCharacter->GetChangeVector());
}

void RunCharacterState::CalculateSpeed(float DeltaSeconds)
{
	if (pCharacter == nullptr) return;

	pCharacter->SetBrakeSpeed(IDLE_DECLEASE);
	pCharacter->SetWalkSpeed(FMath::FInterpConstantTo(pCharacter->GetWalkSpeed(), pCharacter->GetDefaultRunSpeed(), DeltaSeconds, WALKSPEED_DECLEASE));
}

void RunCharacterState::Turn(float DeltaSeconds)
{
	if (pCharacter == nullptr) return;

	const FVector vActorForwardDirection = pCharacter->GetActorForwardVector();
	const FVector vChangeDirection = pCharacter->GetChangeVector();

	double dArccosRadian = FMath::Acos(vActorForwardDirection.Dot(vChangeDirection));
	float fDegree = FMath::RadiansToDegrees(dArccosRadian);

	float fCurrentWalkSpeed = pCharacter->GetWalkSpeed();

	if (fDegree > MAX_TURN_DGREE && fCurrentWalkSpeed > RUN_CONDITION) pCharacter->SetBraking(BRAKE_RUN);

	//외적으로 회전방향 구하기
	if (vActorForwardDirection.Cross(vChangeDirection).Z < 0)
		fDegree *= -1;

	//선형보간으로 회전속도 조절
	double rTurnRot = FMath::FInterpConstantTo(0, fDegree, DeltaSeconds, TURN_SPEED);

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

void RunCharacterState::Enter()
{
	if (pCharacter == nullptr) return;
	pCharacter->SetIsMoving(true);
}

void RunCharacterState::Exit()
{
	if (pCharacter == nullptr) return;
	pCharacter->SetIsMoving(false);
}
