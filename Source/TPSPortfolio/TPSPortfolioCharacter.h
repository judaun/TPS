// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterState.h"
#include "TPSPortfolioCharacter.generated.h"

#define BRAKE_RUN 0.5f
#define BRAKE_SPRINT 0.9f
#define RUN_CONDITION 400
#define SPRINT_CONDITION 600
#define WALKSPEED_DECLEASE 400.f
#define BRAKE_DECLEASE 900.f
#define IDLE_DECLEASE 550.f
#define MAX_TURN_DGREE 120.f
#define SPRINT_TURN_DCREASE 300.f
#define TURN_SPEED 360.f

using namespace std;

UCLASS(config=Game)
class ATPSPortfolioCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	/** Run Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RunAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimAction;

public:
	ATPSPortfolioCharacter();
	~ATPSPortfolioCharacter();

private:
	void initialize();

protected:
	void Move(const FInputActionValue& Value);
	void MoveComplete();
	void Look(const FInputActionValue& Value);
	void Run() { bIsRun = !bIsRun; }
	void Sprint() { bIsSprint = true; }
	void StopSprint() { bIsSprint = false; }
	void Aim();
	void AimComplete();
	void Attack();
	void AttackComplete();

	void SetMoveDirection(const FVector& vFoward, const FVector& vRight, const FVector2D& vMoveVector);
	void Turn(float DeltaSeconds);
			
	void CaculateTotalWalkSpeed(float DeltaSeconds);

	void Timer(float DeltaSeconds);
	void SlideGround(float DeltaSeconds);

	void UpdateState(float DeltaSeconds);
	void ChangeState(eCharacterState eChangeState);
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = TPSCharater)
	bool GetisBraking() {return bIsBraking;}

	UFUNCTION(BlueprintCallable, Category = TPSCharater)
	float GetCrossAngle() { return fCrossAngle; }

	UFUNCTION(BlueprintCallable, Category = TPSCharater)
	float GetisAiming() { return bIsAiming; }

public:
	FVector GetChangeVector() { return vChangeDirection; }
	FVector GetLerpVector() { return vLerpDirection; }
	FVector GetControlVector();

	float GetDefaultWalkSpeed() { return fDefaultWalkSpeed; }
	float GetDefaultRunSpeed() { return fRunSpeed; }
	float GetDefaultSprintSpeed() { return fSprintSpeed; }

	float GetWalkSpeed();
	bool GetIsMoving() { return bIsMoving; }

	void SetLerpVector(FVector LerpVecter) { vLerpDirection = LerpVecter; }
	void SetCrossAngle(float CrossAngle) { fCrossAngle = CrossAngle; }
	void SetIsBraking(bool IsBraking) {bIsBraking = IsBraking;}
	void SetIsMoving(bool IsMoving) { bIsMoving = IsMoving;}
	void SetWalkSpeed(float WalkSpeed);
	void SetBrakeSpeed(float BrakeSpeed);
	void SetIsAiming(bool IsAiming) { bIsAiming = IsAiming; }

	void SetBraking(float fTime);
private:
	TPSCharacterState* stCharacterState;
	vector<unique_ptr<TPSCharacterState>> vecState;
	FVector vChangeDirection;
	FVector vLerpDirection;

	float fCrossAngle;
	float fTurnSpeed;
	float fDefaultWalkSpeed;
	float fRunSpeed;
	float fSprintSpeed;
	float fBrakeTimer;

	bool bIsRun;
	bool bIsSprint;
	bool bIsMoving;
	bool bIsBraking;
	bool bIsAiming;
};
