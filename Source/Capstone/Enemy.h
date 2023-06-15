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
	
	void Die();

	/** Ž�� ������ ���� ������ �Ǹ� ȣ��*/
	UFUNCTION()
	void AgroSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void AttackRangeOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void AttackRangeEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(FName Section, float PlayRate);

	UFUNCTION(BlueprintPure)
	FName GetAttackSectionName();

	void ResetCanAttack();

	UFUNCTION(BlueprintCallable)
	void DestroyEnemy();

	UFUNCTION()
	void WeaponOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult );

	/** Weapon Box Ȱ��/��Ȱ��ȭ */
	UFUNCTION(BlueprintCallable)
	void ActivateWeapon();
	UFUNCTION(BlueprintCallable)
	void DeactivateWeapon();

	void DoDamage(AActor* Victim);

	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);
private:
	/** �Ѿ��� �¾��� �� ��ƼŬ ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "ture"))
	class UParticleSystem* ImpactParticles;

	/** �Ѿ��� �¾��� �� �Ҹ� ��� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "ture"))
	class USoundCue* ImpactSound;

	/** ���� ü�� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "ture"))
	float Health;

	/** �ִ� ü�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "ture"))
	float MaxHealth;

	/** Hit ��Ÿ�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "ture"))
	UAnimMontage* HitMontage;

	/** Attack ��Ÿ�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "ture"))
	UAnimMontage* AttackMontage;

	/** Die ��Ÿ�� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "ture"))
	UAnimMontage* DieMontage;

	/** Behavior Tree */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "ture"))
	class UBehaviorTree* BehaviorTree;
	
	/** enemy�� ������ ���� */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "ture", MakeEditWidget = "true"))
	FVector PatrolPoint;

	/** enemy�� ������ ����2 */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "ture", MakeEditWidget = "true"))
	FVector PatrolPoint2;

	class AEnemyController* EnemyController;

	/** ���� Ž���� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AgroSphere;

	/** ���� ������ ������ true */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bInAttackRange;

	/** ���� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USphereComponent* AttackRangeSphere;
	
	/** ������ �� ������ ture */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bCanAttack;

	FTimerHandle AttackWaitTimer;

	/** ���� ������ �ּ� ���ð� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float AttackWaitTime;

	/** Attack ��Ÿ���� ���� �̸��� */
	FName Attack_1;
	FName Attack_2;

	/** ���� �浹 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* WeaponCollision;

	bool bDie;

	/** ������ �޴� �⺻ ������ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player, meta = (AllowPrivateAccess = "true"));
	float BaseDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	bool bStunned;
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
