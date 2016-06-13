#include "stdafx.h"
#pragma warning( disable : 4706 )
#include "CommonHelper.h"
#include <fstream>
#include <string>
#include "SQLWrapper.h"

CU::SQLWrapper* CU::SQLWrapper::myInstance = nullptr;
namespace CU
{
	SQLWrapper::SQLWrapper()
		: myResult(nullptr)
		, myConnection(nullptr)
		, myIsOnline(false)
	{
		myConnection = mysql_init(nullptr);
		if (myConnection == nullptr)
		{
			WriteError(myConnection);
		}
	}

	SQLWrapper::~SQLWrapper()
	{
		if (myResult != nullptr)
		{
			mysql_free_result(myResult);
			myResult = nullptr;
		}
		if (myConnection != nullptr)
		{
			mysql_close(myConnection);
			myConnection = nullptr;
		}
	}

	void SQLWrapper::Create()
	{
		if (myInstance == nullptr)
		{
			myInstance = new SQLWrapper();
		}
	}
	void SQLWrapper::Destroy()
	{
		if (myInstance != nullptr)
		{
			SAFE_DELETE(myInstance);
		}
	}
	SQLWrapper* SQLWrapper::GetInstance()
	{
		return myInstance;
	}


	bool SQLWrapper::Connect(const char* aServerAddress, const char* aUserName, const char* aPassword,
		const char* aDatabaseName, unsigned long aClientFlag, const int aPort, const char* aUnixSocket)
	{
		myDatabaseName = aDatabaseName;
		int timeout = 3;
		mysql_options(myConnection, MYSQL_OPT_READ_TIMEOUT, &timeout);
		mysql_options(myConnection, MYSQL_OPT_WRITE_TIMEOUT, &timeout);
		mysql_options(myConnection, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
		mysql_options(myConnection, MYSQL_OPT_COMPRESS, nullptr);
		if (mysql_real_connect(myConnection, aServerAddress, aUserName, aPassword, aDatabaseName, aPort, aUnixSocket, aClientFlag) == nullptr)
		{
			WriteError(myConnection);
			myIsOnline = false;
		}
		else
		{
			myIsOnline = true;
		}
		return myIsOnline;
	}

	MYSQL_RES* SQLWrapper::ExecuteQuery(const char* aQuery)
	{
		if (GetIsOnline() == true)
		{
			if (mysql_query(myConnection, aQuery))
			{
				WriteError(myConnection);
				DL_ASSERT("Tried to write in database while not online. Should not even come in here.");
			}
			if (myResult != nullptr)
			{
				mysql_free_result(myResult);
			}
			myResult = mysql_store_result(myConnection);
			if (myResult == nullptr)
			{
				mysql_next_result(myConnection);
				myResult = mysql_store_result(myConnection);
				WriteError(myConnection);
			}
			return myResult;
		}
		return nullptr;
	}

	void SQLWrapper::WriteHighscore(const std::string& aUsername, float aTime, int aLevelID)
	{
		if (GetIsOnline() == true)
		{
			char name[256];
			mysql_real_escape_string(myConnection, name, aUsername.c_str(), aUsername.length());
			std::string username(name);
			std::string query = "INSERT INTO Highscore(Name, Score, LevelID) VALUES('" + username + "', " + std::to_string(aTime) + ", " + std::to_string(aLevelID) + ")";
			ExecuteQuery(query.c_str());
			CheckAndClearRankHigherThanMax(aLevelID);
		}
	}

	void SQLWrapper::WriteDeaths(const int aLevelID, const int aNumberOfDeaths)
	{
		if (GetIsOnline() == true)
		{
			std::string query = "UPDATE DeathTable SET DeathCount=DeathCount+" + std::to_string(aNumberOfDeaths) + " WHERE LevelID = " + std::to_string(aLevelID);
			ExecuteQuery(query.c_str());
		}
	}

	void SQLWrapper::WriteHatStat(const int aHatID)
	{
		if (aHatID == -1) return;
		if (GetIsOnline() == true)
		{
			std::string query = "UPDATE HatStat SET UsageCount=UsageCount+1 WHERE HatID=" + std::to_string(aHatID);
			ExecuteQuery(query.c_str());
		}
	}

	CU::GrowingArray<Highscore> SQLWrapper::RetriveOnlineHighcore(const int aLevelID, const float aScore)
	{
		CU::GrowingArray<Highscore> toReturn(11);
		if (GetIsOnline() == true)
		{
			std::string query("SET @rownum:=0; SELECT Rank, Name, Score, LevelID FROM( SELECT @rownum:=@rownum+1 AS Rank, Score, Name, LevelID FROM (SELECT DISTINCT * FROM Highscore ORDER BY Score) small WHERE LevelID = " + std::to_string(aLevelID) + " ORDER BY Score ) result");
			ExecuteQuery(query.c_str());
			if (myResult != nullptr)
			{
				int num_fields = mysql_num_fields(myResult);
				MYSQL_ROW row;
				int num_rows = 0;
				while ((row = mysql_fetch_row(myResult)))
				{
					Highscore score;
					for (int i = 0; i < num_fields; ++i)
					{
						if (i == 0)
						{
							score.myRank = std::atoi(row[i]);
						}
						else if (i == 1)
						{
							score.myName = row[i];
						}
						else if (i == 2)
						{
							score.myScore = static_cast<float>(std::atof(row[i]));
						}
						else if (i == 3)
						{
							score.myLevelID = std::atoi(row[i]);
						}
					}
					toReturn.Add(score);
					num_rows++;
					if (num_rows >= 10)
					{
						break;
					}
				}
			}
			if (aScore != 0)
			{
				query = "SET @rownum:=0; SELECT Rank, Name, Score, LevelID FROM( SELECT @rownum:=@rownum+1 AS Rank, Score, Name, LevelID FROM (SELECT DISTINCT * FROM Highscore ORDER BY Score) small WHERE LevelID = " + std::to_string(aLevelID) + " ORDER BY Score ) result WHERE Score = " + std::to_string(aScore) + " AND Name = '" + CU::GetUsername() + "'";
				ExecuteQuery(query.c_str());
				if (myResult != nullptr)
				{
					MYSQL_ROW row;
					int num_fields = mysql_num_fields(myResult);
					while ((row = mysql_fetch_row(myResult)))
					{
						Highscore score;
						for (int i = 0; i < num_fields; ++i)
						{
							if (i == 0)
							{
								score.myRank = std::atoi(row[i]);
							}
							else if (i == 1)
							{

								score.myName = row[i];
							}
							else if (i == 2)
							{

								score.myScore = static_cast<float>(std::atof(row[i]));
							}
							else if (i == 3)
							{
								score.myLevelID = std::atoi(row[i]);
							}
						}
						toReturn.Add(score);
						break;
					}
				}
			}
		}
		return toReturn;
	}

	const float SQLWrapper::RetriveLocalHighscore(const int aLevelID, const float)
	{
		float toReturn = 0.f;
		std::fstream file;
		file.open(CU::GetMyDocumentFolderPath() + "Data/Score/Score" + std::to_string(aLevelID) + ".bin", std::ios::binary | std::ios::in);

		if (file.is_open() == true)
		{
			int levelID = 0;
			float time = 0;
			int stars = 0;
			bool isEndOfFile = false;
			while (isEndOfFile == false)
			{
				if (file.eof())
				{
					isEndOfFile = true;
					break;
				}
				file >> levelID >> time >> stars;
				toReturn = time;
			}

			file.close();
		}
		return toReturn;
	}

	bool SQLWrapper::GetIsOnline()
	{
		myIsOnline = false;
		if (mysql_ping(myConnection) == 0)
		{
			myIsOnline = true;
		}
		return myIsOnline;
	}

	void SQLWrapper::WriteError(MYSQL* aMySQL)
	{
		std::string error = mysql_error(aMySQL);
		if (error != "")
		{
			DL_DEBUG("[Warning]: %s", error.c_str());
		}
	}

	void SQLWrapper::CheckAndClearRankHigherThanMax(const int aLevelID)
	{
		std::string query("SET @rownum:=0; SELECT Rank, Name, Score, LevelID, ID FROM( SELECT @rownum:=@rownum+1 AS Rank, Score, Name, LevelID, ID FROM (SELECT * FROM Highscore ORDER BY Score) small WHERE LevelID = " + std::to_string(aLevelID) + " ORDER BY Score) result ORDER BY Rank DESC");
		ExecuteQuery(query.c_str());
		if (myResult != nullptr)
		{
			int num_fields = mysql_num_fields(myResult);
			MYSQL_ROW row;
			CU::GrowingArray<int> toRemove(32);
			while ((row = mysql_fetch_row(myResult)))
			{
				Highscore score;
				for (int i = 0; i < num_fields; ++i)
				{
					if (i == 0)
					{
						score.myRank = std::atoi(row[i]);
					}
					else if (i == 1)
					{
						score.myName = row[i];
					}
					else if (i == 2)
					{
						score.myScore = static_cast<float>(std::atof(row[i]));
					}
					else if (i == 3)
					{
						score.myLevelID = std::atoi(row[i]);
					}
					else if (i == 4)
					{
						if (score.myRank > 256)
						{
							toRemove.Add(std::atoi(row[i]));
						}
					}
				}
				
				if (score.myRank <= 256)
				{
					//This makes sure that we don't delete anything below 256 and is an early exit.
					break;
				}
			}

			std::string deleteQuery;
			for each (int score in toRemove)
			{
				deleteQuery = "DELETE FROM Highscore WHERE ID = " + std::to_string(score);
				ExecuteQuery(deleteQuery.c_str());
			}
		}

	}
}