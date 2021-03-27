
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
#include <QDebug>
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
    initStatusbar();
    
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
    
    showMessage("");
    
    m_active = ActiveTable::NONE;
    
    m_execThread = new ExecThread(this);
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
    
    QString path = getLeftDirPath();
    //qDebug(qPrintable("left:" + path));
    QDir dir = QDir(path);
    saveTableDir(dir, "leftTable");
    path = getRightDirPath();
    //qDebug(qPrintable("right:" + path));
    dir = QDir(path);
    saveTableDir(dir, "rightTable");

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

void MainWindow::showDir(QTableWidget* table, QString path)
{
    QDir dir(path);
    showDir(table, dir);
}

void MainWindow::showDir(QTableWidget* table, QDir dir)
{
    QStringList selectedNames;
    QList<QTableWidgetItem*> listItems = table->selectedItems();
    int items = listItems.count();  
    if(items > 1)
    {
        for(int i = 0; i < items; i ++)
        {
            QTableWidgetItem* itemName = listItems.at(i + ITEM_NAME);
            QTableWidgetItem* itemType = listItems.at(i + ITEM_TYPE); 
            QString name = itemName->text();
            QString type = itemType->text();
            QString s = name; 
            if(type.length() > 0)
            {
                s += "." + type;
            }
            
            selectedNames.append(s);            
            i += ITEMS_COUNT - 1;            
        }
    }
    
    
    table->setSelectionMode(QAbstractItemView::MultiSelection);
    
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
        
        if(selectedNames.contains(fileInfo.fileName()))
        {
            table->selectRow(row);
        }
    }
    
    table->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void MainWindow::showLeftTable()
{
    m_leftTable = new QTableWidget(this);
    showTable(m_leftTable, "leftTable");
    QObject::connect(m_leftTable, SIGNAL(cellClicked(int,int)), 
                    this, SLOT(leftClickHandler(int,int)));
    QObject::connect(m_leftTable, SIGNAL(cellDoubleClicked(int,int)), 
                    this, SLOT(leftDoubleClickHandler(int,int)));
}

void MainWindow::showRightTable()
{
    m_rightTable = new QTableWidget(this);
    showTable(m_rightTable, "rightTable");
    QObject::connect(m_rightTable, SIGNAL(cellClicked(int,int)), 
                    this, SLOT(rightClickHandler(int,int)));
    QObject::connect(m_rightTable, SIGNAL(cellDoubleClicked(int,int)), 
            this, SLOT(rightDoubleClickHandler(int,int)));
}

void MainWindow::showTable(QTableWidget* table, QString key)
{
    QStringList header;
    header<<""<<"name"<<"type"<<"size"<<"owner"<<"modified";
    
    table->setRowCount(1); 
    table->setColumnCount(ITEMS_COUNT);
    table->setHorizontalHeaderLabels(header);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    restoreTableSettings(table, key);
}


void MainWindow::saveTableSettings(QTableWidget* table, QString key)
{
    int w = 0;
    QSettings settings(DOMAIN_NAME, APPLICATION_NAME);
    for(int i = 0; i < ITEMS_COUNT; i ++)
    {
        w = table->columnWidth(i);
        settings.setValue(key + "/column" + QString::number(i), w);
    }        
}

void MainWindow::saveTableDir(QDir dir, QString key)
{
    QSettings settings(DOMAIN_NAME, APPLICATION_NAME);
    settings.setValue(key + "/dir", dir.absolutePath());       
}

QString MainWindow::getTableDir(QString key)
{
    QString path = "";
    QSettings settings(DOMAIN_NAME, APPLICATION_NAME);
    QVariant val = settings.value(key + "/dir", "");
    if (!val.isNull() && val.isValid())
    {
        path = val.toString();
    }
    return path;
}

void MainWindow::restoreTableSettings(QTableWidget* table, QString key)
{
    QSettings settings(DOMAIN_NAME, APPLICATION_NAME);
    for(int i = 0; i < ITEMS_COUNT; i ++)
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
    Q_UNUSED(row);
    Q_UNUSED(col);
    m_active = ActiveTable::LEFT;
    clearSelection(m_rightTable);
    QString path = getLeftDirPath();
    showMessage(path);
}

void MainWindow::rightClickHandler(int row, int col)
{
    Q_UNUSED(row);
    Q_UNUSED(col);
    m_active = ActiveTable::RIGHT;    
    clearSelection(m_leftTable);
    QString path = getRightDirPath();
    showMessage(path);
}

void MainWindow::leftDoubleClickHandler(int row, int col)
{
    m_active = ActiveTable::LEFT;
    QString path = getLeftDirPath();
    QDir dir = doubleClickHandler(m_leftTable, row, col, path);
    path = dir.absolutePath();
    setLeftDirPath(path);
    showMessage(path);
}

void MainWindow::rightDoubleClickHandler(int row, int col)
{
    m_active = ActiveTable::RIGHT;
    QString path = getRightDirPath();
    QDir dir = doubleClickHandler(m_rightTable, row, col, path);
    path = dir.absolutePath();
    setRightDirPath(path);
    showMessage(path);
}

void MainWindow::readSettings()
{
    QString path = getTableDir("leftTable");
    setLeftDirPath(path);
    
    path = getTableDir("rightTable");
    setRightDirPath(path);
}

void MainWindow::initMenu()
{
    menuBar()->addMenu("&File");
    menuBar()->addMenu("&Edit");
    menuBar()->addMenu("&View");
    menuBar()->addMenu("&Simulate");
    menuBar()->addMenu("&Help");
}

void MainWindow::initStatusbar()
{
    QStatusBar* sb = statusBar();
    m_labelStatus1 = new QLabel(sb);
    sb->addWidget(m_labelStatus1);
    m_labelStatus2 = new QLabel(sb);
    sb->addWidget(m_labelStatus2);
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
    
    QAction *newFolderAction = toolbar->addAction(QIcon(newpix), "New Folder");
    QObject::connect(newFolderAction, SIGNAL(triggered()), this, SLOT(cmdNewFolder()));
    
    QAction *copyAction = toolbar->addAction(QIcon(copypix), "Copy");
    QObject::connect(copyAction, SIGNAL(triggered()), this, SLOT(cmdCopy()));
    
    toolbar->addAction(QIcon(movepix), "Move");
    
    QAction *deleteAction = toolbar->addAction(QIcon(deletepix), "Delete");
    QObject::connect(deleteAction, SIGNAL(triggered()), this, SLOT(cmdDelete()));
    
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
                                    int row, int col, QString path)
{
    QDir dir(path);
    return doubleClickHandler(table, row, col, dir);
}

QDir MainWindow::doubleClickHandler(QTableWidget* table, 
                                    int row, int col, QDir dir)
{
    Q_UNUSED(col);
    
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
        showDir(table, newName);
        return newDir;
    }
    
    return dir;
}

QString MainWindow::getSrcPath()
{
    QString path = "";
    switch(m_active)
    {
        case ActiveTable::LEFT:
            path = getLeftDirPath();
            break;
        case ActiveTable::RIGHT:
            path = getRightDirPath();
            break;
        default:
            break;;
    }
    return path;
}

QString MainWindow::getDstPath()
{
    QString path = "";
    switch(m_active)
    {
        case ActiveTable::RIGHT:
            path = getLeftDirPath();
            break;
        case ActiveTable::LEFT:
            path = getRightDirPath();
            break;
        default:
            break;
    }
    return path;
}

QTableWidget* MainWindow::getDstTable()
{
    QTableWidget* table = nullptr;
    switch(m_active)
    {
        case ActiveTable::RIGHT:
            table = m_leftTable;
            break;
        case ActiveTable::LEFT:
            table = m_rightTable;
            break;
        default:
            break;;
    }
    return table;
}

QTableWidget* MainWindow::getSrcTable()
{
    QTableWidget* table = nullptr;
    switch(m_active)
    {
        case ActiveTable::LEFT:
            table = m_leftTable;
            break;
        case ActiveTable::RIGHT:
            table = m_rightTable;
            break;
        default:
            break;;
    }
    return table;
}

void MainWindow::cmdCopy()
{
    QTableWidget* tableSrc = getSrcTable();
    QTableWidget* tableDst = getDstTable();
    QString pathSrc = getSrcPath();
    QDir dirSrc(pathSrc);
    
    QString pathDst = getDstPath();
    QDir dirDst(pathDst);
    
    QFileInfoList files = dirSrc.entryInfoList();    
    
    int cols = tableSrc->columnCount();
    QList<QTableWidgetItem*> list = tableSrc->selectedItems();
    int items = list.count();    
    for(int i = 0; i < items; i ++)
    {
        QTableWidgetItem* item = list.at(i);
        int row = item->row();
        // skip "." (row - 1)
        // skip ".." (row)
        QFileInfo fileInfo = files.at(row + 1);
        QString path = fileInfo.absoluteFilePath();
        QString cmd = QString(CMP_COPY).arg(path).arg(pathDst);
        m_execThread->init(cmd);
        m_execThread->start();
        i += cols;
    }
    showDir(tableDst, pathDst);
}

void MainWindow::cmdNewFolder(void)
{
    QTableWidget* table = getSrcTable();
    QString pathSrc = getSrcPath();
    QDir dirSrc(pathSrc);
    dirSrc.mkdir("New Folder");
    showDir(table, dirSrc);
}

void MainWindow::cmdDelete(void)
{
    QTableWidget* table = getSrcTable();
    QString pathSrc = getSrcPath();
    QDir dirSrc(pathSrc);
    QFileInfoList files = dirSrc.entryInfoList();
    int cols = table->columnCount();
    QList<QTableWidgetItem*> list = table->selectedItems();
    int items = list.count();    
    QStringList selected;
    for(int i = 0; i < items; i ++)
    {
        QTableWidgetItem* item = list.at(i);
        int row = item->row();
        // skip "." (row - 1)
        // skip ".." (row)
        QFileInfo fileInfo = files.at(row + 1);
        QString path = fileInfo.absoluteFilePath();
        selected.append(path); 
        i += cols - 1;
    }
    
    for(QString path: selected)
    {
        QString cmd = QString(CMP_DELETE).arg(path);
        
        m_execThread->init(cmd);
        m_execThread->start();
    }
}

void MainWindow::clearSelection(QTableWidget* table)
{
    QList<QTableWidgetSelectionRange> list = table->selectedRanges();
    QTableWidgetSelectionRange range;
    foreach(range, list)
    {
        table->setRangeSelected(range, false);
    }
}

void MainWindow::showMessage1(QString s)
{
    m_labelStatus1->setText(s);
}

void MainWindow::showMessage2(QString s)
{
    m_labelStatus2->setText(s);
}

void MainWindow::showMessage(QString s)
{
    showMessage1(s);
}

void MainWindow::showMessage(QString s1, QString s2)
{
    showMessage1(s1);
    showMessage2(s2);
    showDirs();
}

void MainWindow::cmdCompleted()
{
    showDirs();
    showMessage("", "");
}

void MainWindow::showDirs()
{
    QString path = getLeftDirPath();
    showDir(m_leftTable, path);
    path = getRightDirPath();
    showDir(m_rightTable, path);
}

QString MainWindow::getLeftDirPath()
{
    m_leftMutex.lock();
    QString path = m_leftDir.absolutePath();
    m_leftMutex.unlock();
    return path;
}

void MainWindow::setLeftDirPath(QString path)
{
    m_leftMutex.lock();
    m_leftDir = QDir(path);
    m_leftMutex.unlock();
}

QString MainWindow::getRightDirPath()
{
    m_rightMutex.lock();
    QString path = m_rightDir.absolutePath();
    m_rightMutex.unlock();
    return path;
}

void MainWindow::setRightDirPath(QString path)
{
    m_rightMutex.lock();
    m_rightDir = QDir(path);
    m_rightMutex.unlock();
}

