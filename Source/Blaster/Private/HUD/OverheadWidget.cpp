#include "HUD/OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

// 텍스트 표시 설정
void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay)); // DisplayText에 텍스트 설정
	}
}

// 플레이어의 네트워크 역할 표시
void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole(); // 네트워크 역할 가져오기
	FString Role;
	
	// 역할에 따른 문자열 설정
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority"); // 권한 있음
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy"); // 자율 프록시
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy"); // 시뮬레이티드 프록시
		break;
	case ENetRole::ROLE_None:
		Role = FString("None"); // 역할 없음
		break;
	}
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
	SetDisplayText(RemoteRoleString); // 설정된 역할 문자열 표시
}

// 플레이어 이름 표시
void UOverheadWidget::ShowPlayerName(APawn* InPawn)
{
	FString PlayerName = InPawn->GetPlayerState()->GetPlayerName(); // 플레이어 이름 가져오기
	SetDisplayText(PlayerName); // 이름 표시
}


// 위젯 파괴 시 호출
void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent(); // 부모에서 제거
	Super::NativeDestruct();
}
