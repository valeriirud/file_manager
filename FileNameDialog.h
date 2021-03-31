/****************************************************************************
 *
 * This file is part of file manager.
 *
 ****************************************************************************/

#ifndef FILENAMEDIALOG_H
#define FILENAMEDIALOG_H
#include <QDialog>
#include <QtCore/QMap>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

class FileNameDialog : public QDialog
{
    Q_OBJECT

public:
    FileNameDialog(QWidget* dlg = 0);
    void init(QString);
    QString getName();
private:
    QLabel* m_label;
    QLineEdit* m_text;
    QPushButton* m_butOk;
    QPushButton* m_butCancel;
    QGridLayout* m_layout;
};
#endif // FILENAMEDIALOG_H

