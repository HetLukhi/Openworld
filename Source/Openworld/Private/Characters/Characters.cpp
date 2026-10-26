
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
#include "Particles/ParticleSystemComponent.h"

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

	LeafEmitterComponent = CreateDefaultSubobject<UParticleSystemComponent>(FName("LeafEmitter"));
	LeafEmitterComponent->SetupAttachment(GetMesh());

	LeafEmitterComponent->SetUsingAbsoluteRotation(true);
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
	if (bComboQueued)
	{
		bComboQueued = false;
		ComboStep++;

		PlayAttackMontage();
	}
}

void ACharacters::AttackRecoveryEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	ComboStep = 0;
	bComboQueued = false;
}

void ACharacters::DashAttackEnd()
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
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ACharacters::DisArm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ACharacters::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	CachedMoveInput = MovementVector;

	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && Value.IsNonZero())
	{
		GetCharacterMovement()->MaxWalkSpeed = 250.0f;
		

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
	if (OverlappingItem)
	{
		IEquipInterface* EquipInterface = Cast<IEquipInterface>(OverlappingItem);
		if (EquipInterface)
		{
			EquipInterface->Execute_Equip(OverlappingItem, this);
		}
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
	if (ActionState == EActionState::EAS_Attacking)
	{
		if (bCanQueueCombo && ComboStep < 3)
		{
			bComboQueued = true;
		}
		return;
	}
	if (CanAttack())
	{
		ComboStep = 1;
		bCanQueueCombo = false;

		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void ACharacters::DashAttack()
{
	if (CanAttack())
	{
		PlayDashAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void ACharacters::Dodge()
{
	if (CanDodge())
	{
		PlayDodgeMontage();
		ActionState = EActionState::EAS_Dodgeing;
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
		if (!AnimInstance->Montage_IsPlaying(AttackMontage))
		{
			AnimInstance->Montage_Play(AttackMontage);
		}
		FName SelectionName = FName();
		switch (ComboStep)
		{
		case 1:
			SelectionName = FName("Attack1");
			break;
		case 2:
			SelectionName = FName("Attack2");
			break;
		case 3:
			SelectionName = FName("Attack3");
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SelectionName, AttackMontage);
	}
}

void ACharacters::PlayDashAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DashAttackMontage)
	{
		AnimInstance->Montage_Play(DashAttackMontage);
		AnimInstance->Montage_JumpToSection(FName("DashAttack"));
	}
}

void ACharacters::PlayEquipMontage(const FName& SectionName)
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
			AnimInstance->Montage_Play(DodgeMontage);
			AnimInstance->Montage_JumpToSection(FName("WalkingDodge"));

		}
		else if (Velocity <= MaxRunSpeed)
		{
			AnimInstance->Montage_Play(DodgeMontage);
			AnimInstance->Montage_JumpToSection(FName("RunningDodge"));
		}
	}
}

void ACharacters::BeginAttackSteering()
{
	bCanSteerDuringAttack = true;
	AttackSteeringDirection = FVector::ZeroVector;
}

void ACharacters::UpdateAttackSteering()
{
	if (!bCanSteerDuringAttack) return;

	const FVector2D MovementVector = CachedMoveInput;

	if (MovementVector.IsNearlyZero()) return;

	const FRotator ControlRotation = GetControlRotation();
	const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	const FVector RightDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	FVector AttackDirection =
		ForwardDirection * MovementVector.Y +
		RightDirection * MovementVector.X;

	AttackDirection.Normalize();

	AttackSteeringDirection = AttackDirection;
}

void ACharacters::ApplyAttackSteeringDirection()
{
	bCanSteerDuringAttack = false;

	if (AttackSteeringDirection.IsNearlyZero()) return;

	SetActorRotation(AttackSteeringDirection.Rotation());
}

void ACharacters::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

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
		EnhancedInputComponent->BindAction(CharacterDashAttackAction, ETriggerEvent::Started, this, &ACharacters::DashAttack);
		EnhancedInputComponent->BindAction(CharacterDodgeAction, ETriggerEvent::Started, this, &ACharacters::Dodge);
	}
}

void ACharacters::SetWeaponCollisionEnable(ECollisionEnabled::Type CollisionEnable)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnable);
		EquippedWeapon->IgnoreActor.Empty();
	}
}