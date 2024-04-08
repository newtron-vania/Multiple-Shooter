#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

// 플레이어가 로그인했을 때 호출
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer); // 기본 로그인 처리 실행

	// 현재 게임 상태에서 플레이어 수 계산
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	// 플레이어 수가 2명이면 맵 전환 실행
	if (NumberOfPlayers == 2)
	{
		UWorld* World = GetWorld(); // 현재 월드 가져오기
		if (World)
		{
			bUseSeamlessTravel = true; // 매끄러운 이동 사용 설정
			World->ServerTravel(FString("/Game/Maps/BlasterMap?listen")); // 지정된 맵으로 이동
		}
	}
}