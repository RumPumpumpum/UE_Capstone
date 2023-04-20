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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** 앞, 뒤 입력시 호출 */
	void MoveForward(float Value);

	/** 좌, 우 입력시 호출 */
	void MoveRight(float Value);

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

	//기본 회전 속도. 초당 회전각도 단위등 다른 스케일링이 최종 회전 속도에 영향을 줄 수 있음
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float BaseTurnRate;

	//기본 look up/down 속도. 초당 회전각도 단위등 다른 스케일링이 최종 회전 속도에 영향을 줄 수 있음
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class USoundCue* FireSound;
public:
	/**
	* FORCEINLINE은 함수를 인라인 함수로 선언하는 매크로
	* 주로 작은 함수나 빈번히 호출되는 함수를 인라인 함수로 선언하여 프로그램의 성능을 향상
	*/

	// CameraBoom subobject를 반환한다.
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// FollowCamera subobject를 반환한다.
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
