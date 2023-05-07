// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"

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
	bFireButtonPressed(false)
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
	GetCharacterMovement()->RotationRate = FRotator(0.f, 700.f, 0.f); // ȸ���ӵ��� ����
	GetCharacterMovement()->JumpZVelocity = 300.f;
	GetCharacterMovement()->AirControl = 2.0f;
	AShooterCharacter::JumpMaxHoldTime = 0.3f; // ���� �ִ� �����ð�
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
		/*
		FRotator NewRotation = FRotator(0, 45, 0); // ������ ���ο� ȸ�� ��
		SetActorRotation(NewRotation); // ������ ȸ�� ���� ���ο� ������ ����
		*/


		GetCharacterMovement()->bOrientRotationToMovement = false; // ĳ���Ͱ� �̵� ���⿡ ���� ȸ���Ұ��ΰ�
		bCanFirstJump = false;

		ACharacter::Jump();
	}

	else if(!bCanFirstJump && bCanDoubleJump)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false; // ĳ���Ͱ� �̵� ���⿡ ���� ȸ���Ұ��ΰ�
		bCanDoubleJump = false;

		ACharacter::Jump();
		//LaunchCharacter(FVector(GetVelocity().X, GetVelocity().Y, GetCharacterMovement()->JumpZVelocity), true, true);
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

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(
			SocketTransform.GetLocation(), BeamEnd);

		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamEnd);
			}
		}

		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			BeamParticles,
			SocketTransform);

		if (Beam)
		{
			Beam->SetVectorParameter(FName("Target"), BeamEnd);
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
	FVector& OutBeamLocation)
{
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
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50000.f };

		// [�������� ����] �� �浹 �˻翡�� ���� �浹�� ������ ����(End) ���� ����
		OutBeamLocation = End;

		// ũ�ν���� ��ġ�� �������� ������ ������ �� 
		GetWorld()->LineTraceSingleByChannel(
			ScreenTraceHit,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);
		if (ScreenTraceHit.bBlockingHit) // ������ �浹�ߴ°�?
		{
			// ����(End Point)�� ������ �浹�� ��ġ
			OutBeamLocation = ScreenTraceHit.Location;
		}

		// �̹��� �ѽſ��� ���� ���������� �õ��Ѵ�.
		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart{ MuzzleSocketLocation };
		const FVector WeaponTraceEnd{ OutBeamLocation };
		GetWorld()->LineTraceSingleByChannel(
			WeaponTraceHit,
			WeaponTraceStart,
			WeaponTraceEnd,
			ECollisionChannel::ECC_Visibility);
		if (WeaponTraceHit.bBlockingHit) // �ѽŰ� End point ���̿� ��ü�� �ִ°�?
		{
			OutBeamLocation = WeaponTraceHit.Location;
		}

		// BeamEndPoint�� ������Ʈ �� �� Ÿ��ȿ�� �߻�
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactParticles,
				OutBeamLocation);
		}

		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				BeamParticles,
				MuzzleSocketLocation);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), OutBeamLocation);
			}
		}
	}


	return false;
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

