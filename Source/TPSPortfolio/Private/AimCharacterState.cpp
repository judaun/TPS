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
	if (pCharacter == nullptr) return;

	pCharacter->SetBrakeSpeed(IDLE_DECLEASE);
	pCharacter->SetWalkSpeed(FMath::FInterpConstantTo(pCharacter->GetWalkSpeed(), pCharacter->GetIsSprint() ? pCharacter->GetDefaultRunSpeed() : pCharacter->GetDefaultWalkSpeed(), DeltaSeconds, WALKSPEED_DECLEASE));
}

void AimCharacterState::Turn(float DeltaSeconds)
{
	if (pCharacter == nullptr) return;

	const FVector vActorForwardDirection = pCharacter->GetActorForwardVector();
	const FVector vChangeDirection = pCharacter->GetControlVector(true);

	double dArccosRadian = FMath::Acos(vActorForwardDirection.Dot(vChangeDirection));
	float fDegree = FMath::RadiansToDegrees(dArccosRadian);

	//외적으로 회전방향 구하기
	if (vActorForwardDirection.Cross(vChangeDirection).Z < 0)
		fDegree *= -1;

	//선형보간으로 회전속도 조절
	double rTurnRot = FMath::FInterpConstantTo(0, fDegree, DeltaSeconds, TURN_SPEED*2.f);

	FVector vChangeControlDirection = pCharacter->GetChangeVector();
	FVector vControlYDirection = pCharacter->GetControlVector(false);

	float fChangingDegree = FMath::RadiansToDegrees(FMath::Acos(vControlYDirection.Dot(vChangeControlDirection)));

	pCharacter->SetCrossAngle(fChangingDegree);

	float fWalkSpeed = pCharacter->GetWalkSpeed();
	float fControlAcos = vControlYDirection.Cross(vChangeControlDirection).Z;
	fWalkSpeed = fControlAcos < 0 || fChangingDegree==0 ? fWalkSpeed : -fWalkSpeed;

	pCharacter->SetFowardValue(fWalkSpeed);

	//UE_LOG(LogTemp, Log, TEXT("FV : %f, CrossAngle : %f"), pCharacter->GetFowardValue(), fChangingDegree);
	
	pCharacter->AddActorWorldRotation(FRotator(0, rTurnRot, 0));
}

void AimCharacterState::Enter()
{
	if (pCharacter == nullptr) return;
	pCharacter->SetIsAiming(true);
}

void AimCharacterState::Exit()
{
	if (pCharacter == nullptr) return;
	pCharacter->SetWalkSpeed(0.f);
	pCharacter->SetIsAiming(false);
}
