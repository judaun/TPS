// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "TPSPortfolioCharacter.generated.h"

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

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RunAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

public:
	ATPSPortfolioCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void MoveComplete();
	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void SetMoveDirection(const FVector& vFoward, const FVector& vRight, const FVector2D& vMoveVector);
	void Turn(float DeltaSeconds);
			
	void Run() { bIsRun = !bIsRun; }
	void Sprint();
	void StopSprint();

	void CaculateTotalWalkSpeed(float DeltaSeconds);
	void SetBraking(float fTime);

	void Timer(float DeltaSeconds);
	void SlideGround(float DeltaSeconds);
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

private:
	FVector vChangeDirection;
	FVector vLerpDirection;

	float fCrossAngle;
	float fTurnSpeed;
	float fDefaultWalkSpeed;
	float fRunSpeedMultiplier;
	float fSprintSpeedMultiplier;
	float fBrakeTimer;
	bool bIsRun;
	bool bIsSprint;
	bool bIsMoving;

	
	bool bIsBraking;
};