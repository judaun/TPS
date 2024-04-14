// Fill out your copyright notice in the Description page of Project Settings.


#include "BrakeCharacterState.h"
#include "TPSPortfolioCharacter.h"

BrakeCharacterState::BrakeCharacterState(ATPSPortfolioCharacter* TpsCharacter)
{
	pCharacter = TpsCharacter;
	eState = eCharacterState::BRAKE;
}

BrakeCharacterState::~BrakeCharacterState()
{
}

void BrakeCharacterState::Update(float DeltaSeconds)
{
	CalculateSpeed(DeltaSeconds);
	Move();
}

void BrakeCharacterState::Move()
{
	if (pCharacter == nullptr) return;

	if (pCharacter->GetWalkSpeed() > MINIMUM_WALKSPEED)
		pCharacter->AddMovementInput(pCharacter->GetActorForwardVector());
}

void BrakeCharacterState::CalculateSpeed(float DeltaSeconds)
{
	if (pCharacter == nullptr) return;

	pCharacter->SetBrakeSpeed(BRAKE_DECLEASE);
	pCharacter->SetWalkSpeed(FMath::FInterpConstantTo(pCharacter->GetWalkSpeed(), 0.f, DeltaSeconds, BRAKE_DECLEASE));

}

void BrakeCharacterState::Enter()
{
	if (pCharacter == nullptr) return;
	pCharacter->SetIsBraking(true);
}

void BrakeCharacterState::Exit()
{
	pCharacter->SetIsBraking(false);
}
