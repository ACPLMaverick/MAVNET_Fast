cmake_minimum_required (VERSION 3.8)

target_sources (FTPSync PRIVATE

	FtpClient/BlockingSocket.cpp
	FTPClient/FTPClient.cpp
	FtpClient/FTPDataTypes.cpp
	FtpClient/FTPFileStatus.cpp
	FtpClient/FTPListParse.cpp
	
	FtpClient/smart_ptr.h

	FtpClient/BlockingSocket.h
	FtpClient/Definements.h
	FtpClient/FTPClient.h
	FtpClient/FTPDataTypes.h
	FtpClient/FTPFileStatus.h
	FtpClient/FTPListParse.h
)
