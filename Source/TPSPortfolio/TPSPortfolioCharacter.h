// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterState.h"
#include "AWeapon.h"
#include "Inventory.h"
#include "TPSPortfolioCharacter.generated.h"

#define BRAKE_RUN 0.5f
#define BRAKE_SPRINT 0.7f
#define RUN_CONDITION 400
#define SPRINT_CONDITION 750
#define WALKSPEED_DECLEASE 400.f
#define BRAKE_DECLEASE 900.f
#define IDLE_DECLEASE 550.f
#define MAX_TURN_DGREE 120.f
#define SPRINT_TURN_DCREASE 300.f
#define TURN_SPEED 360.f
#define CAMERA_FOV_SPEED 5.f
#define IDLE_AIMRATE 1.f
#define AIM_AIMRATE 0.f
#define RUN_AIMRATE 1.f
#define SPRINT_AIMRATE 2.f

using namespace std;

DECLARE_DELEGATE_OneParam(FDele_Player_Aimrate, float);

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UInventory* pInventory;

public:
	ATPSPortfolioCharacter();
	~ATPSPortfolioCharacter();

private:
	void initialize();
	void InitializeInputContext();
	void InitializeMeshComponent();
	void InitializeDefaultComponent();
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
	void CameraControl(float DeltaSeconds);
	void CaculateTotalWalkSpeed(float DeltaSeconds);
	void SetAimRate(eCharacterState eChangeState);

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
	float GetYCrossAngle() { return fYCrossAngle; }

	UFUNCTION(BlueprintCallable, Category = TPSCharater)
	float GetAimAngle() { return fAimAngle; }

	UFUNCTION(BlueprintCallable, Category = TPSCharater)
	float GetFowardValue() { return fFowardValue; }

	UFUNCTION(BlueprintCallable, Category = TPSCharater)
	bool GetisAiming() { return bIsAiming; }

	UFUNCTION(BlueprintCallable, Category = TPSCharater)
	bool GetisAimTurn() { return bIsAimTurn; }

	UFUNCTION(BlueprintCallable, Category = TPSCharater)
	eCharacterState GetCharacterState();

public:
	FVector GetChangeVector() { return vChangeDirection; }
	FVector GetLerpVector() { return vLerpDirection; }
	FVector GetControlVector(bool IsFoward = true);
	FVector GetControlVectorLastUpdated(bool IsFoward = true);
	FVector GetAimPosVector() {return vAimPosition;}

	float GetDefaultWalkSpeed() { return fDefaultWalkSpeed; }
	float GetDefaultRunSpeed() { return fRunSpeed; }
	float GetDefaultSprintSpeed() { return fSprintSpeed; }

	float GetWalkSpeed();
	bool GetIsMoving() { return bIsMoving; }
	bool GetIsSprint() {return bIsSprint;}

	void SetLerpVector(FVector LerpVecter) { vLerpDirection = LerpVecter; }
	void SetCrossAngle(float CrossAngle) { fCrossAngle = CrossAngle; }
	void SetYCrossAngle(float YCrossAngle) { fYCrossAngle = YCrossAngle; }
	void SetIsBraking(bool IsBraking) {bIsBraking = IsBraking;}
	void SetIsMoving(bool IsMoving) { bIsMoving = IsMoving;}
	void SetWalkSpeed(float WalkSpeed);
	void SetBrakeSpeed(float BrakeSpeed);
	void SetIsAiming(bool IsAiming) { bIsAiming = IsAiming; }
	void SetFowardValue(float FowardValue) { fFowardValue = FowardValue; }
	void SetIsAimTurn(bool IsAimTurn) { bIsAimTurn = IsAimTurn; }
	void SetBraking(float fTime);
private:
	TPSCharacterState* stCharacterState;
	vector<unique_ptr<TPSCharacterState>> vecState;

	AWeapon* pCurWeapon;
	FVector vControlVectorX;
	FVector vControlVectorY;
	FVector vChangeDirection;
	FVector vLerpDirection;
	FVector vAimPosition;

	float fCrossAngle;
	float fYCrossAngle;
	float fAimAngle;
	float fFowardValue;
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
	bool bIsAimTurn;
public:
	FDele_Player_Aimrate func_Player_Aimrate;
};
