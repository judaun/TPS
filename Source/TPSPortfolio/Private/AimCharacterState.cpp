// Fill out your copyright notice in the Description page of Project Settings.


#include "AimCharacterState.h"
#include "TPSPortfolioCharacter.h"

AimCharacterState::AimCharacterState(ATPSPortfolioCharacter* TpsCharacter)
{
	pCharacter = TpsCharacter;
	eState = eCharacterState::AIM;
}

AimCharacterState::~AimCharacterState()
{
}

void AimCharacterState::Update(float DeltaSeconds)
{
	CalculateSpeed(DeltaSeconds);
	Turn(DeltaSeconds);
	Move();
}

void AimCharacterState::Move()
{
	if (pCharacter == nullptr) return;

	if (pCharacter->GetIsMoving())
		pCharacter->AddMovementInput(pCharacter->GetChangeVector());
}

void AimCharacterState::CalculateSpeed(float DeltaSeconds)
{
	if (pCharacter == nullptr || !pCharacter->GetIsMoving()) return;

	pCharacter->SetBrakeSpeed(BRAKE_DECLEASE);
	pCharacter->SetWalkSpeed(FMath::FInterpConstantTo(pCharacter->GetWalkSpeed(), pCharacter->GetIsSprint() ? pCharacter->GetDefaultRunSpeed() : pCharacter->GetDefaultWalkSpeed()*2.f, DeltaSeconds, BRAKE_DECLEASE));
}

void AimCharacterState::Turn(float DeltaSeconds)
{
	if (pCharacter == nullptr) return;

	const FVector vActorForwardDirection = pCharacter->GetActorForwardVector();
	const FVector vChangeDirection = pCharacter->GetControlVector(true);

	double dArccosRadian = FMath::Acos(vActorForwardDirection.Dot(vChangeDirection));
	float fDegree = FMath::RadiansToDegrees(dArccosRadian);

	if (pCharacter->GetIsMoving() || pCharacter->GetisAimTurn()) {
		//외적으로 회전방향 구하기
		if (vActorForwardDirection.Cross(vChangeDirection).Z < 0)
			fDegree *= -1;

		double rTurnRot = FMath::FInterpConstantTo(0, fDegree, DeltaSeconds, TURN_SPEED);
		pCharacter->AddActorWorldRotation(FRotator(0, rTurnRot, 0));
	}

	if (pCharacter->GetIsMoving()) {
		LocoMotionDirection();
	}
}

void AimCharacterState::LocoMotionDirection()
{
	FVector vChangeControlDirection = pCharacter->GetChangeVector();
	FVector vControlDirection = pCharacter->GetControlVectorLastUpdated();
	FVector vControlYDirection = pCharacter->GetControlVectorLastUpdated(false);

	//float fChangingDegree = FMath::RadiansToDegrees(FMath::Acos(vControlYDirection.Dot(vChangeControlDirection)));
	float fChangingDegree = FMath::RadiansToDegrees(FMath::Acos(vControlDirection.Dot(vChangeControlDirection))) * 2.f;
	if (fChangingDegree < 1.f)
		fChangingDegree = 1.f;
	if (fChangingDegree > 359.f)
		fChangingDegree = 359.f;

	if (vControlDirection.Cross(vChangeControlDirection).Z < 0)
		fChangingDegree *= -1.f;

	pCharacter->SetYCrossAngle(fChangingDegree);
	pCharacter->SetCrossAngle(vControlDirection.Cross(vChangeControlDirection).Z);

	UE_LOG(LogTemp, Log, TEXT("fChangingDegree : %f"), fChangingDegree);

	float fControlAcos = -vControlYDirection.Cross(vChangeControlDirection).Z;

	pCharacter->SetFowardValue(fControlAcos < 0 ? -1.f : 1.f);
}

void AimCharacterState::Enter()
{
	if (pCharacter == nullptr) return;
	pCharacter->SetIsAiming(true);
	LocoMotionDirection();
}

void AimCharacterState::Exit()
{
	if (pCharacter == nullptr) return;
	pCharacter->SetWalkSpeed(0.f);
	pCharacter->SetIsAiming(false);
}
