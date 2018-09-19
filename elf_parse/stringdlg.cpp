#include "mainwindow.h"
#include "stringdlg.h"
#include "ui_stringdlg.h"
#include "ui_mainwindow.h"

#include <QProcess>

StringDlg::StringDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StringDlg)
{
    ui->setupUi(this);
    cmdLine = new QProcess;
    connect(cmdLine,&QProcess::readyReadStandardOutput,
            this,&StringDlg::onReadAllData);
    //解析串表
    ParseString(parent);
}

StringDlg::~StringDlg()
{
    delete ui;
}

void StringDlg::ParseString(QWidget *parent)
{
    MainWindow* pmainwindow = (MainWindow*)parent;
    for(int i = 0; i < pmainwindow->qlistSection.size(); i++)
    {
        //获取每个字段名字
        int shstartab_off = (pmainwindow->elf_header.e_shoff
                             + pmainwindow->elf_header.e_shstrndx
                             * pmainwindow->elf_header.e_shentsize) + 0x10;
        int shstartaboff_value = pmainwindow->qlist.at(shstartab_off + 0x3) << 24
                               | pmainwindow->qlist.at(shstartab_off + 0x2) << 16
                               | pmainwindow->qlist.at(shstartab_off + 0x1) << 8
                               | pmainwindow->qlist.at(shstartab_off);
        int sh_name_off = pmainwindow->qlistSection.at(i).sh_name + shstartaboff_value;
        //cout << i <<":"<< hex << sh_name_off << endl;
        char sh_name[24] = {'\0'};
        int off = 0;
        while(pmainwindow->qlist.at(sh_name_off + off) != '\0')
        {
            sh_name[off] = pmainwindow->qlist.at(sh_name_off + off);
            off++;
        }
        if(strcmp(sh_name, ".shstrtab") == 0)
        {
            int shstrtab_off = pmainwindow->qlistSection.at(i).sh_offset;
            //构造执行命令 hexdump.exe /skip:$007075 demo.so

            QString argcCmd1("/skip:$" + QString::number(shstrtab_off, 16));
            QString argcCmd2(pmainwindow->getUIObj()->filePath->text());

            QStringList argcList;
            argcList.append(argcCmd1);
            argcList.append(argcCmd2);
            cmdLine->start("hexdump.exe", argcList);
        }
    }
}

void StringDlg::onReadAllData()
{
    QByteArray cmdoutput;
    cmdoutput = cmdLine->readAllStandardOutput();
    ui->textEditString->setText(QString::fromLatin1(cmdoutput));
}
