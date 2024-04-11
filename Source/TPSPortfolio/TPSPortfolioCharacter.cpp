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

	// Configure character movement
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	//GetCharacterMovement()->RotationRate = FRotator(0.0f, 250.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 500.f;

	//initialize changerotator
	const FRotator Rotation = GetActorRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	vChangeDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	fTurnSpeed = 5.f;
	fDefaultWalkSpeed = 150.f;
	fSprintSpeedMultiplier = 3.f;
	bIsSprint = false;
	bIsMoving = false;

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
	CaculateTotalWalkSpeed();
	Turn(DeltaSeconds);
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
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ATPSPortfolioCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATPSPortfolioCharacter::StopSprint);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATPSPortfolioCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ATPSPortfolioCharacter::MoveComplete);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATPSPortfolioCharacter::Look);

	}

}

void ATPSPortfolioCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	//X=LR Y=BF

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
		
		//�����ӿ� ���� ȸ�� ���Ⱚ ����
		SetMoveDirection(ForwardDirection, RightDirection, MovementVector);
		bIsMoving = true;
	}
}

void ATPSPortfolioCharacter::MoveComplete()
{
	bIsMoving = false;
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

void ATPSPortfolioCharacter::SetMoveDirection(const FVector& vFoward, const FVector& vRight, const FVector2D& vMoveVector)
{
	//�������� ĳ���� ���⺤�͸� �������� �����̵��� ����
	// Y>0 X=0 = Forward 
	// Y<0 X=0 = (-Foward)
	// Y>0 X>0 = Forward + Right
	// Y>0 X<0 = Forward - Right
	// Y<0 X>0 = (-Forward) + Right
	// Y<0 X<0 = (-Forward) - Right
	UE_LOG(LogTemp, Log, TEXT("Y : %f , X : %f"), vMoveVector.Y, vMoveVector.X);

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
	if (!bIsMoving) return;

	if (Controller != nullptr)
	{
		const FRotator rActorRot = GetActorRotation();
		const FRotator rActorYawRot(0, rActorRot.Yaw, 0);
		//Actor�� Direction
		const FVector vActorForwardDirection = FRotationMatrix(rActorYawRot).GetUnitAxis(EAxis::X);

		//�������� ȸ���� ���ϱ�
		double dArccosRadian = FMath::Acos(vActorForwardDirection.Dot(vChangeDirection));
		double dDgrees = FMath::RadiansToDegrees(dArccosRadian);
		//UE_LOG(LogTemp, Log, TEXT("Dgree : %f"), dDgrees);
		
		//�������� ȸ������ ���ϱ�
		if(vActorForwardDirection.Cross(vChangeDirection).Z < 0)
			dDgrees *= -1;

		//������������ ȸ���ӵ� ����
		double rTurnRot =  FMath::FInterpTo(0, dDgrees, DeltaSeconds, fTurnSpeed);
	
		AddActorWorldRotation(FRotator(0, rTurnRot, 0));
	}
}

void ATPSPortfolioCharacter::Sprint()
{
	bIsSprint = true;
}

void ATPSPortfolioCharacter::StopSprint()
{
	bIsSprint = false;
}

void ATPSPortfolioCharacter::CaculateTotalWalkSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = fDefaultWalkSpeed * (bIsSprint ? fSprintSpeedMultiplier : 1.f);
}


