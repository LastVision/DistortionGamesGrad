#pragma once
//#include <my_global.h>
#include <string>
#include <mysql.h>
#include "GrowingArray.h"

struct Highscore
{
	bool operator==(const Highscore& aScore)
	{
		return (myName == aScore.myName && myScore == aScore.myScore && myLevelID == aScore.myLevelID);
	}
	std::string myName;
	float myScore;
	int myLevelID;
	int myRank;
};

namespace CU
{
	class SQLWrapper
	{
	public:
		SQLWrapper();
		~SQLWrapper();

		bool Connect(const char* aServerAddress, const char* aUserName, const char* aPassword,
			const char* aDatabaseName, unsigned long aClientFlag = 0, const int aPort = 0, const char* aUnixSocket = NULL);

		MYSQL_RES* ExecuteQuery(const char* aQuery);
		void WriteHighscore(const std::string& aUsername, float aTime, int aLevelID);
		void WriteDeaths(const int aLevelID, const int aNumberOfDeaths);
		void WriteHatStat(const int aHatID);

		CU::GrowingArray<Highscore> RetriveOnlineHighcore(const int aLevelID, const float aScore = 0);
		//CU::GrowingArray<Highscore> RetriveLocalHighcore(const int aLevelID, const float aScore = 0);
		const float RetriveLocalHighscore(const int aLevelID, const float aScore = 0);

		bool GetIsOnline();
	private:
		void WriteError(MYSQL* aMySQL);
		void CheckAndClearRankHigherThanMax(const int aLevelID);

		bool myIsOnline;
		MYSQL* myConnection;
		MYSQL_RES* myResult;

		std::string myDatabaseName;
	};
};