// https://an-embedded-engineer.hateblo.jp/entry/2020/08/29/230546

#pragma once
#include <fstream>
#include <string>
#include <mutex>

/* File Logger�N���X�錾 */
class FileLogger
{
public:
    /* �R���X�g���N�^ */
    FileLogger();
    /* �R���X�g���N�^ */
    FileLogger(const std::string& file_path);
    /* �f�X�g���N�^ */
    ~FileLogger();
    /* �t�@�C���I�[�v�� */
    void Open(const std::string& file_path);
    /* �t�@�C���N���[�Y */
    void Close();
    /* �t�@�C���I�[�v���m�F */
    bool IsOpened();
    /* �t�@�C���������� */
    void Write(const std::string& log);
private:
    /* ���O�t�@�C���p�X */
    std::string m_FilePath;
    /* �t�@�C���o�̓X�g���[�� */
    std::ofstream m_Stream;
};

/* Logger�N���X�錾 */
class Logger
{
public:
    /* �V���O���g���C���X�^���X�擾 */
    static Logger& GetInstance();
    void writeLog(const std::string& message);

    /* �R�s�[�R���X�g���N�^���폜 */
    Logger(const Logger&) = delete;
    /* ���[�u�R���X�g���N�^���폜 */
    Logger(Logger&&) = delete;
    /* �R�s�[����I�y���[�^���폜 */
    Logger& operator=(const Logger&) = delete;
    /* ���[�u����I�y���[�^���폜 */
    Logger& operator=(Logger&&) = delete;

private:
    
    /* �R���X�g���N�^ */
    Logger();
    /* �f�X�g���N�^ */
    ~Logger();
    /* �~���[�e�b�N�X */
    std::mutex m_Mutex;
    /* File Logger�N���X�C���X�^���X */
    FileLogger m_FileLogger;
};