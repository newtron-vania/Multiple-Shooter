#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

//
// 메뉴 클래스가 콜백을 바인딩할 사용자 정의 델리게이트 선언
//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResult, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessful);

/**
 * 멀티플레이어 세션 서브시스템 클래스 정의
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMultiplayerSessionsSubsystem();

	//
	// 세션 기능을 처리하는 메소드. 메뉴 클래스가 이를 호출
	//
	void CreateSession(int32 NumPublicConnections, FString MatchType); // 세션 생성
	void FindSessions(int32 MaxSearchResult); // 세션 찾기
	void JoinSession(const FOnlineSessionSearchResult& SessionResult); // 세션 참가
	void DestroySession(); // 세션 파괴
	void StartSession(); // 세션 시작

	// 메뉴 클래스가 콜백 바인딩을 위해 사용할 사용자 정의 델리게이트
	FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;
	FMultiplayerOnFindSessionsComplete MultiplayerOnFindSessionsComplete;
	FMultiplayerOnJoinSessionComplete MultiplayerOnJoinSessionComplete;
	FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;
	FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;

protected:

	//
	// 온라인 세션 인터페이스 델리게이트 목록에 추가할 내부 콜백
	// 이 클래스 외부에서는 호출할 필요 없음
	//
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful); // 세션 생성 완료시 콜백
	void OnFindSessionsComplete(bool bWasSuccessful); // 세션 찾기 완료시 콜백
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result); // 세션 참가 완료시 콜백
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful); // 세션 파괴 완료시 콜백
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful); // 세션 시작 완료시 콜백

private:
	IOnlineSessionPtr SessionInterface; // 세션 인터페이스 포인터
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings; // 마지막 세션 설정
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch; // 마지막 세션 검색

	//
	// 온라인 세션 인터페이스 델리게이트 목록에 추가할 것
	// 멀티플레이어 세션 서브시스템의 내부 콜백을 이 델리게이트에 바인딩
	//
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

	bool bCreateSessionOnDestroy{ false }; // 파괴 후 세션 생성 여부
	int32 LastNumPublicConnections; // 마지막 공개 연결 수
	FString LastMatchType; // 마지막 매치 타입
};
