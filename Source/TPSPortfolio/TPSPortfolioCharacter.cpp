// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSPortfolioCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "IdleCharacterState.h"
#include "RunCharacterState.h"
#include "BrakeCharacterState.h"
#include "SprintCharacterState.h"
#include "AimCharacterState.h"

//////////////////////////////////////////////////////////////////////////
// ATPSPortfolioCharacter

ATPSPortfolioCharacter::ATPSPortfolioCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//initialize
	initialize();

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

ATPSPortfolioCharacter::~ATPSPortfolioCharacter()
{
	vecState.clear();
}

void ATPSPortfolioCharacter::initialize()
{
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 750.f;

	vChangeDirection = GetActorForwardVector();
	vLerpDirection = vChangeDirection;

	fCrossAngle = 0.f;
	fTurnSpeed = 360.f;
	fDefaultWalkSpeed = 150.f;
	fRunSpeed = 450.f;
	fSprintSpeed = 800.f;
	fBrakeTimer = 0.f;
	bIsRun = true;
	bIsSprint = false;
	bIsMoving = false;
	bIsBraking = false;
	bIsAiming = false;

	//auto stState = new IdleState();
	//auto stState = new IdleCharacterState(this);
	vecState.emplace_back(make_unique<IdleCharacterState>(this));
	vecState.emplace_back(make_unique<RunCharacterState>(this));
	vecState.emplace_back(make_unique<BrakeCharacterState>(this));
	vecState.emplace_back(make_unique<SprintCharacterState>(this));
	vecState.emplace_back(make_unique<AimCharacterState>(this));

	stCharacterState = vecState[0].get();
}

void ATPSPortfolioCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ATPSPortfolioCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Timer(DeltaSeconds);
	SlideGround(DeltaSeconds);
	UpdateState(DeltaSeconds);
}


FVector ATPSPortfolioCharacter::GetControlVector()
{
	if (Controller == nullptr)
		return vChangeDirection;
	
		// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	return FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
}

float ATPSPortfolioCharacter::GetWalkSpeed()
{
	return GetCharacterMovement()->MaxWalkSpeed;
}

void ATPSPortfolioCharacter::SetWalkSpeed(float WalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ATPSPortfolioCharacter::SetBrakeSpeed(float BrakeSpeed)
{
	GetCharacterMovement()->BrakingDecelerationWalking = BrakeSpeed;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATPSPortfolioCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Run
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &ATPSPortfolioCharacter::Run);

		//Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ATPSPortfolioCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATPSPortfolioCharacter::StopSprint);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATPSPortfolioCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ATPSPortfolioCharacter::MoveComplete);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATPSPortfolioCharacter::Look);

		//Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ATPSPortfolioCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ATPSPortfolioCharacter::AimComplete);
	}

}

void ATPSPortfolioCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		//움직임에 따른 회전 방향값 지정
		SetMoveDirection(ForwardDirection, RightDirection, MovementVector);
		SetIsMoving(true);
		ChangeState(bIsSprint ? eCharacterState::SPRINT : eCharacterState::RUN);
	}
}

void ATPSPortfolioCharacter::MoveComplete()
{
	ChangeState(eCharacterState::IDLE);
	SetIsMoving(false);
}

void ATPSPortfolioCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATPSPortfolioCharacter::Aim()
{
	ChangeState(eCharacterState::AIM);
}

void ATPSPortfolioCharacter::AimComplete()
{
	ChangeState(eCharacterState::IDLE);
}

void ATPSPortfolioCharacter::Attack()
{

}

void ATPSPortfolioCharacter::AttackComplete()
{

}

void ATPSPortfolioCharacter::SetMoveDirection(const FVector& vFoward, const FVector& vRight, const FVector2D& vMoveVector)
{
	//움직임을 캐릭터 방향벡터를 기준으로 움직이도록 설정
	if (vMoveVector.X == 0 && vMoveVector.Y == 0) return;
	
	vChangeDirection = 
#pragma region LEFT / RIGHT	
		vMoveVector.Y == 0 ? vMoveVector.X > 0 ? vRight : -vRight :
#pragma endregion					
#pragma region FOWARD / FR / FL
		vMoveVector.Y > 0 ? vMoveVector.X == 0 ? vFoward : vMoveVector.X > 0 ? (vFoward + vRight).GetSafeNormal() : (vFoward - vRight).GetSafeNormal() :
#pragma endregion		
#pragma region BACK / BR / BL
		vMoveVector.X == 0 ? -vFoward : vMoveVector.X > 0 ? (-vFoward + vRight).GetSafeNormal() : (-vFoward - vRight).GetSafeNormal();
#pragma endregion

}

void ATPSPortfolioCharacter::Turn(float DeltaSeconds)
{
	if (!bIsMoving || bIsAiming) return;

	const FVector vActorForwardDirection = GetActorForwardVector();
	//내적으로 회전값 구하기

	double dArccosRadian = FMath::Acos(vActorForwardDirection.Dot(vChangeDirection));
	float fDegree = FMath::RadiansToDegrees(dArccosRadian);

	float fCurrentWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	if (fDegree > MAX_TURN_DGREE && fCurrentWalkSpeed > RUN_CONDITION) SetBraking(fCurrentWalkSpeed > SPRINT_CONDITION ? BRAKE_SPRINT : BRAKE_RUN);

	//외적으로 회전방향 구하기
	if (vActorForwardDirection.Cross(vChangeDirection).Z < 0)
		fDegree *= -1;

	float fCalcTurnSpeed = fCurrentWalkSpeed > SPRINT_TURN_DCREASE ? fTurnSpeed / (fCurrentWalkSpeed / SPRINT_TURN_DCREASE) : fTurnSpeed;

	//선형보간으로 회전속도 조절
	double rTurnRot = FMath::FInterpConstantTo(0, fDegree, DeltaSeconds, fCalcTurnSpeed);


	//좌우 기울기 움직임 값 적용을 위한 벡터의 점진적 회전 적용
	float fLerpDegree = FMath::RadiansToDegrees(FMath::Acos(vLerpDirection.Dot(vChangeDirection)));
	if (vLerpDirection.Cross(vChangeDirection).Z < 0)
		fLerpDegree *= -1;
	double rLerpTurnRot = FMath::FInterpConstantTo(0, fLerpDegree, DeltaSeconds, fCalcTurnSpeed * 2.f);
	vLerpDirection = vLerpDirection.RotateAngleAxis(rLerpTurnRot, FVector::UpVector);

	fCrossAngle = vActorForwardDirection.Cross(vLerpDirection).Z;

	if (!bIsBraking)
		AddActorWorldRotation(FRotator(0, rTurnRot, 0));

}

void ATPSPortfolioCharacter::CaculateTotalWalkSpeed(float DeltaSeconds)
{
	float fMaxSpeed = bIsBraking ? 0.f : !bIsMoving ? 0.f : bIsSprint ? fSprintSpeed : bIsRun ? fRunSpeed : fDefaultWalkSpeed;

	float fLerpSpeed = bIsBraking ? BRAKE_DECLEASE : !bIsMoving ? IDLE_DECLEASE : WALKSPEED_DECLEASE;
	GetCharacterMovement()->BrakingDecelerationWalking = bIsBraking ? BRAKE_DECLEASE : IDLE_DECLEASE;

	GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpConstantTo(GetCharacterMovement()->MaxWalkSpeed, fMaxSpeed, DeltaSeconds, fLerpSpeed);

	UE_LOG(LogTemp, Log, TEXT("MaxSpeed : %f, LerpSpeed : %f"), GetCharacterMovement()->MaxWalkSpeed, fLerpSpeed);
}

void ATPSPortfolioCharacter::SetBraking(float fTime)
{
	ChangeState(eCharacterState::BRAKE);
	fBrakeTimer = fTime;
}

void ATPSPortfolioCharacter::Timer(float DeltaSeconds)
{
	//Timer
	if (fBrakeTimer > 0.f) fBrakeTimer -= DeltaSeconds;
	else if (bIsBraking)
	{
		ChangeState(eCharacterState::IDLE);
	}
}

void ATPSPortfolioCharacter::SlideGround(float DeltaSeconds)
{

}

void ATPSPortfolioCharacter::UpdateState(float DeltaSeconds)
{
	if (stCharacterState == nullptr) return;

	stCharacterState->Update(DeltaSeconds);
}

void ATPSPortfolioCharacter::ChangeState(eCharacterState eChangeState)
{
	if (stCharacterState == nullptr || stCharacterState->eState == eChangeState) return;

	if ((bIsBraking || bIsAiming) && eChangeState != eCharacterState::IDLE)
		return;

	for (int i = 0; i < vecState.size(); ++i)
	{
		if (vecState[i]==nullptr || eChangeState != vecState[i]->eState)
			continue;

		stCharacterState->Exit();
		stCharacterState = vecState[i].get();
		stCharacterState->Enter();
		break;
	}
}

