// https://an-embedded-engineer.hateblo.jp/entry/2020/08/29/230546

#pragma once
#include <fstream>
#include <string>
#include <mutex>

/* File Loggerクラス宣言 */
class FileLogger
{
public:
    /* コンストラクタ */
    FileLogger();
    /* コンストラクタ */
    FileLogger(const std::string& file_path);
    /* デストラクタ */
    ~FileLogger();
    /* ファイルオープン */
    void Open(const std::string& file_path);
    /* ファイルクローズ */
    void Close();
    /* ファイルオープン確認 */
    bool IsOpened();
    /* ファイル書き込み */
    void Write(const std::string& log);
private:
    /* ログファイルパス */
    std::string m_FilePath;
    /* ファイル出力ストリーム */
    std::ofstream m_Stream;
};

/* Loggerクラス宣言 */
class Logger
{
public:
    /* シングルトンインスタンス取得 */
    static Logger& GetInstance();
    void writeLog(const std::string& message);

    /* コピーコンストラクタを削除 */
    Logger(const Logger&) = delete;
    /* ムーブコンストラクタを削除 */
    Logger(Logger&&) = delete;
    /* コピー代入オペレータを削除 */
    Logger& operator=(const Logger&) = delete;
    /* ムーブ代入オペレータを削除 */
    Logger& operator=(Logger&&) = delete;

private:
    
    /* コンストラクタ */
    Logger();
    /* デストラクタ */
    ~Logger();
    /* ミューテックス */
    std::mutex m_Mutex;
    /* File Loggerクラスインスタンス */
    FileLogger m_FileLogger;
};