
#include "Characters/Characters.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimMontage.h"

ACharacters::ACharacters()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 300.0f, 0.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(FName("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	Head = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Head"));
	Head->SetupAttachment(GetMesh());

	Hair = CreateDefaultSubobject<UGroomComponent>(FName("Hair"));
	Hair->SetupAttachment(Head);

	Eyebros = CreateDefaultSubobject<UGroomComponent>(FName("Eyebros"));
	Eyebros->SetupAttachment(Head);
}

void ACharacters::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = CastChecked<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(CharacterMappingContext, 0);
		}
	}

	if (Head && GetMesh())
	{
		Head->SetLeaderPoseComponent(GetMesh());
	}
}

void ACharacters::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ACharacters::EquipEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
}

void ACharacters::DodgeEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ACharacters::UnequipEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	CharacterState = ECharacterState::ECS_Unequipped;
}

void ACharacters::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeahToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ACharacters::DisArm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeahToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ACharacters::DodgeMovement(float DeltaTime)
{
	FVector Movement = DodgeDirection * DodgeSpeed * DeltaTime;

	AddActorWorldOffset(Movement, true);

	DodgeTime += DeltaTime;

	if (DodgeTime >= DodgeDuration)
	{
		bDodgeMovement = false;
	}
}

void ACharacters::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && Value.IsNonZero())
	{
		GetCharacterMovement()->MaxWalkSpeed = 250.0f;
		const FVector2D MovementVector = Value.Get<FVector2D>();

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MovementVector.Y);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ACharacters::Run()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
}

void ACharacters::Look(const FInputActionValue& Value)
{
	if (Controller && Value.IsNonZero())
	{
		const FVector2D LookAxisVector = Value.Get<FVector2D>();
		AddControllerPitchInput(LookAxisVector.Y);
		AddControllerYawInput(LookAxisVector.X);
	}
}

void ACharacters::Jump()
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

	if (CanJump())
	{
		Super::Jump();
		ActionState = EActionState::EAS_Jumpping;
	}
}

void ACharacters::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (ActionState == EActionState::EAS_Jumpping)
	{
		ActionState = EActionState::EAS_Unoccupied;
	}
}

void ACharacters::Equip()
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		OverlappingItem = nullptr;
		EquippedWeapon = OverlappingWeapon;
	}
	else
	{
		if (CanDisArm())
		{
			ActionState = EActionState::EAS_Equipping;
			PlayEquipMontage(FName("Unequip"));
		}
		else if (CanArm())
		{
			ActionState = EActionState::EAS_Equipping;
			PlayEquipMontage(FName("Equip"));
		}
	}
}

void ACharacters::Attack()
{
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void ACharacters::Dodge()
{
	if (CanDodge())
	{
		DodgeDirection = GetLastMovementInputVector();
		if (DodgeDirection.IsNearlyZero())
		{
			DodgeDirection = GetActorForwardVector();
		}
		DodgeDirection.Normalize();

		PlayDodgeMontage();
		ActionState = EActionState::EAS_Dodgeing;

		DodgeTime = 0.0f;
		bDodgeMovement = true;
	}
}

bool ACharacters::CanJump()
{
	if (GetCharacterMovement()->IsFalling())
	{
		return false;
	}
	return ActionState == EActionState::EAS_Unoccupied;
}

bool ACharacters::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped && !GetCharacterMovement()->IsFalling();
}

bool ACharacters::CanDodge()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

bool ACharacters::CanArm()
{
	return CharacterState == ECharacterState::ECS_Unequipped && ActionState == EActionState::EAS_Unoccupied && EquippedWeapon;
}

bool ACharacters::CanDisArm()
{
	return CharacterState != ECharacterState::ECS_Unequipped && ActionState == EActionState::EAS_Unoccupied;
}

void ACharacters::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, 1.75f);
		const int32 Selection = FMath::RandRange(0, 1);
		FName SelectionName = FName();
		switch (Selection)
		{
		case 0:
			SelectionName = FName("Attack1");
			break;
		case 1:
			SelectionName = FName("Attack2");
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SelectionName, AttackMontage);
	}
}

void ACharacters::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ACharacters::PlayDodgeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DodgeMontage && GetCharacterMovement())
	{
		float Velocity = GetVelocity().Size2D();
		if (Velocity <= MaxWalkSpeed)
		{
			DodgeSpeed = 350.0f;
			AnimInstance->Montage_Play(DodgeMontage);
			AnimInstance->Montage_JumpToSection(FName("WalkingDodge"));

		}
		else if (Velocity <= MaxRunSpeed)
		{
			DodgeSpeed = 450.0f;
			AnimInstance->Montage_Play(DodgeMontage);
			AnimInstance->Montage_JumpToSection(FName("RunningDodge"));
		}
	}
}

void ACharacters::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);
	if (bDodgeMovement)
	{
		DodgeMovement(DeltaTime);
	}
}

void ACharacters::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(CharacterMoveAction, ETriggerEvent::Triggered, this, &ACharacters::Move);
		EnhancedInputComponent->BindAction(CharacterRunAction, ETriggerEvent::Triggered, this, &ACharacters::Run);
		EnhancedInputComponent->BindAction(CharacterLookAction, ETriggerEvent::Triggered, this, &ACharacters::Look);
		EnhancedInputComponent->BindAction(CharacterJumpAction, ETriggerEvent::Started, this, &ACharacters::Jump);
		EnhancedInputComponent->BindAction(CharacterEquipAction, ETriggerEvent::Started, this, &ACharacters::Equip);
		EnhancedInputComponent->BindAction(CharacterAttackAction, ETriggerEvent::Started, this, &ACharacters::Attack);
		EnhancedInputComponent->BindAction(CharacterDodgeAction, ETriggerEvent::Started, this, &ACharacters::Dodge);
	}
}

void ACharacters::SetWeaponCollisionEnable(ECollisionEnabled::Type CollisionEnable)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnable);
	}
}