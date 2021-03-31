/****************************************************************************
 *
 * This file is part of UseeU.
 *
 ****************************************************************************/

#include "FileNameDialog.h"

FileNameDialog::FileNameDialog(QWidget* dlg)
    : QDialog(dlg, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
    m_label = new QLabel("New name", this);    
    m_text = new QLineEdit(this);
    
    m_butOk = new QPushButton("&OK", this);
    m_butCancel = new QPushButton("&Cancel", this);
    
    QObject::connect(m_butOk, SIGNAL(clicked()), SLOT(accept()));
    QObject::connect(m_butCancel, SIGNAL(clicked()), SLOT(reject()));
    
    m_layout = new QGridLayout(this);
    
    m_layout->addWidget(m_label, 0, 0);
    m_layout->addWidget(m_text, 0, 1);

    m_layout->addWidget(m_butOk, 1, 0);
    m_layout->addWidget(m_butCancel, 1, 1);
    
    setWindowTitle("File name");
    setLayout(m_layout);    
}

void FileNameDialog::init(QString name)
{
    m_text->setText(name);
}

QString FileNameDialog::getName()
{
    QString cmd = m_text->text();    
    return cmd;    
}
