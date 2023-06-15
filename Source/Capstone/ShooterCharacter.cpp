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
	CameraDefaultFOV(0.f), // BeginPlay에서 설정
	CameraZoomedFOV(70.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),
	// 자동 발사 변수들
	AutomaticFireRate(0.2f),
	bShouldFire(true),
	bFireButtonPressed(false),
	Health(100.f),
	MaxHealth(100.f),
	bDied(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// CameraBoom 생성(캐릭터 주변에 충돌이 있는 경우 카메라 붐이 캐릭터 쪽으로 당겨짐)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1000.f; // 카메라는 캐릭터 뒤에 이 거리에서 따라감
	CameraBoom->bUsePawnControlRotation = true; // 컨트롤러에 맞춰서 arm 회전
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 50.f); // 소켓을 입력한 x방향, y방향, z방향 으로 보냄

	// Follow Camera 생성
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // boom끝에 카메라 설치
	FollowCamera->bUsePawnControlRotation = true; // 카메라는 arm의 회전을 따라가기만 하면 됨

	// 컨트롤러 회전에 캐릭터를 회전하지 않음. 컨트롤러 회전은 카메라만 회전
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 캐릭터 이동 구성
	GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터가 이동 방향에 맞춰 회전할것인가
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f); // 회전속도를 결정
	GetCharacterMovement()->JumpZVelocity = 300.f;
	GetCharacterMovement()->AirControl = 1.0f;
	AShooterCharacter::JumpMaxHoldTime = 0.15f; // 점프 최대 유지시간
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
		// 어디가 앞인지 탐색
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		// YawRotation을 회전행렬로 변환한 후, X축 방향을 구하고, Direction 변수에 넣음
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// 어디가 오른쪽인지 탐색
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		// YawRotation을 회전행렬로 변환한 후, Y축 방향을 구하고, Direction 변수에 넣음
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::Jump()
{

	if (bCanFirstJump)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false; // 캐릭터가 이동 방향에 맞춰 회전할것인가
		bCanFirstJump = false;

		ACharacter::Jump();
	}

	else if (!bCanFirstJump && bCanDoubleJump)
	{
		FVector InputDirection = GetLastMovementInputVector().GetSafeNormal2D();
		if (!InputDirection.IsNearlyZero())
		{
			// 입력 방향에 따라 회전값 계산
			FRotator NewRotation = InputDirection.Rotation();
			// 캐릭터의 회전을 입력 방향으로 설정
			SetActorRotation(NewRotation);
		}

		// 현재 속도를 유지하면서 입력 방향으로 이동 방향 변경
		FVector NewVelocity = InputDirection * GetCharacterMovement()->Velocity.Size();
		GetCharacterMovement()->Velocity = NewVelocity;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		bCanDoubleJump = false;

		ACharacter::Jump();
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	/** 주어진 Rate 정보에서 변화량(delta)를 계산 */
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); //회전각/초 * 초/프레임
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	/** 주어진 Rate 정보에서 변화량(delta)를 계산 */
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); //회전각/초 * 초/프레임

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
			// 피격된 액터가 있는가?
			if (BeamHitResult.GetActor())
			{
				// 피격된 액터의 HitInterface를 가져옴
				IHitInterface* HitInterface = Cast<IHitInterface>(BeamHitResult.GetActor());

				// 피격된 액터가 HitInterface를 가지고 있다면
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
				// 피격된 액터가 HitInterface를 가지고 있지 않다면
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

	// 뷰포트의 현재 크기를 얻음
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// 크로스헤어의 screen space 위치를 얻음
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// 크로스헤어의 world 위치와 방향을 얻음
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	/*
	* deprojection) 2D 이미지 상의 픽셀 좌표를 3D 공간의 점으로 변환하는 것을 의미
	* deprojection이 성공적인가?
	*/
	if (bScreenToWorld)
	{
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50000.f };

		// [라인추적 시작] 빔 충돌 검사에서 빔이 충돌한 지점을 끝점(End) 으로 설정
		OutBeamLocation = End;

		// 크로스헤어 위치를 기준으로 밖으로 광선을 쏨 
		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);
		if (!OutHitResult.bBlockingHit) // 광선이 충돌하지 않았는가?
		{
			// 끝점(End Point)는 광선이 충돌한 위치
			OutHitResult.Location = OutBeamLocation;
			return false;
		}
	}

	return true;
}

void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;
	CameraBoom->SocketOffset = FVector(0.f, 100.f, 100.f); // 소켓을 입력한 x방향, y방향, z방향 으로 보냄

}

void AShooterCharacter::AimingButtonReleased()
{
	bAiming = false;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 50.f); // 소켓을 입력한 x방향, y방향, z방향 으로 보냄

}

void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{

	// 현재 카메라 시점 설정
	if (bAiming)
	{
		// 줌 한 시야로 부드럽게 전환
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraZoomedFOV,
			DeltaTime,
			ZoomInterpSpeed);
		GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	}
	else
	{
		// 기본 시야로 부드럽게 전환
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
	if (bDied) return; // 죽으면 발사 불가능

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
	
	GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터가 이동 방향에 맞춰 회전할것인가
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

	// Aiming 할 때 카메라의 줌을 부드럽게 처리
	CameraInterpZoom(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent); // null 값인지 확인 후 null이면 종료

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