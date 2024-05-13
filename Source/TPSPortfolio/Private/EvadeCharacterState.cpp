// Fill out your copyright notice in the Description page of Project Settings.


#include "EvadeCharacterState.h"
#include "TPSPortfolioCharacter.h"
#include "CalculationFunction.h"

EvadeCharacterState::EvadeCharacterState(ATPSPortfolioCharacter* TpsCharacter)
{
	pCharacter = TpsCharacter;
	eState = ECharacterState::EVADE;
	vLastDirection = FVector::ZeroVector;
}

EvadeCharacterState::~EvadeCharacterState()
{
}

void EvadeCharacterState::Update(float DeltaSeconds)
{
	CalculateSpeed(DeltaSeconds);
	Turn(DeltaSeconds);
	Move();
}

void EvadeCharacterState::Move()
{
	if (pCharacter == nullptr) return;
	pCharacter->AddMovementInput(vLastDirection);
}

void EvadeCharacterState::Turn(float DeltaSeconds)
{
	if (pCharacter == nullptr) return;

	const FVector vActorForwardDirection = pCharacter->GetActorForwardVector();
	const FVector vChangeDirection = vLastDirection;
	const FVector vLerpDirection = pCharacter->GetLerpVector();

	//선형보간으로 회전속도 조절
	double rTurnRot = InterpConstantToVector(vActorForwardDirection, vChangeDirection, DeltaSeconds, TURN_SPEED*3.f);

	pCharacter->AddActorWorldRotation(FRotator(0, rTurnRot, 0));
}

void EvadeCharacterState::CalculateSpeed(float DeltaSeconds)
{
	if (pCharacter == nullptr) return;

	pCharacter->SetBrakeSpeed(IDLE_DECLEASE);
	pCharacter->SetWalkSpeed(FMath::FInterpConstantTo(pCharacter->GetWalkSpeed(), 0.f, DeltaSeconds, BRAKE_DECLEASE));
}

void EvadeCharacterState::Enter()
{
	if (pCharacter == nullptr) return;
	pCharacter->SetIsEvade(true);
	vLastDirection = pCharacter->GetChangeVector();
}

void EvadeCharacterState::Exit()
{
	if (pCharacter == nullptr) return;
	pCharacter->SetIsEvade(false);
}

void EvadeCharacterState::DustEffect()
{

}

