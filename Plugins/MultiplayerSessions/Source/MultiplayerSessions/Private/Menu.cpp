#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

// 메뉴 설정 함수
void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	NumPublicConnections = NumberOfPublicConnections; // 공개 연결 수
	MatchType = TypeOfMatch; // 매치 타입
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath); // 로비 경로
	AddToViewport(); // 뷰포트에 추가
	SetVisibility(ESlateVisibility::Visible); // 가시성 설정
	SetIsFocusable(true); // 포커스 가능 설정

	UWorld* World = GetWorld(); // 월드 가져오기
	if(World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController(); // 플레이어 컨트롤러 가져오기
		if(PlayerController)
		{
			FInputModeUIOnly InputModeData; // UI 전용 입력 모드
			InputModeData.SetWidgetToFocus(TakeWidget()); // 위젯 포커스 설정
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 마우스 락 모드 설정
			PlayerController->SetInputMode(InputModeData); // 입력 모드 설정
			PlayerController->SetShowMouseCursor(true); // 마우스 커서 표시
		}
	}
	
	UGameInstance* GameInstance = GetGameInstance(); // 게임 인스턴스 가져오기
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>(); // 멀티플레이어 세션 서브시스템 가져오기
	}

	if(MultiplayerSessionsSubsystem)
	{
		// 세션 이벤트에 대한 콜백 함수 바인딩
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		// 비동적인 바인딩
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

// 초기화 함수
bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false; // 기본 초기화 실패 시 false 반환
	}

	// 호스트 버튼에 대한 이벤트 바인딩
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	// 참가 버튼에 대한 이벤트 바인딩
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

// 메뉴 파괴 시 호출되는 함수
void UMenu::NativeDestruct()
{
	MenuTearDown(); // 메뉴 해체
	
	Super::NativeDestruct(); // 부모 클래스의 파괴 함수 호출
}

// 세션 생성 결과 처리 함수
void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString(TEXT("Session created successfully!")));
		}
		
		UWorld* World = GetWorld();
		if(World)
		{
			World->ServerTravel(PathToLobby); // 서버 이동
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Failed to create session!")));
		}
		HostButton->SetIsEnabled(true);
	}
}

// 세션 찾기 결과 처리 함수
void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if(MultiplayerSessionsSubsystem == nullptr)
	{
		return;
	}
	
	for(auto Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if(SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result); // 세션 참가 요청
			return;
		}
	}
	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

// 세션 참가 결과 처리 함수
void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if(PlayerController)
			{
				PlayerController->ClientTravel(Address, TRAVEL_Absolute); // 클라이언트 이동
			}
		}
	}
	if(Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

// 세션 파괴 결과 처리 함수
void UMenu::OnDestroySession(bool bWasSuccessful)
{
	// 세션 파괴 후 처리 로직 (현재 비어있음)
}

// 세션 시작 결과 처리 함수
void UMenu::OnStartSession(bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Success to start session!")));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Failed to start session!")));
		}
	}
}

// 호스트 버튼 클릭 이벤트 처리 함수
void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if(MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType); // 세션 생성 요청
	}
}

// 참가 버튼 클릭 이벤트 처리 함수
void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if(MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000); // 세션 찾기 요청
	}
}

// 메뉴 해체 함수
void UMenu::MenuTearDown()
{
	RemoveFromParent(); // 부모에서 제거
	UWorld* World = GetWorld();
	if(World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if(PlayerController)
		{
			FInputModeGameOnly InputModeData; // 게임 전용 입력 모드
			PlayerController->SetInputMode(InputModeData); // 입력 모드 설정
			PlayerController->SetShowMouseCursor(false); // 마우스 커서 숨김
		}
	}
}
