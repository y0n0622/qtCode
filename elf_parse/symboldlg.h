#ifndef SYMBOLDLG_H
#define SYMBOLDLG_H

#include <QDialog>
#include "elf.h"
namespace Ui {
class SymbolDlg;
}

class SymbolDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SymbolDlg(QWidget *parent = 0);
    ~SymbolDlg();

    //符号表偏移
    int sym_off;
    int sym_size;
    int sym_entsize;
    //串表偏移
    int str_off;

    //定义符号表elf_symbol
    Elf32_Sym elf_symbol;

    void ParseSym(QWidget *parent);

private:
    Ui::SymbolDlg *ui;
};

#endif // SYMBOLDLG_H
