#include <iostream>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
void DisplayError(LPTSTR lpszFunction)
// Routine Description:
// Retrieve and output the system error message for the last-error code
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);

	lpDisplayBuf =
		(LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf)
			+ lstrlen((LPCTSTR)lpszFunction)
			+ 40) // account for format string
			* sizeof(TCHAR));

	if (FAILED(StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error code %d as follows:\n%s"),
		lpszFunction,
		dw,
		lpMsgBuf)))
	{
		printf("FATAL ERROR: Unable to output error code.\n");
	}

	_tprintf(TEXT("ERROR: %s\n"), (LPCTSTR)lpDisplayBuf);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}
FILE_HANDLE FILE_BASE::Open( const char * pathName, FILE_ACCESS_FLAGS accessFlags, FILE_ACCESS_MODE accessMode )
{
	uint32 mode = 0;
	uint32 share = 0;
	switch (accessMode)
	{
	case Read:
		mode |= GENERIC_READ;
		share = FILE_SHARE_READ;
		break;
	case Write:
		mode |= GENERIC_WRITE;
		break;
	case ReadWrite:
		mode |= GENERIC_READ | GENERIC_WRITE;
		break;
	}
	uint32 flags = OPEN_ALWAYS;//might need to revise those flags meanings
	if (accessFlags.GetFlag(FILE_ACCESS_FLAGS::FLAGS::Truncate)
		&& accessFlags.GetFlag(FILE_ACCESS_FLAGS::FLAGS::Exists))
	{
		//std::cout << "TRUNCATE_EXISTING" << std::endl;
		flags = TRUNCATE_EXISTING;
	}
	else if (accessFlags.GetFlag(FILE_ACCESS_FLAGS::FLAGS::Create)
		&& !accessFlags.GetFlag(FILE_ACCESS_FLAGS::FLAGS::Exists))
	{
		//std::cout << "CREATE_NEW" << std::endl;
		flags = CREATE_NEW;
	}
	else if (accessFlags.GetFlag(FILE_ACCESS_FLAGS::FLAGS::Create))
	{
		//std::cout << "CREATE_ALWAYS" << std::endl;
		flags = CREATE_ALWAYS;
	}
	else if (accessFlags.GetFlag(FILE_ACCESS_FLAGS::FLAGS::Exists))
	{
		//std::cout << "OPEN_EXISTING" << std::endl;
		flags = OPEN_EXISTING;
	}
	/*else
	{
		std::cout << "OPEN_ALWAYS" << std::endl;
	}*/

	FILE_HANDLE ret = CreateFile( std::string( pathName ).c_str(), mode, share, NULL, flags, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( ret == INVALID_HANDLE_VALUE)
	{
		DisplayError(TEXT("CreateFile"));
	}
	return ret;
}

bool FILE_BASE::Close( FILE_HANDLE handle )
{
	BOOL result = CloseHandle(handle);
	if(result == FALSE)
	{
		DisplayError(TEXT("DeleteFile"));
		return false;
	}
	return true;
}

bool FILE_BASE::ReadFile( FILE_HANDLE handle, std::string & content )
{
	const DWORD bufferSize = GetFileSize(handle, NULL) + 1;
	char* buffer_read = new char[bufferSize];
	DWORD bytes_read = 0;
	if (FALSE == ::ReadFile(handle, buffer_read, bufferSize - 1, &bytes_read, NULL))
	{
		DisplayError(TEXT("ReadFile"));
		delete[] buffer_read;
		return false;
	}
	if (bytes_read >= 0 && bytes_read <= bufferSize - 1) //move buffer to string
	{
		buffer_read[bytes_read] = '\0'; // NULL character
		content = buffer_read;
		delete[] buffer_read;
		return true;
	}
	delete[] buffer_read;
	return false;
}

bool FILE_BASE::WriteFile( FILE_HANDLE handle, const std::string & content)
{
	DWORD bytesWritten = 0;
	if (FALSE == ::WriteFile(handle, content.c_str(), content.size(), &bytesWritten, NULL))
	{
		DisplayError(TEXT("WriteFile"));
		return false;
	}
	else if (bytesWritten != content.size()) return false;

	if (FALSE == ::FlushFileBuffers(handle))
	{
		DisplayError(TEXT("WriteFile->FlushFileBuffers"));
		return false;
	}
	return true;
}

bool FILE_BASE::DeleteFile( const char * pathName )
{
	BOOL result = ::DeleteFile( pathName );
	if(result == FALSE)
	{
		DisplayError(TEXT("DeleteFile"));
		return false;
	}
	return true;
}