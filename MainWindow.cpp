
#include "Definitions.h"
#include "MainWindow.h"

#include <QtCore/QSettings>
#include <QtCore/QSize>

#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QMessageBox>

#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QFileIconProvider>
#include <QDateTime>
#include <QMenuBar>
#include <QPixmap>
#include <QStatusBar>
#include <QToolBar>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <chrono>
#include <thread>

#define BUF_SIZE 1024


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
    QString title = QString(APPLICATION_TITLE);
	setWindowTitle(title);
    
    readSettings();
    
    initMenu();
    initToolbar();
    
    showLeftTable();    
    showRightTable();
    
    m_splitter = new QSplitter(Qt::Horizontal);
    m_splitter->addWidget(m_leftTable);
    m_splitter->addWidget(m_rightTable);
    
    bool b = restoreSettings();
	if (! b)
	{
		resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
	}
	
	setCentralWidget(m_splitter);
    
    showDir(m_leftTable, m_leftDir);
    showDir(m_rightTable, m_rightDir);
    
    statusBar()->showMessage("");
    
    m_active = ActiveTable::NONE;
}


MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent* event)
{
	QSettings settings(DOMAIN_NAME, APPLICATION_NAME);
	settings.setValue("mainwindow/geometry", saveGeometry());
	settings.setValue("mainwindow/state", saveState());
    
    saveTableSettings(m_leftTable, "leftTable");
    saveTableSettings(m_rightTable, "rightTable");

	QMainWindow::closeEvent(event);
}

bool MainWindow::restoreSettings()
{
	QString key;
	bool b = true;

	QSettings settings(DOMAIN_NAME, APPLICATION_NAME);

	QVariant geometry = settings.value("mainwindow/geometry");
	if (!geometry.isNull() && geometry.isValid())
	{
		restoreGeometry(geometry.toByteArray());
	}
	else
	{
		b = false;
	}

	QSize size = QDesktopWidget().availableGeometry(this).size();
	QSize thisSize = this->size();

	if (thisSize.width() >= size.width() || thisSize.height() >= size.height())
	{
		setGeometry(50, 50, size.width()*0.9, thisSize.height()*0.9);

		return false;
	}
	
	QVariant state = settings.value("mainwindow/state");
	if (!state.isNull() && state.isValid())
	{
		restoreState(state.toByteArray());
	}
	else
	{
		b = false;
	}

	return b;
}

QIcon MainWindow::getFileIcon(QFileInfo fileInfo)
{
    QFileIconProvider fip;
    QIcon icon = fip.icon(fileInfo);
    return icon;
}

void MainWindow::showDir(QTableWidget* table, QDir dir)
{
    table->setRowCount(1);
    
    QTableWidgetItem *itemParent = new QTableWidgetItem("..");
    table->setItem(0, 0, itemParent);
    
    QFileInfoList list = dir.entryInfoList();
    int size = list.size();
    for (int i = 0; i < size; ++i) 
    {
        QFileInfo fileInfo = list.at(i);
        
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
        {
            continue;
        }
        QString fileName = fileInfo.baseName();
        QString fileType = fileInfo.completeSuffix();
        QDateTime created = fileInfo.lastModified();
        QDateTime localDateTime = created.toLocalTime();
        QString strDateTime = localDateTime.toString(Qt::SystemLocaleShortDate);
        QTableWidgetItem *itemName = new QTableWidgetItem(fileName);
        QTableWidgetItem *itemType = new QTableWidgetItem(fileType);
        QString fileSize = QString::number(fileInfo.size());
        QTableWidgetItem *itemSize = new QTableWidgetItem(fileSize);
        itemSize->setTextAlignment( Qt::AlignRight);
        QTableWidgetItem *itemOwner = new QTableWidgetItem(fileInfo.owner());
        QTableWidgetItem *itemDateTime= new QTableWidgetItem(strDateTime);
        QIcon icon = getFileIcon(fileInfo);
        QTableWidgetItem *itemIcon = new QTableWidgetItem;
        itemIcon->setIcon(icon);
        
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, itemIcon);
        table->setItem(row, 1, itemName);
        table->setItem(row, 2, itemType);
        table->setItem(row, 3, itemSize);
        table->setItem(row, 4, itemOwner);
        table->setItem(row, 5, itemDateTime);
    }
}

void MainWindow::showLeftTable()
{
    m_leftTable = new QTableWidget;
    showTable(m_leftTable, "leftTable");
    QObject::connect(m_leftTable, SIGNAL(cellClicked(int,int)), 
                    this, SLOT(leftClickHandler(int,int)));
    QObject::connect(m_leftTable, SIGNAL(cellDoubleClicked(int,int)), 
                    this, SLOT(leftDoubleClickHandler(int,int)));
}

void MainWindow::showRightTable()
{
    m_rightTable = new QTableWidget;
    showTable(m_rightTable, "rightTable");
    connect(m_rightTable, SIGNAL(cellDoubleClicked(int,int)), 
            this, SLOT(rightDoubleClickHandler(int,int)));
}

void MainWindow::showTable(QTableWidget* table, QString key)
{
    QStringList header;
    header<<""<<"name"<<"type"<<"size"<<"owner"<<"modified";
    
    table->setRowCount(1); 
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels(header);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    restoreTableSettings(table, key);
}


void MainWindow::saveTableSettings(QTableWidget* table, QString key)
{
    int w = 0;
    QSettings settings(DOMAIN_NAME, APPLICATION_NAME);
    for(int i = 0; i < 6; i ++)
    {
        w = table->columnWidth(i);
        settings.setValue(key + "/column" + QString::number(i), w);
    }        
}

void MainWindow::restoreTableSettings(QTableWidget* table, QString key)
{
    QSettings settings(DOMAIN_NAME, APPLICATION_NAME);
    for(int i = 0; i < 6; i ++)
    {
        QVariant w = settings.value(key + "/column" + QString::number(i));
        if (!w.isNull() && w.isValid())
        {
            table->setColumnWidth(i, w.toInt());
        }
    }        
}

void MainWindow::leftClickHandler(int row, int col)
{
    m_active = ActiveTable::LEFT;
}

void MainWindow::rightClickHandler(int row, int col)
{
    m_active = ActiveTable::RIGHT;
}

void MainWindow::leftDoubleClickHandler(int row, int col)
{
    m_active = ActiveTable::LEFT;
    m_leftDir = doubleClickHandler(m_leftTable, row, col, m_leftDir);
    statusBar()->showMessage(m_leftDir.absolutePath());
}

void MainWindow::rightDoubleClickHandler(int row, int col)
{
    m_active = ActiveTable::RIGHT;
    m_rightDir = doubleClickHandler(m_rightTable, row, col, m_rightDir);
    statusBar()->showMessage(m_rightDir.absolutePath());
}

void MainWindow::readSettings()
{
    m_leftDir = QDir("");
    m_rightDir = QDir("");
}

void MainWindow::initMenu()
{
    
    menuBar()->addMenu("&File");
    menuBar()->addMenu("&Edit");
    menuBar()->addMenu("&View");
    menuBar()->addMenu("&Simulate");
    menuBar()->addMenu("&Help");

}

void MainWindow::initToolbar()
{
    QPixmap newpix("icons/folder-new.png");
    QPixmap copypix("icons/copy.png");
    QPixmap movepix("icons/move.png");
    QPixmap deletepix("icons/delete.png");
    QPixmap zippix("icons/zip.png");
    QPixmap quitpix("icons/quit.png");
    
    QToolBar *toolbar = addToolBar("main toolbar");
    toolbar->setObjectName("MainToolbar");
    toolbar->addAction(QIcon(newpix), "New Folder");
    
    QAction *copy = toolbar->addAction(QIcon(copypix), "Copy");
    QObject::connect(copy, SIGNAL(triggered()), this, SLOT(copy_cmd()));
    
    toolbar->addAction(QIcon(movepix), "Move");
    toolbar->addAction(QIcon(deletepix), "Delete");
    toolbar->addAction(QIcon(zippix), "Compress");
    toolbar->addSeparator();

    QAction *quit = toolbar->addAction(QIcon(quitpix), "Quit Application");
    connect(quit, &QAction::triggered, qApp, &QApplication::quit);
}

QDir MainWindow::showParentDir(QTableWidget* table, QDir dir)
{
    bool b = dir.cdUp();
    if(! b)
    {
        return dir;
    }
    showDir(table, dir);
    return dir;
}


QDir MainWindow::doubleClickHandler(QTableWidget* table, 
                                    int row, int col, QDir dir)
{
    if(row == 0)
    {       
        QDir newDir = showParentDir(table, dir);
        return newDir;
    }
    
    QFileInfoList list = dir.entryInfoList();
    // skip "." (row - 1)
    // skip ".." (row)
    QFileInfo fileInfo = list.at(row + 1);
    bool b = fileInfo.isDir();
    if(b)
    {
        QString newName = fileInfo.absoluteFilePath();
        QDir newDir = QDir(newName);
        showDir(table, newDir);
        return newDir;
    }
    return dir;
}

void MainWindow::copy_cmd()
{
    statusBar()->showMessage("Copy CMD");
}

std::string MainWindow::exec(std::string cmd) 
{
    char buffer[BUF_SIZE];
    std::string output = "";
    std::cout << cmd << std::endl;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    FILE* f = pipe.get();
    // wait while command starting
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    while (! feof(f))
    {
        size_t rd = fread(buffer, 1, 1, f);
        if(rd > 0)
        {
            if(buffer[0] == 0xa || buffer[0] == 0xd)
            {
                statusBar()->showMessage(QString::fromStdString(output));
                output = "";
            }
            else
            {
                output += buffer[0];
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return output;
}
