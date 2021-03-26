#ifndef EXECTHREAD_H
#define EXECTHREAD_H

#include <QThread>
#include <memory>

#ifndef __ET_H_DEFINED
    #define __ET_H_DEFINED
    class ExecThread;	
    #include "MainWindow.h"	
#endif // __ET_H_DEFINED

class ExecThread : public QThread
{

	Q_OBJECT

public:
	explicit ExecThread(QWidget *parent = 0);
	void init(QString);
	void run();
    bool isCompleted();
	QString getResult();
signals:
	void sigShowMessage1(QString);
    void sigShowMessage2(QString);
    void sigCmdCompleted();
private:
    MainWindow* m_mainWindow;
	QString m_cmd;
    QString m_result;
    bool m_state;
    std::string exec(std::string);
};

#endif // EXECTHREAD_H
