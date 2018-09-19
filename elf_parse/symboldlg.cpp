#include "mainwindow.h"
#include "symboldlg.h"
#include "ui_symboldlg.h"
#include <QMessageBox>

SymbolDlg::SymbolDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SymbolDlg)
{
    ui->setupUi(this);

    //解析symbol
    ParseSym(parent);
}

SymbolDlg::~SymbolDlg()
{
    delete ui;
}

void SymbolDlg::ParseSym(QWidget *parent)
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
        //判断是否是.dynsym表
        if(strcmp(sh_name, ".strtab") == 0)
        {
            //如果字符表内容在.strtab中暂时不解析
            QMessageBox::information(this,
                                     "tips",
                                     ".strtab暂时不解析",
                                     QMessageBox::StandardButton::Ok);
            return;
        }
        else if(strcmp(sh_name, ".dynstr") == 0)
        {
            str_off = pmainwindow->qlistSection.at(i).sh_offset;
        }
        else if(strcmp(sh_name, ".dynsym") == 0)
        {
            sym_off = pmainwindow->qlistSection.at(i).sh_offset;
            sym_size = pmainwindow->qlistSection.at(i).sh_size;
            sym_entsize = pmainwindow->qlistSection.at(i).sh_entsize;
        }
        else
        {

        }
    }
    //设置列表行数
    ui->tableSymbol->setRowCount(sym_size/sym_entsize);
    //根据获得的偏移进行解析
    for(int j = 0; j < sym_size/sym_entsize; j++)
    {
        //解析名称
        int sym_name_off = pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0x3) << 24
                         | pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0x2) << 16
                         | pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0x1) << 8
                         | pmainwindow->qlist.at(sym_off + j * sym_entsize);
        int sym_name_str_off = str_off + sym_name_off;
        char sym_name[24] = {'\0'};
        int off = 0;
        while(pmainwindow->qlist.at(sym_name_str_off + off) != '\0')
        {
            sym_name[off] = pmainwindow->qlist.at(sym_name_str_off + off);
            off++;
        }
        //将结果设置到界面
        QString str_index;
        str_index.setNum(j);
        ui->tableSymbol->setItem(j, 0, new QTableWidgetItem(str_index));
        ui->tableSymbol->setItem(j, 1, new QTableWidgetItem(sym_name));

        //解析符号值
        int sym_value = pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0x7) << 24
                      | pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0x6) << 16
                      | pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0x5) << 8
                      | pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0x4);
        ui->tableSymbol->setItem(j, 2, new QTableWidgetItem("0x" + QString::number(sym_value, 16)));

        //解析符号大小
        int sym_stsize = pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0xb) << 24
                       | pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0xa) << 16
                       | pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0x9) << 8
                       | pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0x8);
        ui->tableSymbol->setItem(j, 3, new QTableWidgetItem(QString::number(sym_stsize, 16) + "h"));

        //解析符号类型
        uchar sym_stinfo = pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0xc);
        switch (ELF32_ST_TYPE(sym_stinfo)) {
        case STT_NOTYPE:
            ui->tableSymbol->setItem(j, 4, new QTableWidgetItem("STT_NOTYPE(NOTYPE)"));
            break;
        case STT_OBJECT:
            ui->tableSymbol->setItem(j, 4, new QTableWidgetItem("STT_OBJECT(OBJECT)"));
            break;
        case STT_FUNC:
            ui->tableSymbol->setItem(j, 4, new QTableWidgetItem("STT_FUNC(FUNC)"));
            break;
        case STT_SECTION:
            ui->tableSymbol->setItem(j, 4, new QTableWidgetItem("STT_SECTION(SECTION)"));
            break;
        case STT_FILE:
            ui->tableSymbol->setItem(j, 4, new QTableWidgetItem("STT_FILE(FILE)"));
            break;
        case STT_COMMON:
            ui->tableSymbol->setItem(j, 4, new QTableWidgetItem("STT_COMMON(COMMON)"));
            break;
        case STT_TLS:
            ui->tableSymbol->setItem(j, 4, new QTableWidgetItem("STT_TLS(TLS)"));
            break;
        case STT_NUM:
            ui->tableSymbol->setItem(j, 4, new QTableWidgetItem("STT_NUM(NUM)"));
            break;
        default:
            ui->tableSymbol->setItem(j, 4, new QTableWidgetItem("low:0x" + QString::number(ELF32_ST_TYPE(sym_stinfo), 16)));
            break;
        }

        //绑定
        switch (ELF32_ST_BIND(sym_stinfo)) {
        case STB_LOCAL:
            ui->tableSymbol->setItem(j, 5, new QTableWidgetItem("STB_LOCAL(LOCAL)"));
            break;
        case STB_GLOBAL:
            ui->tableSymbol->setItem(j, 5, new QTableWidgetItem("STB_GLOBAL(GLOBAL)"));
            break;
        case STB_WEAK:
            ui->tableSymbol->setItem(j, 5, new QTableWidgetItem("STB_WEAK(WEAK)"));
            break;
        case STB_NUM:
            ui->tableSymbol->setItem(j, 5, new QTableWidgetItem("STB_NUM(NUM)"));
            break;
        default:
            ui->tableSymbol->setItem(j, 5, new QTableWidgetItem("high:0x" + QString::number(ELF32_ST_BIND(sym_stinfo), 16)));
            break;
        }

        //显示
        uchar sym_stother = pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0xd);
        switch (sym_stother) {
        case STV_DEFAULT:
            ui->tableSymbol->setItem(j, 6, new QTableWidgetItem("STV_DEFAULT(DEFAULT)"));
            break;
        case STV_INTERNAL:
            ui->tableSymbol->setItem(j, 6, new QTableWidgetItem("STV_INTERNAL(INTERNAL)"));
            break;
        case STV_HIDDEN:
            ui->tableSymbol->setItem(j, 6, new QTableWidgetItem("STV_HIDDEN(HIDDEN)"));
            break;
        case STV_PROTECTED:
            ui->tableSymbol->setItem(j, 6, new QTableWidgetItem("STV_PROTECTED(PROTECTED)"));
            break;
        default:
            ui->tableSymbol->setItem(j, 6, new QTableWidgetItem(QString::number(sym_stother, 16)));
            break;
        }

        //表项在段表中的索引
        ushort sym_stshndx = pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0xf) << 8
                           | pmainwindow->qlist.at((sym_off + j * sym_entsize) + 0xe);
        switch (sym_stshndx) {
        case SHN_UNDEF:
            ui->tableSymbol->setItem(j, 7, new QTableWidgetItem("SHN_UNDEF(UNDEF)"));
            break;
        case SHN_ABS:
            ui->tableSymbol->setItem(j, 7, new QTableWidgetItem("SHN_ABS(ABS)"));
            break;
        case SHN_COMMON:
            ui->tableSymbol->setItem(j, 7, new QTableWidgetItem("SHN_COMMON(COMMON)符号指表示对齐属性"));
            break;
        default:
            ui->tableSymbol->setItem(j, 7, new QTableWidgetItem("0x" + QString::number(sym_stshndx, 16)));
            break;
        }

    }

}
