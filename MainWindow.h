#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QAction>
#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableWidget>

#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QMutex>
#include <QWidget>

#include <memory>

enum ActiveTable {NONE, LEFT, RIGHT};

#ifndef __MW_H_DEFINED
    #define __MW_H_DEFINED
    class MainWindow;
    #include "ExecThread.h"
#endif // __MW_H_DEFINED

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
    void cmdCopy(void);
    void cmdDelete(void);
    void cmdNewFolder(void);
	void leftDoubleClickHandler(int, int);
    void rightDoubleClickHandler(int, int);
    void leftClickHandler(int, int);
    void rightClickHandler(int, int);
    void showMessage1(QString);
    void showMessage2(QString);
    void showMessage(QString);
    void showMessage(QString, QString);
    void cmdCompleted();

private:
    QMutex m_leftMutex;
    QMutex m_rightMutex;
    QLabel* m_labelStatus1;
    QLabel* m_labelStatus2;
    QSplitter* m_splitter;
    QTableWidget* m_leftTable;
    QTableWidget* m_rightTable;
    QDir m_leftDir;
    QDir m_rightDir;
    ActiveTable m_active;
    
    ExecThread* m_execThread;
    
    void initMenu(void);
    void initToolbar(void);
    void initStatusbar(void);
    void showLeftTable();
    void showRightTable();
    bool restoreSettings(void);
    QIcon getFileIcon(QFileInfo);
    void showTable(QTableWidget*, QString);
    void saveTableSettings(QTableWidget*, QString);
    void restoreTableSettings(QTableWidget*, QString);
    void showDir(QTableWidget*, QDir);
    void showDir(QTableWidget*, QString);
    void showDirs();
    QDir showParentDir(QTableWidget*,QDir);
    void readSettings();
    QDir doubleClickHandler(QTableWidget*, int, int, QDir);   
    QDir doubleClickHandler(QTableWidget*, int, int, QString); 
    void clearSelection(QTableWidget*);
    QString getSrcPath();
    QString getDstPath();
    QTableWidget* getSrcTable();
    QTableWidget* getDstTable();
    void saveTableDir(QDir, QString);
    QString getTableDir(QString);
    QString getLeftDirPath();
    QString getRightDirPath();
    void setLeftDirPath(QString);
    void setRightDirPath(QString);
protected:
	void closeEvent(QCloseEvent*);
};

#endif // MAINWINDOW_H
