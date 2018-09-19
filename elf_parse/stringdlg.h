#ifndef STRINGDLG_H
#define STRINGDLG_H

#include <QDialog>
#include <qprocess.h>

namespace Ui {
class StringDlg;
}

class StringDlg : public QDialog
{
    Q_OBJECT

public:
    explicit StringDlg(QWidget *parent = 0);
    ~StringDlg();


   void ParseString(QWidget *parent);
public slots:
   void onReadAllData();
private:
    Ui::StringDlg *ui;
    QProcess *cmdLine;
};

#endif // STRINGDLG_H
