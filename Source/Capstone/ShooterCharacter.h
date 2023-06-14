// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class CAPSTONE_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

	// �޴� ������ ó��
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** ��, �� �Է½� ȣ�� */
	void MoveForward(float Value);

	/** ��, �� �Է½� ȣ�� */
	void MoveRight(float Value);

	/** ����Ű ������ ȣ�� */
	void Jump();

	/**
	* ���� �Է��� ���� �־��� ����(Rate) ��ŭ ȸ���Ѵ�.
	* @param Rate : ����ȭ�� ����, 1.0�� ���ϴ� ȸ�� �ӵ��� 100%�� �ǹ�
	*/
	void TurnAtRate(float Rate);

	/**
	* ���� �Է��� ���� �־��� ����(Rate) ��ŭ look up/down �Ѵ�.
	* @param Rate : ����ȭ�� ����, 1.0�� ���ϴ� ȸ�� �ӵ��� 100%�� �ǹ�
	*/
	void LookUpAtRate(float Rate);

	/** Fire Button�� �������� ȣ�� */
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);

	/** ��ư �Է����� bAming ���� ture or false�� ���� */
	void AimingButtonPressed();
	void AimingButtonReleased();

	/** ���ؽ� ī�޶� ��ȯ �ε巴�� */
	void CameraInterpZoom(float DeltaTime);

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	/** ĳ���Ͱ� ���� ��� �ִ°� */
	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();

	void Die();

	UFUNCTION(BlueprintCallable)
	void DyingCharacter();

	void DoDamage(AActor* Victim);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/**
	* Camera boom(ī�޶� ��)�� ĳ���� �ڿ� ī�޶� ��ġ ��Ų��.
	* �����ڸ� ȣ������ �ʱ����� �����ͺ����� ����
	* ������ ������ ��ü�� �������� �ʰ� ��ü�� ���� ������ ������ �� �����Ƿ� �޸� ��뷮�� ����
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* CameraBoom;

	// ī�޶� ĳ���͸� ���󰣴�.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* FollowCamera;

	// �⺻ ȸ�� �ӵ�. �ʴ� ȸ������ ������ �ٸ� �����ϸ��� ���� ȸ�� �ӵ��� ������ �� �� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float BaseTurnRate;

	// �⺻ look up/down �ӵ�. �ʴ� ȸ������ ������ �ٸ� �����ϸ��� ���� ȸ�� �ӵ��� ������ �� �� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float BaseLookUpRate;

	// ���� �ѼҸ� sound cue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class USoundCue* FireSound;

	// RightEffectSocket ���� ���� ����Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* MuzzleFlash;

	// �߻縦 ���� ��Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UAnimMontage* HipFireMontage;

	// �Ѿ˿� ���� ����Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	UParticleSystem* ImpactParticles;

	// �Ѿ˿� ���� ����(��)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	UParticleSystem* BeamParticles;

	// ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float Damage;

	/** Aiming�� True �� �� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category = Combat, meta = (AllowPrivateAccess = "true"));
	bool bAiming;

	/** �⺻ ī�޶� ȭ�� �� �� */ 
	float CameraDefaultFOV;

	/** ���� ī�޶� ȭ�� �� �� */
	float CameraZoomedFOV;

	/** �� �������� ���� �þ� */
	float CameraCurrentFOV;

	/** ������ �� Ȯ�� �ӵ� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float ZoomInterpSpeed;

	/** ��Ŭ�� Ȥ�� ������ �ܼ� Ʈ���� ���� */
	bool bFireButtonPressed;

	/** �Ѿ��� �߻� �� �� ������ True, Ÿ�̸Ӹ� ��ٸ��� false */
	bool bShouldFire;

	/** �ڵ���� ���� */
	float AutomaticFireRate;

	/** �߻� ������ Ÿ�̸� ���� ���� */
	FTimerHandle AutoFireTimer;

	/** ���� ���� ���� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player, meta = (AllowPrivateAccess = "true"));
	bool bCanFirstJump;

	/** ���� ���� ���� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player, meta = (AllowPrivateAccess = "true"));
	bool bCanDoubleJump;

	/** ���� ü�� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "ture"))
	float Health;

	/** �ִ� ü�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "ture"))
	float MaxHealth;

	/** ĳ���� ��� ��Ÿ�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player, meta = (AllowPrivateAccess = "true"));
	UAnimMontage* DieMontage;

public:
	/**
	* FORCEINLINE�� �Լ��� �ζ��� �Լ��� �����ϴ� ��ũ��
	* �ַ� ���� �Լ��� ����� ȣ��Ǵ� �Լ��� �ζ��� �Լ��� �����Ͽ� ���α׷��� ������ ���
	*/  

	// CameraBoom subobject�� ��ȯ�Ѵ�.
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// FollowCamera subobject�� ��ȯ�Ѵ�.
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Damage�� ��ȯ�Ѵ�.
	FORCEINLINE float GetDamage() const { return Damage; }
};
