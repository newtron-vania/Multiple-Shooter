#include "Weapon/Weapon.h"
#include "Animation/AnimationAsset.h"
#include "Characters/BlasterCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Casing.h"

// 기본 값 설정
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false; // 매 프레임마다 Tick 호출 안 함
	bReplicates = true; // 네트워크 복제 활성화

	// 무기 메쉬 컴포넌트 생성 및 설정
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	// 충돌 설정
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block); // 모든 채널에 대해 블록 처리
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); // 플레이어 캐릭터에 대해 무시
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌 비활성화

	// 영역 구 컴포넌트 생성 및 설정
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // 모든 채널 무시
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌 비활성화

	// 위젯 컴포넌트 생성 및 설정
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
}

// 게임 시작 시 호출
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// 서버에 권한이 있을 경우 충돌 설정
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 쿼리 및 물리 충돌 활성화
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // 플레이어와 겹칠 때 이벤트 발생
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap); // 겹침 시작 이벤트 바인딩
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap); // 겹침 종료 이벤트 바인딩
	}

	ShowPickupWidget(false); // 처음에는 픽업 위젯 숨김
}

// 무기 영역과 다른 액터가 겹쳤을 때 호출
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(this); // 겹치는 무기 설정
	}
}

// 무기 영역과 다른 액터의 겹침이 끝났을 때 호출
void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr); // 겹치는 무기 해제
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Tick 함수 내용 (현재 비어 있음)
}

// 네트워크 복제 속성 설정
void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState); // WeaponState 변수의 네트워크 복제 설정
}

// 무기 상태가 변경되었을 때 호출
void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false); // 무기가 장착된 경우 픽업 위젯 숨김
		break;
	}
}

// 무기 상태 설정
void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State; // 무기 상태 업데이트
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false); // 무기가 장착된 경우 픽업 위젯 숨김
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌 비활성화
		break;
	}
}

// 픽업 위젯의 표시 여부 설정
void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget); // 위젯 표시 상태 변경
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if (CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);

			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ACasing>(
					CasingClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator()
				);
			}
		}
	}
}

