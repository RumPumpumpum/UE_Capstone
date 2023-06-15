// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Capstone.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "HitInterface.h"
#include "Enemy.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	bAiming(false),
	CameraDefaultFOV(0.f), // BeginPlay���� ����
	CameraZoomedFOV(70.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),
	// �ڵ� �߻� ������
	AutomaticFireRate(0.2f),
	bShouldFire(true),
	bFireButtonPressed(false),
	Health(100.f),
	MaxHealth(100.f),
	bDied(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// CameraBoom ����(ĳ���� �ֺ��� �浹�� �ִ� ��� ī�޶� ���� ĳ���� ������ �����)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1000.f; // ī�޶�� ĳ���� �ڿ� �� �Ÿ����� ����
	CameraBoom->bUsePawnControlRotation = true; // ��Ʈ�ѷ��� ���缭 arm ȸ��
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 50.f); // ������ �Է��� x����, y����, z���� ���� ����

	// Follow Camera ����
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // boom���� ī�޶� ��ġ
	FollowCamera->bUsePawnControlRotation = true; // ī�޶�� arm�� ȸ���� ���󰡱⸸ �ϸ� ��

	// ��Ʈ�ѷ� ȸ���� ĳ���͸� ȸ������ ����. ��Ʈ�ѷ� ȸ���� ī�޶� ȸ��
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// ĳ���� �̵� ����
	GetCharacterMovement()->bOrientRotationToMovement = true; // ĳ���Ͱ� �̵� ���⿡ ���� ȸ���Ұ��ΰ�
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f); // ȸ���ӵ��� ����
	GetCharacterMovement()->JumpZVelocity = 300.f;
	GetCharacterMovement()->AirControl = 1.0f;
	AShooterCharacter::JumpMaxHoldTime = 0.15f; // ���� �ִ� �����ð�
	AShooterCharacter::JumpMaxCount = 2;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
}

void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// ��� ������ Ž��
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		// YawRotation�� ȸ����ķ� ��ȯ�� ��, X�� ������ ���ϰ�, Direction ������ ����
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// ��� ���������� Ž��
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		// YawRotation�� ȸ����ķ� ��ȯ�� ��, Y�� ������ ���ϰ�, Direction ������ ����
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::Jump()
{

	if (bCanFirstJump)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false; // ĳ���Ͱ� �̵� ���⿡ ���� ȸ���Ұ��ΰ�
		bCanFirstJump = false;

		ACharacter::Jump();
	}

	else if (!bCanFirstJump && bCanDoubleJump)
	{
		FVector InputDirection = GetLastMovementInputVector().GetSafeNormal2D();
		if (!InputDirection.IsNearlyZero())
		{
			// �Է� ���⿡ ���� ȸ���� ���
			FRotator NewRotation = InputDirection.Rotation();
			// ĳ������ ȸ���� �Է� �������� ����
			SetActorRotation(NewRotation);
		}

		// ���� �ӵ��� �����ϸ鼭 �Է� �������� �̵� ���� ����
		FVector NewVelocity = InputDirection * GetCharacterMovement()->Velocity.Size();
		GetCharacterMovement()->Velocity = NewVelocity;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		bCanDoubleJump = false;

		ACharacter::Jump();
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	/** �־��� Rate �������� ��ȭ��(delta)�� ��� */
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); //ȸ����/�� * ��/������
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	/** �־��� Rate �������� ��ȭ��(delta)�� ��� */
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); //ȸ����/�� * ��/������

}

void AShooterCharacter::FireWeapon()
{
	if (!bAiming)
		return;

	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* MagicSocket = GetMesh()->GetSocketByName("MagicSocket");
	if (MagicSocket)
	{
		const FTransform SocketTransform = MagicSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FHitResult BeamHitResult;
		bool bBeamEnd = GetBeamEndLocation(
			SocketTransform.GetLocation(), BeamHitResult);

		if (bBeamEnd)
		{
			// �ǰݵ� ���Ͱ� �ִ°�?
			if (BeamHitResult.GetActor())
			{
				// �ǰݵ� ������ HitInterface�� ������
				IHitInterface* HitInterface = Cast<IHitInterface>(BeamHitResult.GetActor());

				// �ǰݵ� ���Ͱ� HitInterface�� ������ �ִٸ�
				if (HitInterface)
				{
					HitInterface->Hit_Implementation(BeamHitResult);

					AEnemy* HitEnemy = Cast<AEnemy>(BeamHitResult.GetActor());
					if (HitEnemy)
					{
						UGameplayStatics::ApplyDamage(
							BeamHitResult.GetActor(),
							Damage,
							GetController(),
							this,
							UDamageType::StaticClass());
					}
				}
				// �ǰݵ� ���Ͱ� HitInterface�� ������ ���� �ʴٸ�
				else
				{
					UGameplayStatics::SpawnEmitterAtLocation(
						GetWorld(),
						ImpactParticles,
						BeamHitResult.Location);
				}
			}
		}

		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			BeamParticles,
			SocketTransform);

		if (Beam)
		{
			Beam->SetVectorParameter(FName("Target"), BeamHitResult.Location);
		}

	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

bool AShooterCharacter::GetBeamEndLocation(
	const FVector& MuzzleSocketLocation, 
	FHitResult& OutHitResult)
{
	FVector OutBeamLocation;

	// ����Ʈ�� ���� ũ�⸦ ����
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// ũ�ν������ screen space ��ġ�� ����
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// ũ�ν������ world ��ġ�� ������ ����
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	/*
	* deprojection) 2D �̹��� ���� �ȼ� ��ǥ�� 3D ������ ������ ��ȯ�ϴ� ���� �ǹ�
	* deprojection�� �������ΰ�?
	*/
	if (bScreenToWorld)
	{
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50000.f };

		// [�������� ����] �� �浹 �˻翡�� ���� �浹�� ������ ����(End) ���� ����
		OutBeamLocation = End;

		// ũ�ν���� ��ġ�� �������� ������ ������ �� 
		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);
		if (!OutHitResult.bBlockingHit) // ������ �浹���� �ʾҴ°�?
		{
			// ����(End Point)�� ������ �浹�� ��ġ
			OutHitResult.Location = OutBeamLocation;
			return false;
		}
	}

	return true;
}

void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;
	CameraBoom->SocketOffset = FVector(0.f, 100.f, 100.f); // ������ �Է��� x����, y����, z���� ���� ����

}

void AShooterCharacter::AimingButtonReleased()
{
	bAiming = false;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 50.f); // ������ �Է��� x����, y����, z���� ���� ����

}

void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{

	// ���� ī�޶� ���� ����
	if (bAiming)
	{
		// �� �� �þ߷� �ε巴�� ��ȯ
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraZoomedFOV,
			DeltaTime,
			ZoomInterpSpeed);
		GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	}
	else
	{
		// �⺻ �þ߷� �ε巴�� ��ȯ
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraDefaultFOV,
			DeltaTime,
			ZoomInterpSpeed);
		GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	}
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	StartFireTimer();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;

}

void AShooterCharacter::StartFireTimer()
{
	if (bShouldFire)
	{
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(
			AutoFireTimer,
			this,
			&AShooterCharacter::AutoFireReset,
			AutomaticFireRate);
	}
}

void AShooterCharacter::AutoFireReset()
{
	if (bDied) return; // ������ �߻� �Ұ���

	bShouldFire = true;
	if (bFireButtonPressed)
	{
		StartFireTimer();
	}
}

void AShooterCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	bCanFirstJump = true;
	bCanDoubleJump = true;
	
	GetCharacterMovement()->bOrientRotationToMovement = true; // ĳ���Ͱ� �̵� ���⿡ ���� ȸ���Ұ��ΰ�
}

EPhysicalSurface AShooterCharacter::GetSurfaceType()
{
	FHitResult HitResult;
	const FVector Start{ GetActorLocation() };
	const FVector End{ Start + FVector(0.f, 0.f, -400.f) };
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(
		HitResult, 
		Start, 
		End, 
		ECollisionChannel::ECC_Visibility, 
		QueryParams);

	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Aiming �� �� ī�޶��� ���� �ε巴�� ó��
	CameraInterpZoom(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent); // null ������ Ȯ�� �� null�̸� ����

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AShooterCharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);


	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, 
		&AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, 
		&AShooterCharacter::AimingButtonReleased);
}

float AShooterCharacter::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{

	if (Health - DamageAmount <= 0.f && !bDied)
	{
		Health = 0.f;
		Die();
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AShooterCharacter::Die()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		DisableInput(PC);
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DieMontage)
	{
		AnimInstance->Montage_Play(DieMontage);
		bDied = true;
	} 
}

void AShooterCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
}