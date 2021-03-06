/*
* An example demonstrating recursive directory traversal.
*
* Compile this file with Visual Studio 2008 project vs2008.sln and run
* the produced command in console with a directory name argument.  For
* example, command
*
*     find "C:\Program Files"
*
* might produce a listing with thousands of entries such as
*
*     c:\Program Files/7-Zip/7-zip.chm
*     c:\Program Files/7-Zip/7-zip.dll
*     c:\Program Files/7-Zip/7z.dll
*     c:\Program Files/Adobe/Reader 10.0/Reader/logsession.dll
*     c:\Program Files/Adobe/Reader 10.0/Reader/LogTransport2.exe
*     c:\Program Files/Windows NT/Accessories/wordpad.exe
*     c:\Program Files/Windows NT/Accessories/write.wpc
*
* The find command provided by this file is only an example.  That is,
* the command does not provide options to restrict the output to certain
* files as the Linux version does.
*
* Copyright (C) 2006-2012 Toni Ronkko
* This file is part of dirent.  Dirent may be freely distributed
* under the MIT license.  For all details and documentation, see
* https://github.com/tronkko/dirent
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dirent.h"
#include <Shlwapi.h>
#include <iostream>

#include "CalcRadiusReader.h"
#include <CommonHelper.h>
#include "DGFXReader.h"
#include <DL_Debug.h>
#include "EntityReader.h"
#include <Engine.h>
#include "LevelReader.h"
#include "ModelCountReader.h"
#include "TerrainReader.h"
#include "IReader.h"
#include <TimerManager.h>
#include <MemoryMacros.h>
#include "HashReader.h"

#pragma comment(lib, "Shlwapi.lib")

static int find_directory(const char *dirname, IReader& aReader);
static int remove_files_with_extension(const char *dirname, const std::string& aExtension);
static void remove_file(const std::string& aFileName, const std::string& aExtension);
static int DeleteDirectory(const std::string &refcstrRootDirectory, bool bDeleteSubdirectories = true);


int main(int argC,      // Number of strings in array argv
	char *argV[],   // Array of command-line argument strings
	char *envP[])
{
	bool convertDGFX = false;
	bool calcCollisionRadius = false;
	bool readModelCount = false;
	bool readEntities = false;
	bool readLevel = false;
	bool removeOldFiles = false;
	bool hashFiles = false;

	for (int i = 0; i < argC; ++i)
	{
		std::string command(argV[i]);
		command = CU::ToLower(command);
		if (command == "-convertdgfx")
		{
			convertDGFX = true;
		}
		else if (command == "-calccollisionradius")
		{
			calcCollisionRadius = true;
		}
		else if (command == "-readmodelcount")
		{
			readModelCount = true;
		}
		else if (command == "-readentities")
		{
			readEntities = true;
		}
		else if (command == "-readlevel")
		{
			readLevel = true;
		}
		else if (command == "-removeoldfiles")
		{
			removeOldFiles = true;
		}
		else if (command == "-hashfiles")
		{
			hashFiles = true;
		}
	}

	DL_Debug::Debug::Create();
	//CU::TimerManager::Create();
	

	if (removeOldFiles == true)
	{
		DeleteDirectory("GeneratedData", true);
		DeleteDirectory(CU::GetMyDocumentFolderPath() + "Resource", true);
	}


	IReader* reader;

	if (calcCollisionRadius == true)
	{
		system("CLS");
		std::cout << "---| Calculating CollisionRadius |---\n" << std::endl;

		reader = new CalcRadiusReader();

		find_directory("Data", *reader);
		delete reader;

		std::cout << "---| CollisionRadius Done |---\n" << std::endl;
		system("CLS");
	}

	if (convertDGFX == true)
	{
		system("CLS");
		std::cout << "---| Converting FBX to DGFX |---\n" << std::endl;

		reader = new DGFXReader();
		std::cout << "Removing old Files\n" << std::endl;
		remove_files_with_extension("Data", "dgfx");

		find_directory("Data", *reader);
		delete reader;
		std::cout << "---| DGFX Converting Done |---\n" << std::endl;
		system("CLS");
	}

	if (readLevel == true)
	{
		system("CLS");
		std::cout << "\n---| Reading Level |---\n" << std::endl;

		reader = new LevelReader("GeneratedData/LI_level.xml", "GeneratedData/levelcount.bin");
		find_directory("Data/Level", *reader);
		delete reader;

		reader = new LevelReader("GeneratedData/LI_level_nightmare.xml", "GeneratedData/levelcount_nightmare.bin");
		find_directory("Data/NightmareLevel", *reader);
		delete reader;

		std::cout << "\n---| Level Reading Done |---\n" << std::endl;
		system("CLS");
	}


	if (readModelCount == true)
	{
		system("CLS");
		std::cout << "\n---| Reading ModelCount |---\n" << std::endl;

		reader = new ModelCountReader();
		reader->ReadFile("GeneratedData/LI_level.xml");
		reader->ReadFile("GeneratedData/LI_level_nightmare.xml");
		delete reader;

		std::cout << "\n---| ModelCount Reading Done |---\n" << std::endl;
		system("CLS");
	}

	if (readEntities == true)
	{
		system("CLS");
		std::cout << "\n---| Reading Entities |---\n" << std::endl;

		reader = new EntityReader();
		find_directory("Data/Resource/Entity", *reader);

		delete reader;

		std::cout << "\n---| Entities Reading Done |---\n" << std::endl;
		system("CLS");
	}

	if (hashFiles == true)
	{
		system("CLS");
		std::cout << "\n---| Hasing Files |---\n" << std::endl;

		reader = new HashReader();
		find_directory("Data/", *reader);
		delete reader;

		std::cout << "\n---| Hasing Files Done |---\n" << std::endl;
		system("CLS");
	}

	//CU::TimerManager::Destroy();
	Prism::MemoryTracker::Destroy();
	DL_Debug::Debug::Destroy();


	return EXIT_SUCCESS;
}

/* Find files and subdirectories recursively */
static int find_directory(const char *dirname, IReader& aReader)
{
	DIR *dir;
	char buffer[PATH_MAX + 2];
	char *p = buffer;
	const char *src;
	char *end = &buffer[PATH_MAX];
	int ok;

	/* Copy directory name to buffer */
	src = dirname;
	while (p < end  &&  *src != '\0') {
		*p++ = *src++;
	}
	*p = '\0';

	/* Open directory stream */
	dir = opendir(dirname);
	if (dir != NULL) {
		struct dirent *ent;

		/* Print all files and directories within the directory */
		while ((ent = readdir(dir)) != NULL) {
			char *q = p;
			char c;

			/* Get final character of directory name */
			if (buffer < q) {
				c = q[-1];
			}
			else {
				c = ':';
			}

			/* Append directory separator if not already there */
			if (c != ':'  &&  c != '/'  &&  c != '\\') {
				*q++ = '/';
			}

			/* Append file name */
			src = ent->d_name;
			while (q < end  &&  *src != '\0') {
				*q++ = *src++;
			}
			*q = '\0';

			/* Decide what to do with the directory entry */
			switch (ent->d_type) {
			case DT_LNK:
			case DT_REG:
				/* Output file name with directory */
				//printf("%s\n", buffer);
				aReader.ReadFile(buffer);
				break;

			case DT_DIR:
				/* Scan sub-directory recursively */
				if (strcmp(ent->d_name, ".") != 0
					&& strcmp(ent->d_name, "..") != 0) {
					find_directory(buffer, aReader);
				}
				break;

			default:
				/* Ignore device entries */
				/*NOP*/;
			}

		}

		closedir(dir);
		ok = 1;

	}
	else {
		/* Could not open directory */
		printf("Cannot open directory %s\n", dirname);
		ok = 0;
	}

	return ok;
}

/* Find files and subdirectories recursively */
static int remove_files_with_extension(const char *dirname, const std::string& aExtension)
{
	DIR *dir;
	char buffer[PATH_MAX + 2];
	char *p = buffer;
	const char *src;
	char *end = &buffer[PATH_MAX];
	int ok;

	/* Copy directory name to buffer */
	src = dirname;
	while (p < end  &&  *src != '\0') {
		*p++ = *src++;
	}
	*p = '\0';

	/* Open directory stream */
	dir = opendir(dirname);
	if (dir != NULL) {
		struct dirent *ent;

		/* Print all files and directories within the directory */
		while ((ent = readdir(dir)) != NULL) {
			char *q = p;
			char c;

			/* Get final character of directory name */
			if (buffer < q) {
				c = q[-1];
			}
			else {
				c = ':';
			}

			/* Append directory separator if not already there */
			if (c != ':'  &&  c != '/'  &&  c != '\\') {
				*q++ = '/';
			}

			/* Append file name */
			src = ent->d_name;
			while (q < end  &&  *src != '\0') {
				*q++ = *src++;
			}
			*q = '\0';

			/* Decide what to do with the directory entry */
			switch (ent->d_type) {
			case DT_LNK:
			case DT_REG:
				/* Output file name with directory */
				//printf("%s\n", buffer);
				remove_file(buffer, aExtension);
				break;

			case DT_DIR:
				/* Scan sub-directory recursively */
				if (strcmp(ent->d_name, ".") != 0
					&& strcmp(ent->d_name, "..") != 0) {
					remove_files_with_extension(buffer, aExtension);
				}
				break;

			default:
				/* Ignore device entries */
				/*NOP*/;
			}

		}

		closedir(dir);
		ok = 1;

	}
	else {
		/* Could not open directory */
		printf("Cannot open directory %s\n", dirname);
		ok = 0;
	}

	return ok;
}

static void remove_file(const std::string& aFileName, const std::string& aExtension)
{
	std::string generatedPath = CU::GetGeneratedDataFolderFilePath(aFileName, aExtension);
	std::ifstream stream(generatedPath);
	bool shouldDelete = false;
	if (stream.good())
	{
		shouldDelete = true;
	}
	stream.close();
	if (shouldDelete)
	{
		DeleteFile(generatedPath.c_str());
	}

	int slashIndex = generatedPath.find_last_of("/");
	std::string folder(generatedPath.begin(), generatedPath.begin() + slashIndex);

	while (PathIsDirectoryEmpty(folder.c_str()) == TRUE)
	{
		RemoveDirectory(folder.c_str());

		slashIndex = folder.find_last_of("/");
		if (slashIndex != std::string::npos)
		{
			folder = std::string(folder.begin(), folder.begin() + slashIndex);
		}
		else
		{
			folder = std::string(folder.begin(), folder.end());
		}
		
	}
}

static int DeleteDirectory(const std::string &refcstrRootDirectory, bool bDeleteSubdirectories)
{
	bool            bSubdirectory = false;       // Flag, indicating whether
	// subdirectories have been found
	HANDLE          hFile;                       // Handle to directory
	std::string     strFilePath;                 // Filepath
	std::string     strPattern;                  // Pattern
	WIN32_FIND_DATA FileInformation;             // File information


	strPattern = refcstrRootDirectory + "\\*.*";
	hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FileInformation.cFileName[0] != '.')
			{
				strFilePath.erase();
				strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (bDeleteSubdirectories)
					{
						// Delete subdirectory
						int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
						if (iRC)
							return iRC;
					}
					else
						bSubdirectory = true;
				}
				else
				{
					// Set file attributes
					if (::SetFileAttributes(strFilePath.c_str(),
						FILE_ATTRIBUTE_NORMAL) == FALSE)
						return ::GetLastError();

					// Delete file
					if (::DeleteFile(strFilePath.c_str()) == FALSE)
						return ::GetLastError();
				}
			}
		} while (::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
			return dwError;
		else
		{
			if (!bSubdirectory)
			{
				// Set directory attributes
				if (::SetFileAttributes(refcstrRootDirectory.c_str(),
					FILE_ATTRIBUTE_NORMAL) == FALSE)
					return ::GetLastError();

				// Delete directory
				if (::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
					return ::GetLastError();
			}
		}
	}

	return 0;
}
