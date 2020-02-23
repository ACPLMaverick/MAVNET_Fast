#include "CommandParser.h"

CommandParser::CommandParser(int argc, char * argv[])
	: m_workMode("-m", WorkMode::Unknown)
	, m_ipAddress("-a", { "127.0.0.1", 2221 })
	, m_login("-u", "defaultLogin")
	, m_password("-p", "defaultPassword")
	, m_localPath("-d", "defaultLocalPath")
	, m_remotePath("-rd", "defaultRemotePath")
	, m_filters("-f")
	, m_workingDir("/")
{
	DecodeCommands(argc, argv);
}

CommandParser::~CommandParser()
{
}

void CommandParser::DecodeCommands(int argc, char * argv[])
{
	if (argc == 0)
	{
		return;
	}

	m_workingDir = argv[0];


	auto funcGetNextArg = [](char* argv[], int& index, std::string& to)
	{
		++index;
		to = argv[index];
	};

	auto funcGetNextArgLowercase = [](char* argv[], int& index, std::string& to)
	{
		++index;
		to = argv[index];
		ToLower(to);
	};

	auto funcPeekNextArg = [](int argc, char* argv[], const int index) -> const char*
	{
		if (index + 1 >= argc)
		{
			return "";
		}
		else
		{
			return argv[index + 1];
		}
	};

	std::string arg;
	for (int i = 1; i < argc; ++i)
	{
		arg = argv[i];

		if (arg.compare(m_workMode.Flag) == 0)
		{
			funcGetNextArgLowercase(argv, i, arg);

			if (arg.compare("pull") == 0)
			{
				m_workMode.Value = WorkMode::Pull;
			}
			else if (arg.compare("push") == 0)
			{
				m_workMode.Value = WorkMode::Push;
			}
			else
			{
				m_workMode.Value = WorkMode::Unknown;
				FTPS_Assert(false);
				return;	// No point in doing any further decoding...
			}
		}
		else if (arg.compare(m_ipAddress.Flag) == 0)
		{
			// Expecting ip addr and port here.
			static const uint16_t DEFAULT_PORT = 2221;

			funcGetNextArg(argv, i, arg);
			m_ipAddress.Value.IpAddress = arg;

			std::string nextArg = funcPeekNextArg(argc, argv, i);
			if (!IsFlag(nextArg) && !nextArg.empty())
			{
				funcGetNextArg(argv, i, arg);
				m_ipAddress.Value.Port = std::stoi(arg);
			}
			else
			{
				m_ipAddress.Value.Port = DEFAULT_PORT;
			}
		}
		else if (arg.compare(m_login.Flag) == 0)
		{
			funcGetNextArg(argv, i, arg);
			m_login.Value = arg;
		}
		else if (arg.compare(m_password.Flag) == 0)
		{
			funcGetNextArg(argv, i, arg);
			m_password.Value = arg;
		}
		else if (arg.compare(m_localPath.Flag) == 0)
		{
			funcGetNextArg(argv, i, arg);
			m_localPath.Value = arg;
		}
		else if (arg.compare(m_remotePath.Flag) == 0)
		{
			funcGetNextArg(argv, i, arg);
			m_remotePath.Value = arg;
		}
		else if (arg.compare(m_filters.Flag) == 0)
		{
			std::string nextArg = funcPeekNextArg(argc, argv, i);
			while (!nextArg.empty() && !IsFlag(nextArg))
			{
				funcGetNextArg(argv, i, arg);
				m_filters.Value.push_back(arg);

				nextArg = funcPeekNextArg(argc, argv, i);
			}
		}
	}
}

bool CommandParser::IsFlag(const std::string & arg)
{
	if (!(arg.size() == 2 || arg.size() == 3))
	{
		return false;
	}

	return arg[0] == '-';
}

void CommandParser::ToLower(std::string & arg)
{
	static const std::locale loc;

	for (std::string::size_type i = 0; i < arg.size(); ++i)
	{
		arg[i] = std::tolower(arg[i], loc);
	}
}
