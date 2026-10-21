#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Characters/CharacterTypes.h"
#include "Characters.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class AWeapon;
class UParticleSystemComponent;

UCLASS()
class OPENWORLD_API ACharacters : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacters();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnable(ECollisionEnabled::Type CollisionEnable);
	

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* CharacterMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CharacterMoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CharacterRunAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CharacterLookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CharacterJumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CharacterDodgeAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CharacterEquipAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CharacterAttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CharacterDashAttackAction;

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void AttackRecoveryEnd();

	UFUNCTION(BlueprintCallable)
	void DashAttackEnd();

	UFUNCTION(BlueprintCallable)
	void EquipEnd(); 

	UFUNCTION(BlueprintCallable)
	void DodgeEnd();

	UFUNCTION(BlueprintCallable)
	void UnequipEnd();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void DisArm();

	UFUNCTION(BlueprintCallable)
	void DodgeMovement(float DeltaTime);
	
	void Move(const FInputActionValue& Value);
	void Run();
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	virtual void Landed(const FHitResult& Hit) override;
	void Equip();
	void Attack();
	void DashAttack();
	void Dodge();
	bool CanJump();
	bool CanAttack();
	bool CanDodge();
	bool CanArm();
	bool CanDisArm();

	void PlayAttackMontage();
	void PlayEquipMontage(const FName& SectionName);
	void PlayDashAttackMontage();
	void PlayDodgeMontage();

	UPROPERTY(EditDefaultsOnly)
	float MaxWalkSpeed = 250.0f;

	UPROPERTY(EditDefaultsOnly)
	float MaxRunSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly)
	float WalkDodgeSpeed = 250.0f;

	UPROPERTY(EditDefaultsOnly)
	float RunDodgeSpeed = 450.0f;

	UPROPERTY(EditDefaultsOnly)
	float DodgeSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	float DodgeDuration = 1.0f;

private:
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Head;

	UPROPERTY(VisibleAnywhere)
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere)
	UGroomComponent* Eyebros;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DashAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DodgeMontage;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* LeafEmitterComponent;

	UPROPERTY(VisibleAnywhere)
	bool bDodgeMovement = false;

	float DodgeTime = 0.0f;

	FVector DodgeDirection = FVector::ZeroVector;

	int32 ComboStep = 0;

	bool bComboQueued = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bCanQueueCombo = false;
public: 
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE AItem* GetOverlappingItem() const { return OverlappingItem; }
	FORCEINLINE void SetEquippedWeapon(AWeapon* Weapon) { EquippedWeapon = Weapon; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState;  }
	FORCEINLINE void SetCharacterState(const ECharacterState State) { CharacterState = State; }
	FORCEINLINE void SetActionState(const EActionState State) { ActionState = State; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCanQueueCombo(bool Status) { bCanQueueCombo = Status; }
};
