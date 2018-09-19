#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDragEnterEvent>

#include <QMainWindow>
#include "elf.h"
#include "symboldlg.h"
#include "relocdlg.h"

//#define MAX_NUM 128

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    //字节链表，用来保存从文件中读取的字节数据
    QList<uchar> qlist;
    //定义elf_header
    Elf32_Ehdr elf_header;
    //定义elf_section
    Elf32_Shdr elf_section;
    QList<Elf32_Shdr> qlistSection;
    //定义elf_program
    Elf32_Phdr elf_program;


    Ui::MainWindow *getUIObj();
    //解析文件头
    void parseElfHeader();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void on_analyzeElf_clicked();

    void on_filePath_textEdited(const QString &arg1);

    void on_pBtnSection_clicked();

    void on_pBtnProgram_clicked();

    void on_filePath_textChanged(const QString &arg1);

    void on_pBtnSymbol_clicked();

    void on_pBtnReloc_clicked();

    void on_pBtnString_clicked();



private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
