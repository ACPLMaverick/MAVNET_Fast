#pragma once

#include "FTPSync.h"

enum class WorkMode : uint8_t
{
	Unknown,
	Pull,
	Push
};

class CommandParser
{
public:
	CommandParser(int argc, char* argv[]);
	~CommandParser();

	inline WorkMode GetMode() { return m_workMode.Value; }
	inline const std::string& GetIpAddress() const { return m_ipAddress.Value; }
	inline const std::string& GetLogin() const { return m_login.Value; }
	inline const std::string& GetPassword() const { return m_password.Value; }
	inline const std::string& GetLocalPath() const { return m_localPath.Value; }
	inline const std::string& GetRemotePath() const { return m_remotePath.Value; }
	inline const std::vector<std::string>& GetFilters() const { return m_filters.Value; }
	inline const std::string& GetWorkingDir() const { return m_workingDir; }

private:

	template <typename ValueType> struct FlagValuePair
	{
		const std::string Flag;
		ValueType Value;

		FlagValuePair(const std::string& flag, const ValueType& value = ValueType())
			: Flag(flag)
			, Value(value)
		{
		}
	};

	void DecodeCommands(int argc, char* argv[]);
	static bool IsFlag(const std::string& arg);
	static void ToLower(std::string& arg);

	FlagValuePair<WorkMode> m_workMode;
	FlagValuePair<std::string> m_ipAddress;
	FlagValuePair<std::string> m_login;
	FlagValuePair<std::string> m_password;
	FlagValuePair<std::string> m_localPath;
	FlagValuePair<std::string> m_remotePath;
	FlagValuePair<std::vector<std::string>> m_filters;

	std::string m_workingDir;
};