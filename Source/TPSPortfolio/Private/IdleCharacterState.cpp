// Fill out your copyright notice in the Description page of Project Settings.


#include "IdleCharacterState.h"
#include "TPSPortfolioCharacter.h"

IdleCharacterState::IdleCharacterState(ATPSPortfolioCharacter* TpsCharacter)
{
	pCharacter = TpsCharacter;
	eState = ECharacterState::IDLE;
}

IdleCharacterState::~IdleCharacterState()
{
}

void IdleCharacterState::Update(float DeltaSeconds)
{
	CalculateSpeed(DeltaSeconds);
	Move();
}

void IdleCharacterState::Move()
{
	if (pCharacter == nullptr) return;

	if (pCharacter->GetWalkSpeed() > MINIMUM_WALKSPEED)
		pCharacter->AddMovementInput(pCharacter->GetActorForwardVector());
}

void IdleCharacterState::CalculateSpeed(float DeltaSeconds)
{
	if (pCharacter == nullptr) return;

	pCharacter->SetBrakeSpeed(BRAKE_DECLEASE);
	pCharacter->SetWalkSpeed(FMath::FInterpConstantTo(pCharacter->GetWalkSpeed(), 0.f, DeltaSeconds, BRAKE_DECLEASE));
}
