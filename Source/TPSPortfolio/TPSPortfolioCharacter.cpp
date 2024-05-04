// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSPortfolioCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "TPSPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "IdleCharacterState.h"
#include "RunCharacterState.h"
#include "BrakeCharacterState.h"
#include "SprintCharacterState.h"
#include "AimCharacterState.h"
#include "CrossHair.h"
#include "AWeapon.h"

//////////////////////////////////////////////////////////////////////////
// ATPSPortfolioCharacter

ATPSPortfolioCharacter::ATPSPortfolioCharacter()
{
	InitializeDefaultComponent();
	InitializeInputContext();
	InitializeMeshComponent();

	initialize();
}

ATPSPortfolioCharacter::~ATPSPortfolioCharacter()
{
	vecState.clear();
	func_Player_Bulletrate.Unbind();
	func_Player_Aimrate.Clear();
	if (pCurWeapon != nullptr && IsValid(pCurWeapon))
	{
		pCurWeapon->Destroy();
	}
}

void ATPSPortfolioCharacter::initialize()
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 750.f;

	vChangeDirection = GetActorForwardVector();
	vLerpDirection = vChangeDirection;
	vControlVectorX = FVector(1.f, 0.f, 0.f);
	vControlVectorY = FVector(1.f, 0.f, 0.f);
	vAimPosition = FVector(1.f, 0.f, 0.f);

	fCrossAngle = 0.f;
	fYCrossAngle = 0.f;
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
	bIsAimTurn = false;

	//auto stState = new IdleState();
	//auto stState = new IdleCharacterState(this);
	vecState.emplace_back(make_unique<IdleCharacterState>(this));
	vecState.emplace_back(make_unique<RunCharacterState>(this));
	vecState.emplace_back(make_unique<BrakeCharacterState>(this));
	vecState.emplace_back(make_unique<SprintCharacterState>(this));
	vecState.emplace_back(make_unique<AimCharacterState>(this));

	stCharacterState = vecState[0].get();
}

void ATPSPortfolioCharacter::InitializeInputContext()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> FObj_InputContext(TEXT("/Game/ThirdPerson/Input/IMC_Default.IMC_Default"));
	if (FObj_InputContext.Succeeded())
	{
		UE_LOG(LogTemp, Log, TEXT("InputTest"));
		DefaultMappingContext = FObj_InputContext.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction>IA_Jump
	(TEXT("/Game/ThirdPerson/Input/Actions/IA_Jump.IA_Jump"));
	if (IA_Jump.Succeeded())
		JumpAction = IA_Jump.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_Aim
	(TEXT("/Game/ThirdPerson/Input/Actions/IA_Aim.IA_Aim"));
	if (IA_Aim.Succeeded())
		AimAction = IA_Aim.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_Look
	(TEXT("/Game/ThirdPerson/Input/Actions/IA_Look.IA_Look"));
	if (IA_Look.Succeeded())
		LookAction = IA_Look.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_Move
	(TEXT("/Game/ThirdPerson/Input/Actions/IA_Move.IA_Move"));
	if (IA_Move.Succeeded())
		MoveAction = IA_Move.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_Run
	(TEXT("/Game/ThirdPerson/Input/Actions/IA_Run.IA_Run"));
	if (IA_Run.Succeeded())
		RunAction = IA_Run.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_Sprint
	(TEXT("/Game/ThirdPerson/Input/Actions/IA_Sprint.IA_Sprint"));
	if (IA_Sprint.Succeeded())
		SprintAction = IA_Sprint.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_Attack
	(TEXT("/Game/ThirdPerson/Input/Actions/IA_Attack.IA_Attack"));
	if (IA_Attack.Succeeded())
		AttackAction = IA_Attack.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_Reload
	(TEXT("/Game/ThirdPerson/Input/Actions/IA_Reload.IA_Reload"));
	if (IA_Reload.Succeeded())
		ReloadAction = IA_Reload.Object;
}

void ATPSPortfolioCharacter::InitializeMeshComponent()
{
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, 270.f, 0.f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FObj_Skmesh(TEXT("/Game/Characters/PlayerCharacters/Vepley/Mesh/Vepley.Vepley"));
	if (FObj_Skmesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(FObj_Skmesh.Object);
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> FObj_Anim(TEXT("Blueprint'/Game/Characters/PlayerCharacters/Vepley/ABP_Vepley.ABP_Vepley_C'"));
	if (FObj_Anim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(FObj_Anim.Class);
	}

	static ConstructorHelpers::FObjectFinder<UPhysicsAsset> FObj_Physics(TEXT("/Game/Characters/PlayerCharacters/Vepley/Mesh/Vepleysize10_Physics.Vepleysize10_Physics"));
	if (FObj_Physics.Succeeded())
	{
		GetMesh()->SetPhysicsAsset(FObj_Physics.Object);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName(TEXT("TPSCharacter"));
	}
}

void ATPSPortfolioCharacter::InitializeDefaultComponent()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 90.0f);
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 350.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.f, 45.f, 70.f);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	pInventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));
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


	FName WeaponSocket(TEXT("r_hand_socket"));
	pCurWeapon = pInventory->LoadWeapon(0);
	pCurWeapon->SetPlayer(this);
	//pCurWeapon = GetWorld()->SpawnActor<AWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);

	if (GetMesh()->DoesSocketExist(WeaponSocket) && pCurWeapon != nullptr)
	{
		pCurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	}

	if (ATPSPlayerController* TPSController = Cast<ATPSPlayerController>(Controller))
	{
		if (TPSController->CrossHairHUDWidget != nullptr)
		{
			Cast<UCrossHair>(TPSController->CrossHairHUDWidget)->BindUserAimRate(this);
		}	
	}
}

void ATPSPortfolioCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Timer(DeltaSeconds);
	CameraControl(DeltaSeconds);
	UpdateState(DeltaSeconds);
}

eCharacterState ATPSPortfolioCharacter::GetCharacterState()
{
	if (stCharacterState == nullptr) return eCharacterState::IDLE;

	return stCharacterState->eState;
}

FVector ATPSPortfolioCharacter::GetControlVector(bool IsFoward)
{
	if (Controller == nullptr)
		return vChangeDirection;
	
		// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	return IsFoward ?
		 FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X):
		 FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
}

FVector ATPSPortfolioCharacter::GetControlVectorLastUpdated(bool IsFoward)
{
	// get forward vector
	return IsFoward ?
		vControlVectorX :
		vControlVectorY;
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
	
		//Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ATPSPortfolioCharacter::Attack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ATPSPortfolioCharacter::AttackComplete);

		//Reloading
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ATPSPortfolioCharacter::Reload);
		//EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Completed, this, &ATPSPortfolioCharacter::AttackComplete);

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

		vControlVectorX = ForwardDirection;
		vControlVectorY = RightDirection;

		//움직임에 따른 회전 방향값 지정
		SetMoveDirection(ForwardDirection, RightDirection, MovementVector);
		SetIsMoving(true);
		ChangeState(bIsSprint ? eCharacterState::SPRINT : eCharacterState::RUN);
	}
}

void ATPSPortfolioCharacter::MoveComplete()
{
	SetIsMoving(false);
	if(!bIsBraking)
	ChangeState(eCharacterState::IDLE);
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

		//if (bIsAiming)
		//{
		//	const FRotator rotControl = Controller->GetControlRotation();
		//	const FRotator rotControl_Pitch(rotControl.Pitch, 0, 0);
		//	fAimAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::UpVector.Dot(rotControl_Pitch.Vector()))) / 180.f;
		//}
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
	if (nullptr == pCurWeapon) return;

	pCurWeapon->AttackStart();
	func_Player_Bulletrate.ExecuteIfBound(pCurWeapon->GetBulletrate());
}

void ATPSPortfolioCharacter::AttackComplete()
{
	if (nullptr == pCurWeapon) return;
	UE_LOG(LogTemp, Log, TEXT("AttackStop"));
	pCurWeapon->AttackStop();
}

void ATPSPortfolioCharacter::Reload()
{
	if (nullptr == pCurWeapon) return;
	if (pCurWeapon->IsFullCapacity()) return;
	if (!pCurWeapon->IsPosibleReload()) return;

	bIsReloading = true;
	pCurWeapon->ReloadStart();
	func_Player_Bulletrate.ExecuteIfBound(pCurWeapon->GetBulletrate());
}

void ATPSPortfolioCharacter::ReloadComplete()
{
	bIsReloading = false;
	if (nullptr == pCurWeapon) return;
	pCurWeapon->Reload();
	func_Player_Bulletrate.ExecuteIfBound(pCurWeapon->GetBulletrate());
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

void ATPSPortfolioCharacter::CameraControl(float DeltaSeconds)
{
	if (FollowCamera == nullptr)
		return;

	float fCurFOV = FollowCamera->FieldOfView;
	float fTargetFOV = bIsAiming ? 90.f : 100.f;
	float fLerpFOV = FMath::FInterpTo(fCurFOV, fTargetFOV, DeltaSeconds, CAMERA_FOV_SPEED);
	FollowCamera->SetFieldOfView(fLerpFOV);

	float fCurLength = CameraBoom->TargetArmLength;
	float fTargetLength = bIsAiming ? 200.f : 350.f;
	float fLerpLength = FMath::FInterpTo(fCurLength, fTargetLength, DeltaSeconds, CAMERA_FOV_SPEED);
	CameraBoom->TargetArmLength = fLerpLength;

	FVector vCurCamera = CameraBoom->SocketOffset;
	FVector vTarget = bIsAiming ? FVector(0.f, 60.f, 80.f) : FVector(0.f, 45.f ,70.f);
	FVector vLerp = FMath::VInterpConstantTo(vCurCamera, vTarget, DeltaSeconds, CAMERA_FOV_SPEED);
	
	CameraBoom->SocketOffset = vLerp;

	//----------------------LayTrace-------------------------
	if (!bIsAiming)
	{
		fAimAngle = 0.f;
		return;
	}


	FVector vecWorldLocation = FollowCamera->GetComponentLocation();
	FVector vecCameraLook = Controller->GetControlRotation().Vector();
	FVector vecTarget = vecWorldLocation + vecCameraLook * 3000.f;
	FHitResult FHresult;
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(FHresult, vecWorldLocation, vecTarget, ECollisionChannel::ECC_OverlapAll_Deprecated, collisionParams))
		vAimPosition = FHresult.Location;
	else
		vAimPosition = vecTarget;

	FVector vTargetPos = GetActorLocation();
	vTargetPos.Z += 60.f;
	const FRotator rotControl = (vAimPosition- vTargetPos).GetSafeNormal().Rotation();
	const FRotator rotControl_Pitch(rotControl.Pitch, 0, 0);
	const FVector vControlPitch = rotControl_Pitch.Vector();
	const FRotator rotActor = GetActorForwardVector().Rotation();
	const FRotator rotActor_Pitch(rotActor.Pitch, 0, 0);
	const FVector vActorPitch = rotActor_Pitch.Vector();
	fAimAngle = FMath::RadiansToDegrees(FMath::Acos(vActorPitch.Dot(vControlPitch)));
	if (vActorPitch.Cross(vControlPitch).Y < 0.f)
		fAimAngle *= -1.f;

	//DrawDebugLine(GetWorld(), vecWorldLocation, vecTarget, FColor::Red, false, 1.0f);
	//UE_LOG(LogTemp, Log, TEXT("fAimAngle : %f"), fAimAngle);
}

void ATPSPortfolioCharacter::CaculateTotalWalkSpeed(float DeltaSeconds)
{
	float fMaxSpeed = bIsBraking ? 0.f : !bIsMoving ? 0.f : bIsSprint ? fSprintSpeed : bIsRun ? fRunSpeed : fDefaultWalkSpeed;

	float fLerpSpeed = bIsBraking ? BRAKE_DECLEASE : !bIsMoving ? IDLE_DECLEASE : WALKSPEED_DECLEASE;
	GetCharacterMovement()->BrakingDecelerationWalking = bIsBraking ? BRAKE_DECLEASE : IDLE_DECLEASE;

	GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpConstantTo(GetCharacterMovement()->MaxWalkSpeed, fMaxSpeed, DeltaSeconds, fLerpSpeed);

	//UE_LOG(LogTemp, Log, TEXT("MaxSpeed : %f, LerpSpeed : %f"), GetCharacterMovement()->MaxWalkSpeed, fLerpSpeed);
}

void ATPSPortfolioCharacter::SetAimRate(eCharacterState eChangeState)
{
	float fAimrate = 0.f;
	switch (eChangeState)
	{
	case eCharacterState::IDLE :
	case eCharacterState::BRAKE: fAimrate = 1.f;
		break;
	case eCharacterState::RUN: fAimrate = bIsAiming ? 0.5f : 2.f;
		break;
	case eCharacterState::SPRINT: fAimrate = bIsAiming ? 1.f : 3.f;
		break;

	case eCharacterState::AIM: fAimrate = 0.f;
		break;
	}
	if(func_Player_Aimrate.IsBound())
	func_Player_Aimrate.Broadcast(fAimrate);
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

	SetAimRate(eChangeState);

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

