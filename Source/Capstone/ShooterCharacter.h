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

	// 받는 데미지 처리
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** 앞, 뒤 입력시 호출 */
	void MoveForward(float Value);

	/** 좌, 우 입력시 호출 */
	void MoveRight(float Value);

	/** 점프키 누를시 호출 */
	void Jump();

	/**
	* 받은 입력을 통해 주어진 비율(Rate) 만큼 회전한다.
	* @param Rate : 정규화된 비율, 1.0은 원하는 회전 속도의 100%를 의미
	*/
	void TurnAtRate(float Rate);

	/**
	* 받은 입력을 통해 주어진 비율(Rate) 만큼 look up/down 한다.
	* @param Rate : 정규화된 비율, 1.0은 원하는 회전 속도의 100%를 의미
	*/
	void LookUpAtRate(float Rate);

	/** Fire Button이 눌러지면 호출 */
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);

	/** 버튼 입력으로 bAming 값을 ture or false로 설정 */
	void AimingButtonPressed();
	void AimingButtonReleased();

	/** 조준시 카메라 전환 부드럽게 */
	void CameraInterpZoom(float DeltaTime);

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	/** 캐릭터가 땅에 닿아 있는가 */
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
	* Camera boom(카메라 팔)은 캐릭터 뒤에 카메라를 위치 시킨다.
	* 생성자를 호출하지 않기위해 포인터변수로 선언
	* 포인터 변수는 객체를 복사하지 않고 객체에 대한 참조를 유지할 수 있으므로 메모리 사용량도 감소
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* CameraBoom;

	// 카메라가 캐릭터를 따라간다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* FollowCamera;

	// 기본 회전 속도. 초당 회전각도 단위등 다른 스케일링이 최종 회전 속도에 영향을 줄 수 있음
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float BaseTurnRate;

	// 기본 look up/down 속도. 초당 회전각도 단위등 다른 스케일링이 최종 회전 속도에 영향을 줄 수 있음
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float BaseLookUpRate;

	// 랜덤 총소리 sound cue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class USoundCue* FireSound;

	// RightEffectSocket 에서 무기 이팩트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* MuzzleFlash;

	// 발사를 위한 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UAnimMontage* HipFireMontage;

	// 총알에 맞은 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	UParticleSystem* ImpactParticles;

	// 총알에 대한 흔적(선)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	UParticleSystem* BeamParticles;

	// 데미지 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float Damage;

	/** Aiming이 True 일 때 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category = Combat, meta = (AllowPrivateAccess = "true"));
	bool bAiming;

	/** 기본 카메라 화면 뷰 값 */ 
	float CameraDefaultFOV;

	/** 줌인 카메라 화면 뷰 값 */
	float CameraZoomedFOV;

	/** 이 프레임의 현재 시야 */
	float CameraCurrentFOV;

	/** 조준할 때 확대 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float ZoomInterpSpeed;

	/** 좌클릭 혹은 오른쪽 콘솔 트리거 누름 */
	bool bFireButtonPressed;

	/** 총알을 발사 할 수 있을때 True, 타이머를 기다릴때 false */
	bool bShouldFire;

	/** 자동사격 간격 */
	float AutomaticFireRate;

	/** 발사 사이의 타이머 간격 설정 */
	FTimerHandle AutoFireTimer;

	/** 최초 점프 가능 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player, meta = (AllowPrivateAccess = "true"));
	bool bCanFirstJump;

	/** 더블 점프 가능 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player, meta = (AllowPrivateAccess = "true"));
	bool bCanDoubleJump;

	/** 현재 체력 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "ture"))
	float Health;

	/** 최대 체력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "ture"))
	float MaxHealth;

	/** 캐릭터 사망 몽타쥬 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player, meta = (AllowPrivateAccess = "true"));
	UAnimMontage* DieMontage;

public:
	/**
	* FORCEINLINE은 함수를 인라인 함수로 선언하는 매크로
	* 주로 작은 함수나 빈번히 호출되는 함수를 인라인 함수로 선언하여 프로그램의 성능을 향상
	*/  

	// CameraBoom subobject를 반환한다.
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// FollowCamera subobject를 반환한다.
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Damage를 반환한다.
	FORCEINLINE float GetDamage() const { return Damage; }
};
