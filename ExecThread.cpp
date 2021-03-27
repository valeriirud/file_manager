
#include "ExecThread.h"

#include <iostream>

#define BUF_SIZE 1024

ExecThread::ExecThread(QWidget *parent)
    : QThread(parent)
{
    m_mainWindow = (MainWindow*)parent;
    //QObject::connect(this, SIGNAL(sigShowMessage1(QString)),
    //                m_mainWindow, SLOT(showMessage1(QString)));
    //QObject::connect(this, SIGNAL(sigShowMessage2(QString)),
    //                m_mainWindow, SLOT(showMessage2(QString)));
    QObject::connect(this, SIGNAL(sigShowMessage(QString, QString)),
                    m_mainWindow, SLOT(showMessage(QString, QString)));
    QObject::connect(this, SIGNAL(sigCmdCompleted()),
                    m_mainWindow, SLOT(cmdCompleted()));
}

void ExecThread::init(QString cmd)
{
	m_cmd = cmd;
    m_state = false;
}

void ExecThread::run()
{
	std::string s = exec(m_cmd.toStdString());
    
    //std::cout << "[ExecThread] result:" << s << std::endl;
    
    m_result = QString::fromStdString(s);
    m_state = true;
}

bool ExecThread::isCompleted()
{
    return m_state;
}

QString ExecThread::getResult()
{
    return m_result;
}

std::string ExecThread::exec(std::string cmd) 
{
    QString s1 = "";
    char buffer[BUF_SIZE];
    std::string output = "";
    std::cout << "[ExecThread] cmd:" << cmd << std::endl;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    
    buffer[0] = '\0';
    buffer[1] = '\0';
    FILE* f = pipe.get();
    // wait while command starting
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    while (! feof(f))
    {
        size_t rd = fread(buffer, 1, 1, f);
        if(rd > 0)
        {
            if(buffer[0] == 0xa)
            {
                s1 = QString::fromStdString(output);
                emit sigShowMessage(s1, "");
                output = "";
            }
            else if (buffer[0] == 0xd)
            {
                QString s2 = QString::fromStdString(output);
                emit sigShowMessage(s1, s2);
                output = "";
            }
            else
            {
                output += buffer[0];
            }
        }
    }
    
    emit sigCmdCompleted();
    
    return output;
}
