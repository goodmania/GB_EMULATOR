#include <iostream>
#include "logger.h"

/* �R���\�[���o�͕����F����R�[�h */
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

/* �R���X�g���N�^ */
FileLogger::FileLogger()
    : m_FilePath("")
    , m_Stream()
{
    /* Nothing to do */
}
/* �R���X�g���N�^ */
FileLogger::FileLogger(const std::string& file_path)
    : m_FilePath(file_path)
    , m_Stream()
{
    /* �t�@�C���I�[�v�� */
    this->Open(file_path);
}

/* �f�X�g���N�^ */
FileLogger::~FileLogger()
{
    /* �t�@�C���N���[�Y */
    this->Close();
}

/* �t�@�C���I�[�v�� */
void FileLogger::Open(const std::string& file_path)
{
    /* �t�@�C�����J���Ă��Ȃ������� */
    if (this->IsOpened() == false)
    {
        this->m_Stream.open(file_path, std::ios::out);
    }
}

/* �t�@�C���N���[�Y */
void FileLogger::Close()
{
    /* �t�@�C�����J���Ă����� */
    if (this->IsOpened() == true)
    {
        this->m_Stream.close();
    }
}

/* �t�@�C���I�[�v���m�F */
bool FileLogger::IsOpened()
{
    return this->m_Stream.is_open();
}

/* �t�@�C���������� */
void FileLogger::Write(const std::string& log)
{
    /* �t�@�C�����J���Ă����� */
    if (this->IsOpened() == true)
    {
        /* �t�@�C���X�g���[���ɏ������� */
        this->m_Stream << log << std::endl;
    }
}

/* �V���O���g���C���X�^���X�擾 */
Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

/* �R���X�g���N�^ */
Logger::Logger()
    : m_Mutex()
    , m_FileLogger()
{
    /* ���O�t�@�C���I�[�v�� */
    this->m_FileLogger.Open("log.txt");
}

/* �f�X�g���N�^ */
Logger::~Logger()
{
    /* ���O�t�@�C���N���[�Y */
    this->m_FileLogger.Close();
}

/* ���O�o�� */
void Logger::writeLog(const std::string& message)
{
    /* �~���[�e�b�N�X�ɂ��r������ */
    std::lock_guard<std::mutex> lock(this->m_Mutex);
    /* �t�@�C���ɏo�� */
    this->m_FileLogger.Write(message);
}