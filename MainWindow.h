#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
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
#include <QWidget>

enum ActiveTable {NONE, LEFT, RIGHT};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	
public slots:
    void copy_cmd(void);
	void leftDoubleClickHandler(int, int);
    void rightDoubleClickHandler(int, int);

private:
    QSplitter* m_splitter;
    QTableWidget* m_leftTable;
    QTableWidget* m_rightTable;
    QDir m_leftDir;
    QDir m_rightDir;
    ActiveTable m_active;
    
    void initMenu(void);
    void initToolbar(void);
    void showLeftTable();
    void showRightTable();
    bool restoreSettings(void);
    QIcon getFileIcon(QFileInfo);
    void showTable(QTableWidget*, QString);
    void saveTableSettings(QTableWidget*, QString);
    void restoreTableSettings(QTableWidget*, QString);
    void showDir(QTableWidget*, QDir);
    QDir showParentDir(QTableWidget*,QDir);
    void readSettings();
    QDir doubleClickHandler(QTableWidget*, int, int, QDir);    
    std::string exec(std::string);
protected:
	void closeEvent(QCloseEvent*);
};

#endif // MAINWINDOW_H
