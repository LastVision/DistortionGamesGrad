#include "stdafx.h"
#pragma warning( disable : 4706 )
#include "CommonHelper.h"
#include <fstream>
#include <string>
#include "SQLWrapper.h"

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

	bool SQLWrapper::Connect(const char* aServerAddress, const char* aUserName, const char* aPassword,
		const char* aDatabaseName, unsigned long aClientFlag, const int aPort, const char* aUnixSocket)
	{
		myDatabaseName = aDatabaseName;
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
		if (myIsOnline == true)
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
		std::fstream file(CU::GetMyDocumentFolderPath() + "/Data/HS_Level" + std::to_string(aLevelID) + ".bin",
			std::ios::binary | std::ios::in | std::ios::app);
		if (GetIsOnline() == true)
		{
			std::string query = "INSERT INTO Highscore(Name, Score, LevelID) VALUES('" + aUsername + "', " + std::to_string(aTime) + ", " + std::to_string(aLevelID) + ")";
			ExecuteQuery(query.c_str());
		}
		file << "\n" << aUsername << "\n" << aTime << "\n" << aLevelID;
		file.close();
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

	const float SQLWrapper::RetriveLocalHighscore(const int aLevelID, const float aScore)
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

	/*CU::GrowingArray<Highscore> SQLWrapper::RetriveLocalHighcore(const int aLevelID, const float aScore)
	{
		CU::GrowingArray<Highscore> toReturn(11);
		CU::GrowingArray<Highscore> unsortedHighscore(11);
		std::fstream file(CU::GetMyDocumentFolderPath() + "/Data/HS_Level" + std::to_string(aLevelID) + ".bin",
			std::ios::binary | std::ios::in);
		bool isEndOfFile = false;
		int rows = 0;
		while (isEndOfFile == false)
		{
			Highscore score;
			if (file.eof())
			{
				isEndOfFile = true;
				break;
			}

			file >> score.myName >> score.myScore >> score.myLevelID;
			rows++;
			unsortedHighscore.Add(score);
			if (rows >= 10)
			{
				break;
			}
		}
		file.close();
		int rank = 1;
		while (unsortedHighscore.Size() > 0)
		{
			Highscore bestScore;
			bestScore.myScore = 1000000.f;
			for each(Highscore unscore in unsortedHighscore)
			{
				if (bestScore.myScore >= unscore.myScore)
				{
					bestScore = unscore;
				}
			}
			bestScore.myRank = rank;
			unsortedHighscore.RemoveCyclic(bestScore);
			if (rank <= 10)
			{
				toReturn.Add(bestScore);
			}
			else if (bestScore.myScore == aScore)
			{
				toReturn.Add(bestScore);
			}
			rank++;
		}
		return toReturn;
	}*/

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
		DL_DEBUG("[Warning]: %s", mysql_error(aMySQL));
	}
}