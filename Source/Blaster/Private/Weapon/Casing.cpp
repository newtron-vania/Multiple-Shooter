// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Casing.h"

#include "Kismet/GameplayStatics.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = true;
	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);

	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); // 카메라 채널에 대한 충돌 응답 무시
	CasingMesh->SetSimulatePhysics(true); // 물리 시뮬레이션 설정
	CasingMesh->SetEnableGravity(true); // 중력 설정
	CasingMesh->SetNotifyRigidBodyCollision(true); // 강체 충돌 알림 설정
	ShellEjectionImpulse = 10.f; // 탄피 배출 힘 설정

	bIsTimerSet = false; // 타이머 설정 여부 초기화
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();

	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
}

// 탄피가 다른 오브젝트와 충돌 시 호출되는 함수
void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ShellSound,
			GetActorLocation());
	}

	// 3초 뒤에 탄피를 파괴하도록 타이머 설정
	// 타이머가 이미 설정되지 않은 경우에만 타이머 설정
	if (!bIsTimerSet)
	{
		bIsTimerSet = true; // 타이머 설정 여부 갱신
		FTimerHandle DestroyHandle;
		GetWorld()->GetTimerManager().SetTimer(DestroyHandle, this, &ACasing::DestroyCasing, 3.0f, false);
	}
}

void ACasing::DestroyCasing()
{
	Destroy();
}