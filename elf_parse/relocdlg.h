#ifndef RELOCDLG_H
#define RELOCDLG_H

#include <QDialog>
#include "elf.h"
namespace Ui {
class RelocDlg;
}

class RelocDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RelocDlg(QWidget *parent = 0);
    ~RelocDlg();

    void ParseReloc(QWidget *parent);


    //定义elf_rel
    Elf32_Rel elf_rel;

private:
    Ui::RelocDlg *ui;
};

#endif // RELOCDLG_H
