#include "avalon/GameCenter.h"
#include "steam/steam_api.h"
#include <algorithm>
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>

namespace avalon {

static void OutputDebugString(const char * message)
{
	printf("%s", message);
}

class FindLeaderBoardCall
{
public:
	FindLeaderBoardCall(SteamAPICall_t hSteamAPICall, int score):_score(score)
	{
		_callBack.Set(hSteamAPICall, this, &FindLeaderBoardCall::_onFindLeaderboard);
	}

private:
	int _score = 0;
	CCallResult<FindLeaderBoardCall, LeaderboardFindResult_t> _callBack;

	void _onFindLeaderboard(LeaderboardFindResult_t *pResult, bool bIOFailure)
	{
		if (!pResult->m_bLeaderboardFound || bIOFailure)
		{
			OutputDebugString("Leaderboard could not be found\n");
		}
		else
		{
			SteamUserStats()->UploadLeaderboardScore(pResult->m_hSteamLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, _score, NULL, 0);
		}

		delete this;
	}
};

class GameCenterSteamImpl
{
public:
	static GameCenterSteamImpl& getInstance()
	{
		static GameCenterSteamImpl instance;
		return instance;
	}

	void login()
	{
		// Is Steam loaded? If not we can't get stats.
		if (NULL == SteamUserStats() || NULL == SteamUser())
		{
			return;
		}
		// Is the user logged on?  If not we can't get stats.
		if (!SteamUser()->BLoggedOn())
		{
			return;
		}
		// Request user stats.
		SteamUserStats()->RequestCurrentStats();
	}

	void logout()
	{

	}

	bool isLoggedIn()
	{
		return _initialized;
	}

	bool showGameCenter()
	{
		return showAchievements();
	}

	bool showAchievements()
	{
		wchar_t buffer[256];
		swprintf(buffer, L"https://steamcommunity.com/stats/%d/achievements/", _appID);
		return (int)ShellExecute(0, 0, buffer, 0, 0, SW_SHOW) > 32;
	}

	void postAchievement(const std::string &idName, int percentComplete, bool showBanner)
	{
		if (_initialized && percentComplete == 100)
		{
			SteamUserStats()->SetAchievement(idName.c_str());
			SteamUserStats()->StoreStats();
		}
	}

	void clearAllAchievements()
	{
		if (_initialized)
		{
			uint32 achievementsCount = SteamUserStats()->GetNumAchievements();
			for (uint32 i = 0; i < achievementsCount; ++i)
			{
				const char *achievementName = SteamUserStats()->GetAchievementName(i);
				SteamUserStats()->ClearAchievement(achievementName);
			}
		}
	}

	std::string getPlayerId()
	{
		if (_initialized)
			return std::to_string(SteamUser()->GetSteamID().GetAccountID());
		else
			return "";
	}

	bool showScores()
	{
		wchar_t buffer[256];
		swprintf(buffer, L"https://steamcommunity.com/stats/%d/leaderboards/", _appID);
		return (int)ShellExecute(0, 0, buffer, 0, 0, SW_SHOW) > 32;
	}

	void postScore(const std::string &idName, int score)
	{
		SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard(idName.c_str());
		if(hSteamAPICall)
			new FindLeaderBoardCall(hSteamAPICall, score);
	}

	void clearAllScores()
	{
		if (_initialized)
			SteamUserStats()->ResetAllStats(false);
	}

private:

	int64 _appID = 0; // Our current AppID
	bool _initialized = false; // Have we called Request stats and received the callback?

	STEAM_CALLBACK(GameCenterSteamImpl, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	STEAM_CALLBACK(GameCenterSteamImpl, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	STEAM_CALLBACK(GameCenterSteamImpl, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);

	GameCenterSteamImpl()
		:m_CallbackUserStatsReceived(this, &GameCenterSteamImpl::OnUserStatsReceived)
		,m_CallbackUserStatsStored(this, &GameCenterSteamImpl::OnUserStatsStored)
		,m_CallbackAchievementStored(this, &GameCenterSteamImpl::OnAchievementStored)
	{
		_appID = SteamUtils()->GetAppID();
	}
};

void GameCenterSteamImpl::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (_appID == pCallback->m_nGameID)
	{
		if (k_EResultOK == pCallback->m_eResult)
		{
			OutputDebugString("Received stats and achievements from Steam\n");
			_initialized = true;
		}
		else
		{
			char buffer[128];
			_snprintf(buffer, 128, "RequestStats - failed, %d\n", pCallback->m_eResult);
			OutputDebugString(buffer);
		}
	}
}

void GameCenterSteamImpl::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (_appID == pCallback->m_nGameID)
	{
		if (k_EResultOK == pCallback->m_eResult)
		{
			OutputDebugString("Stored stats for Steam\n");
		}
		else
		{
			char buffer[128];
			_snprintf(buffer, 128, "StatsStored - failed, %d\n", pCallback->m_eResult);
			OutputDebugString(buffer);
		}
	}
}

void GameCenterSteamImpl::OnAchievementStored(UserAchievementStored_t *pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (_appID == pCallback->m_nGameID)
	{
		OutputDebugString("Stored Achievement for Steam\n");
	}
}

GameCenter* GameCenter::getInstance()
{
    static GameCenter* instance = new GameCenter();
    return instance;
}

void GameCenter::login()
{
	GameCenterSteamImpl::getInstance().login();
}
    
void GameCenter::logout()
{
	GameCenterSteamImpl::getInstance().logout();
}
    
bool GameCenter::isLoggedIn()
{
    return GameCenterSteamImpl::getInstance().isLoggedIn();
}
    
bool GameCenter::showGameCenter()
{
	return GameCenterSteamImpl::getInstance().showGameCenter();
}

bool GameCenter::showAchievements()
{
	return GameCenterSteamImpl::getInstance().showAchievements();
}

void GameCenter::postAchievement(const std::string &idName, int percentComplete, bool showBanner)
{
	GameCenterSteamImpl::getInstance().postAchievement(idName, percentComplete, showBanner);
}

void GameCenter::clearAllAchievements()
{
	GameCenterSteamImpl::getInstance().clearAllAchievements();
}

std::string GameCenter::getPlayerId()
{
    return GameCenterSteamImpl::getInstance().getPlayerId();
}

bool GameCenter::showScores()
{
	return GameCenterSteamImpl::getInstance().showScores();
}

void GameCenter::postScore(const std::string &idName, int score)
{
	GameCenterSteamImpl::getInstance().postScore(idName, score);
}

void GameCenter::clearAllScores()
{
	GameCenterSteamImpl::getInstance().clearAllScores();
}

} // namespace avalon
