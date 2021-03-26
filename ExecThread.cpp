
#include "ExecThread.h"

#include <iostream>

#define BUF_SIZE 1024

ExecThread::ExecThread(QWidget *parent)
    : QThread(parent)
{
    m_mainWindow = (MainWindow*)parent;
    QObject::connect(this, SIGNAL(sigShowMessage1(QString)),
                    m_mainWindow, SLOT(showMessage1(QString)));
    QObject::connect(this, SIGNAL(sigShowMessage2(QString)),
                    m_mainWindow, SLOT(showMessage2(QString)));
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
            //std::cout << std::hex << buffer[0] << std::endl;
            //statusBar()->showMessage(QString(buffer));
            if(buffer[0] == 0xa)
            {
                //statusBar()->showMessage(QString::fromStdString(output));
                //std::cout << "[ExecThread] set result empty" << std::endl;
                QString s = QString::fromStdString(output);
                emit sigShowMessage1(s);
                output = "";
            }
            else if (buffer[0] == 0xd)
            {
                QString s = QString::fromStdString(output);
                emit sigShowMessage2(s);
                output = "";
            }
            else
            {
                output += buffer[0];
                //std::cout << "[ExecThread] " << output.length() << std::endl;
            }
        }
        
        //std::cout << std::dec << std::endl;
        
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    //std::cout << "[ExecThread] output:" << output << std::endl;
    
    emit sigCmdCompleted();
    
    return output;
}
