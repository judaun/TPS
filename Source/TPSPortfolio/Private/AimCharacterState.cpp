// Fill out your copyright notice in the Description page of Project Settings.


#include "AimCharacterState.h"
#include "TPSPortfolioCharacter.h"

AimCharacterState::AimCharacterState(ATPSPortfolioCharacter* TpsCharacter)
{
	pCharacter = TpsCharacter;
	eState = eCharacterState::AIM;
	fTimer = 0.f;
	bIsTimeOut = false;
}

AimCharacterState::~AimCharacterState()
{
}

void AimCharacterState::Update(float DeltaSeconds)
{
	CalculateSpeed(DeltaSeconds);
	TimerTurn(DeltaSeconds);
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
	float fAcross = vActorForwardDirection.Cross(vChangeDirection).Z;

	if (fDegree > MINIMUM_TURN_DEGREE && bIsTimeOut)
	{
		SetTimer(1.f);
		pCharacter->SetFowardValue(fAcross < 0 ? -fDegree : fDegree);
	}
	else if(fDegree < ALLOW_TURN_DEGREE)
		pCharacter->SetIsAimTurn(false);

	bool bIsMoving = pCharacter->GetIsMoving();
	if (bIsMoving || pCharacter->GetisAimTurn()) {
		//외적으로 회전방향 구하기
		
		double rTurnRot = FMath::FInterpConstantTo(0, fAcross < 0 ? -fDegree : fDegree, DeltaSeconds, bIsMoving ? TURN_SPEED *2.f : TURN_SPEED/1.5f);
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
	
	//오리엔테이션워핑 각도 구하기
	float fChangingDegree = FMath::RadiansToDegrees(FMath::Acos(vControlDirection.Dot(vChangeControlDirection)))*2.f;
	
	//범위제한 1~359
	fChangingDegree = clamp(fChangingDegree, 1.f, 359.f);
	
	if (vControlDirection.Cross(vChangeControlDirection).Z < 0)
		fChangingDegree *= -1.f;

	UE_LOG(LogTemp, Log, TEXT("fChangingDegree: %f"), fChangingDegree);
	pCharacter->SetYCrossAngle(fChangingDegree);
	pCharacter->SetCrossAngle(vControlDirection.Cross(vChangeControlDirection).Z);
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

void AimCharacterState::TimerTurn(float DeltaSeconds)
{
	if (fTimer > 0.f)
		fTimer -= DeltaSeconds;

	if (fTimer <= 0.f && !bIsTimeOut)
	{
		pCharacter->SetIsAimTurn(true);
		bIsTimeOut = true;
	}
}

void AimCharacterState::SetTimer(float TimerTime)
{
	fTimer = TimerTime;
	bIsTimeOut = false;
}
