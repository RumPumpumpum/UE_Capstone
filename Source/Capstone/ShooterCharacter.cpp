// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"

// Sets default values
AShooterCharacter::AShooterCharacter()	:
	BaseTurnRate(45.f),	BaseLookUpRate(45.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// CameraBoom ����(ĳ���� �ֺ��� �浹�� �ִ� ��� ī�޶� ���� ĳ���� ������ �����)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1000.f; // ī�޶�� ĳ���� �ڿ� �� �Ÿ����� ����
	CameraBoom->bUsePawnControlRotation = true; // ��Ʈ�ѷ��� ���缭 arm ȸ��

	// Follow Camera ����
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // boom���� ī�޶� ��ġ
	FollowCamera->bUsePawnControlRotation = false; // ī�޶�� arm�� ȸ���� ���󰡱⸸ �ϸ� ��

	// ��Ʈ�ѷ� ȸ���� ĳ���͸� ȸ������ ����. ��Ʈ�ѷ� ȸ���� ī�޶� ȸ��
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// ĳ���� �̵� ����
	GetCharacterMovement()->bOrientRotationToMovement = true; // ĳ���Ͱ� �Է��� �̵��������� ȸ��
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ȸ���ӵ��� ����
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
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

		// YawRotation�� ȸ����ķ� ��ȯ�� ��, X�� ������ ���ϰ�, Direction ������ ����
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
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
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
	const USkeletalMeshSocket* RightEffectSocket = GetMesh()->GetSocketByName("RightEffectSocket");
	if (RightEffectSocket)
	{
		const FTransform SocketTransform = RightEffectSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}
		FHitResult FireHit;
		const FVector Start{ SocketTransform.GetLocation() };
		const FQuat Rotation{ SocketTransform.GetRotation() };
		const FVector RotationAxis{ Rotation.GetAxisX() };
		const FVector End{ Start + RotationAxis * 50000.f };

		GetWorld()->LineTraceSingleByChannel(FireHit, Start, End, ECollisionChannel::ECC_Visibility);
		if (FireHit.bBlockingHit)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f);
			DrawDebugPoint(GetWorld(), FireHit.Location, 5.f, FColor::Red, false, 2.f);
		}
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);
}

