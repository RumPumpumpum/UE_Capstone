// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AEnemy::AEnemy() :
	Health(100.f),
	MaxHealth(100.f),
	Attack_1(TEXT("Attack_1")),
	Attack_2(TEXT("Attack_2")),
	bCanAttack(true),
	AttackWaitTime(2.f),
	bDie(false),
	BaseDamage(20.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 탐지 범위 생성
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());

	// 공격 범위 생성
	AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphere"));
	AttackRangeSphere->SetupAttachment(GetRootComponent());

	// 무기 충돌 박스 생성
	WeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponCollision->SetupAttachment(GetMesh(), FName("WeaponBone"));

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);

	AttackRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackRangeOverlap);

	AttackRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AttackRangeEndOverlap);

	// Weapon box가 overlap 되었을 때 함수
	WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::WeaponOverlap);

	// Weapon box 충돌 설정, 공격 전까지 무기와 충돌하면 안됨
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	WeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponCollision->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Pawn, 
		ECollisionResponse::ECR_Overlap);

	GetMesh()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Visibility, 
		ECollisionResponse::ECR_Block);

	/** 카메라가 mesh와 capsule은 무시 */
	GetMesh()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Camera, 
		ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Camera,
		ECollisionResponse::ECR_Ignore);


	EnemyController = Cast<AEnemyController>(GetController());

	// 공격 가능으로 초기화
	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsBool(
			FName("CanAttack"),
			true);
	}

	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(
		GetActorTransform(),
		PatrolPoint);


	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(
		GetActorTransform(),
		PatrolPoint2);



	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsVector(
			TEXT("PatrolPoint"),
			WorldPatrolPoint);

		EnemyController->GetBlackBoardComponent()->SetValueAsVector(
			TEXT("PatrolPoint2"),
			WorldPatrolPoint2);

		EnemyController->RunBehaviorTree(BehaviorTree);
	}
}

void AEnemy::PlayHitMontage(FName Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(HitMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(Section, HitMontage);
	}
}

void AEnemy::Die()
{
	// 이미 죽었으면 반환
	if (bDie) return;
	bDie = true;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DieMontage)
	{
		AnimInstance->Montage_Play(DieMontage, 0.5f);
	}

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(
			FName("Dead"),
			true);
		EnemyController->StopMovement();
	}
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	if (EnemyController == nullptr) return;

	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		// Blackboard의 Target 키 설정
		EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
	}
}

void AEnemy::AttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	if (EnemyController == nullptr) return;

	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		bInAttackRange = true;
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(
				TEXT("InAttackRange"),
				true);
		}
	}
}

void AEnemy::AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;
	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		bInAttackRange = false;
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(
				TEXT("InAttackRange"),
				false);
		}
	}
}

void AEnemy::PlayAttackMontage(FName Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		AnimInstance->Montage_JumpToSection(Section, AttackMontage);
	}

	bCanAttack = false;
	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsBool(
			FName("CanAttack"),
			false);
	}

	GetWorldTimerManager().SetTimer(
		AttackWaitTimer,
		this,
		&AEnemy::ResetCanAttack,
		AttackWaitTime);


}

FName AEnemy::GetAttackSectionName()
{
	FName SectionName;
	const int32 Section{ FMath::RandRange(1, 2) }; // 랜덤
	switch(Section)
	{
	case 1:
		SectionName = Attack_1;
		break;
	case 2:
		SectionName = Attack_2;
		break;
	}

	return SectionName;
}

void AEnemy::ResetCanAttack()
{
	bCanAttack = true;
	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsBool(
			FName("CanAttack"),
			true);
	}
}

void AEnemy::DestroyEnemy()
{
	Destroy();
}

void AEnemy::DoDamage(AActor* Victim)
{
	if (Victim == nullptr) return;
	AShooterCharacter* Character = Cast<AShooterCharacter>(Victim);
	if (Character)
	{
		UGameplayStatics::ApplyDamage(
			Character,
			BaseDamage,
			EnemyController,
			this,
			UDamageType::StaticClass());
	}
}

void AEnemy::WeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DoDamage(OtherActor);
}

void AEnemy::ActivateWeapon()
{
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateWeapon()
{
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}



// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::Hit_Implementation(FHitResult HitResult)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}

	// 죽었으면 피격 모션 무시
	if (bDie) return;

	PlayHitMontage(FName("HitReact"));

}

float AEnemy::TakeDamage(
	float DamageAmount, 
	FDamageEvent const& DamageEvent, 
	AController* EventInstigator,
	AActor* DamageCauser)
{
	// Blackboard의 Target 키를 데미지를 입힌 Chracter로 설정
	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsObject(
			FName("Target"),
			DamageCauser);
	}

	if (Health - DamageAmount <= 0.f)
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
