cmake_minimum_required (VERSION 3.8)

if(UNIX)
target_compile_options(FTPSync PRIVATE -Wno-deprecated-declarations)
endif(UNIX)

target_sources (FTPSync PRIVATE

	FtpClient/BlockingSocket.cpp
	FtpClient/FTPClient.cpp
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
