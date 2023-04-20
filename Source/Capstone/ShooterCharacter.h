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

	/** ��, �� �Է½� ȣ�� */
	void MoveForward(float Value);

	/** ��, �� �Է½� ȣ�� */
	void MoveRight(float Value);

	/**
	* ���� �Է��� ���� �־��� ����(Rate) ��ŭ ȸ���Ѵ�.
	* @param Rate : ����ȭ�� ����, 1.0�� ���ϴ� ȸ�� �ӵ��� 100%�� �ǹ�
	*/
	void TurnAtRate(float Rate);

	/**
	* ���� �Է��� ���� �־��� ����(Rate) ��ŭ look up/down �Ѵ�.
	* @param Rate : ����ȭ�� ����, 1.0�� ���ϴ� ȸ�� �ӵ��� 100%�� �ǹ�
	*/
	void LookUpAtRate(float Rate);

	/** Fire Button�� �������� ȣ�� */
	void FireWeapon();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/**
	* Camera boom(ī�޶� ��)�� ĳ���� �ڿ� ī�޶� ��ġ ��Ų��.
	* �����ڸ� ȣ������ �ʱ����� �����ͺ����� ����
	* ������ ������ ��ü�� �������� �ʰ� ��ü�� ���� ������ ������ �� �����Ƿ� �޸� ��뷮�� ����
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* CameraBoom;

	// ī�޶� ĳ���͸� ���󰣴�.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* FollowCamera;

	//�⺻ ȸ�� �ӵ�. �ʴ� ȸ������ ������ �ٸ� �����ϸ��� ���� ȸ�� �ӵ��� ������ �� �� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float BaseTurnRate;

	//�⺻ look up/down �ӵ�. �ʴ� ȸ������ ������ �ٸ� �����ϸ��� ���� ȸ�� �ӵ��� ������ �� �� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class USoundCue* FireSound;
public:
	/**
	* FORCEINLINE�� �Լ��� �ζ��� �Լ��� �����ϴ� ��ũ��
	* �ַ� ���� �Լ��� ����� ȣ��Ǵ� �Լ��� �ζ��� �Լ��� �����Ͽ� ���α׷��� ������ ���
	*/

	// CameraBoom subobject�� ��ȯ�Ѵ�.
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// FollowCamera subobject�� ��ȯ�Ѵ�.
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
