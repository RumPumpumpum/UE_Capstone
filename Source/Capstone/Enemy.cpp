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

// Sets default values
AEnemy::AEnemy() : Health(100.f), MaxHealth(100.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 탐지 영역 생성
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	
	EnemyController = Cast<AEnemyController>(GetController());

	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(
		GetActorTransform(),
		PatrolPoint);

	DrawDebugSphere(
		GetWorld(),
		WorldPatrolPoint,
		25.f,
		12,
		FColor::Red,
		true
	);

	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(
		GetActorTransform(),
		PatrolPoint2);

	DrawDebugSphere(
		GetWorld(),
		WorldPatrolPoint2,
		25.f,
		12,
		FColor::Red,
		true
	);

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

void AEnemy::PlayDieMontage(FName Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(DieMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(Section, DieMontage);
	}
}

void AEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DieMontage)
	{
		AnimInstance->Montage_Play(DieMontage);
	}
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	auto Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		// Blackboard의 Target 키 설정
		EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
	}
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

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DieMontage)
	{
		AnimInstance->Montage_Play(HitMontage);
	}

}

float AEnemy::TakeDamage(
	float DamageAmount, 
	FDamageEvent const& DamageEvent, 
	AController* EventInstigator,
	AActor* DamageCauser)
{
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

