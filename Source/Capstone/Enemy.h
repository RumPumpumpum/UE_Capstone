// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HitInterface.h"
#include "Enemy.generated.h"

UCLASS()
class CAPSTONE_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void PlayHitMontage(FName Section, float PlayRate = 1.0f);

	void PlayDieMontage(FName Section, float PlayRate = 1.0f);
	
	void Die();

	/** 탐지 범위에 무언가 오버랩 되면 호출*/
	UFUNCTION()
	void AgroSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);


private:
	/** 총알이 맞았을 때 파티클 생성 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "ture"))
	class UParticleSystem* ImpactParticles;

	/** 총알이 맞았을 때 소리 재생 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "ture"))
	class USoundCue* ImpactSound;

	/** 현재 체력 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "ture"))
	float Health;

	/** 최대 체력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "ture"))
	float MaxHealth;

	/** Hit 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "ture"))
	UAnimMontage* HitMontage;

	/** Die 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "ture"))
	UAnimMontage* DieMontage;

	/** Behavior Tree */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "ture"))
	class UBehaviorTree* BehaviorTree;
	
	/** enemy를 움직일 지점 */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "ture", MakeEditWidget = "true"))
	FVector PatrolPoint;

	/** enemy를 움직일 지점2 */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "ture", MakeEditWidget = "true"))
	FVector PatrolPoint2;

	class AEnemyController* EnemyController;

	/** 적이 탐지할 범위 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AgroSphere;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void Hit_Implementation(FHitResult HitResult) override;

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
};
