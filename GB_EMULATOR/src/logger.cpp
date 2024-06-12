#include <iostream>
#include "logger.h"

/* コンソール出力文字色制御コード */
namespace {
    constexpr const char* ConsoleColorReset = "\x1b[0m";
    constexpr const char* ConsoleColorBlack = "\x1b[30m";
    constexpr const char* ConsoleColorRed = "\x1b[31m";
    constexpr const char* ConsoleColorGreen = "\x1b[32m";
    constexpr const char* ConsoleColorYellow = "\x1b[33m";
    constexpr const char* ConsoleColorBlue = "\x1b[34m";
    constexpr const char* ConsoleColorMagenta = "\x1b[35m";
    constexpr const char* ConsoleColorCyan = "\x1b[36m";
    constexpr const char* ConsoleColorWhite = "\x1b[37m";
}

/* コンストラクタ */
FileLogger::FileLogger()
    : m_FilePath("")
    , m_Stream()
{
    /* Nothing to do */
}
/* コンストラクタ */
FileLogger::FileLogger(const std::string& file_path)
    : m_FilePath(file_path)
    , m_Stream()
{
    /* ファイルオープン */
    this->Open(file_path);
}

/* デストラクタ */
FileLogger::~FileLogger()
{
    /* ファイルクローズ */
    this->Close();
}

/* ファイルオープン */
void FileLogger::Open(const std::string& file_path)
{
    /* ファイルが開いていなかったら */
    if (this->IsOpened() == false)
    {
        this->m_Stream.open(file_path, std::ios::out);
    }
}

/* ファイルクローズ */
void FileLogger::Close()
{
    /* ファイルが開いていたら */
    if (this->IsOpened() == true)
    {
        this->m_Stream.close();
    }
}

/* ファイルオープン確認 */
bool FileLogger::IsOpened()
{
    return this->m_Stream.is_open();
}

/* ファイル書き込み */
void FileLogger::Write(const std::string& log)
{
    /* ファイルが開いていたら */
    if (this->IsOpened() == true)
    {
        /* ファイルストリームに書き込み */
        this->m_Stream << log << std::endl;
    }
}

/* シングルトンインスタンス取得 */
Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

/* コンストラクタ */
Logger::Logger()
    : m_Mutex()
    , m_FileLogger()
{
    /* ログファイルオープン */
    this->m_FileLogger.Open("log.txt");
}

/* デストラクタ */
Logger::~Logger()
{
    /* ログファイルクローズ */
    this->m_FileLogger.Close();
}

/* ログ出力 */
void Logger::writeLog(const std::string& message)
{
    /* ミューテックスによる排他処理 */
    std::lock_guard<std::mutex> lock(this->m_Mutex);
    /* ファイルに出力 */
    this->m_FileLogger.Write(message);
}