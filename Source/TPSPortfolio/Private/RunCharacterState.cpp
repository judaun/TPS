// Fill out your copyright notice in the Description page of Project Settings.


#include "RunCharacterState.h"
#include "TPSPortfolioCharacter.h"
#include "CalculationFunction.h"

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

	pCharacter->AddMovementInput(pCharacter->GetWalkSpeed() > SPRINT_CONDITION ? pCharacter->GetActorForwardVector() : pCharacter->GetChangeVector());
}

void RunCharacterState::CalculateSpeed(float DeltaSeconds)
{
	if (pCharacter == nullptr) return;

	pCharacter->SetBrakeSpeed(IDLE_DECLEASE);
	pCharacter->SetWalkSpeed(FMath::FInterpConstantTo(pCharacter->GetWalkSpeed(), pCharacter->GetDefaultRunSpeed(), DeltaSeconds, BRAKE_DECLEASE));
}

void RunCharacterState::Turn(float DeltaSeconds)
{
	if (pCharacter == nullptr) return;

	const FVector vActorForwardDirection = pCharacter->GetActorForwardVector();
	const FVector vChangeDirection = pCharacter->GetChangeVector();
	const FVector vLerpDirection = pCharacter->GetLerpVector();

	//선형보간으로 회전속도 조절
	double rTurnRot = InterpConstantToVector(vActorForwardDirection, vChangeDirection, DeltaSeconds, TURN_SPEED);

	//좌우 기울기 움직임 값 적용을 위한 벡터의 점진적 회전 속도
	double rLerpTurnRot = InterpConstantToVector(vLerpDirection, vChangeDirection, DeltaSeconds, TURN_SPEED*2.f);

	pCharacter->SetLerpVector(vLerpDirection.RotateAngleAxis(rLerpTurnRot, FVector::UpVector));
	pCharacter->SetCrossAngle(vActorForwardDirection.Cross(vLerpDirection).Z);
	UE_LOG(LogTemp, Log, TEXT("Cross : %f"), vActorForwardDirection.Cross(vLerpDirection).Z);

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
