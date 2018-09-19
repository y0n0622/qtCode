#include "mainwindow.h"
#include "stringdlg.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <qevent.h>
#include <qmimedata.h>
#include <qfile.h>
#include <QDebug>
#include <iostream>

using namespace std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->analyzeElf->setEnabled(false);
    ui->pBtnSection->setEnabled(false);
    ui->pBtnProgram->setEnabled(false);
    ui->pBtnSymbol->setEnabled(false);
    ui->pBtnReloc->setEnabled(false);
    ui->pBtnString->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::parseElfHeader()
{
    //为elf文件则进行头部数据解析
    //并将解析的结果设置到界面中的控件中
    //解析magic
    ui->showMagic->setText(ELFMAG);
    //解析文件类型32/64
    if(elf_header.e_ident[4] == ELFCLASS32)
    {
        ui->showClass->setText("ELF32");
    }
    else if(elf_header.e_ident[4] == ELFCLASSNONE)
    {
        ui->showClass->setText("未知！");
    }
    else if(elf_header.e_ident[4] == ELFCLASS64)
    {
        ui->showClass->setText("ELF64");
    }
    else{
        ui->showClass->setText("ELFCLASSNUM");
    }
    //解析字节序
    if(elf_header.e_ident[5] == ELFDATA2LSB)
    {
        ui->showData->setText("小端");
        //解析程序入口
        elf_header.e_entry = qlist.at(0x1b) << 24
                           | qlist.at(0x1a) << 16
                           | qlist.at(0x19) << 8
                           | qlist.at(0x18);

        ui->showEntry->setText("0x" + QString::number(elf_header.e_entry, 16));
        //解析程序头偏移
        elf_header.e_phoff = qlist.at(0x1f) << 24
                           | qlist.at(0x1e) << 16
                           | qlist.at(0x1d) << 8
                           | qlist.at(0x1c);

        ui->showPhoff->setText("0x" + QString::number(elf_header.e_phoff, 16));
        //解析段表偏移
        elf_header.e_shoff = qlist.at(0x23) << 24
                           | qlist.at(0x22) << 16
                           | qlist.at(0x21) << 8
                           | qlist.at(0x20);

        ui->showShoff->setText("0x" + QString::number(elf_header.e_shoff, 16));
        //解析elf_header的大小
        elf_header.e_ehsize = qlist.at(0x29) << 8 | qlist.at(0x28);
        ui->showEhsize->setText("0x" + QString::number(elf_header.e_ehsize, 16));
        //解析程序头的大小
        elf_header.e_phentsize = qlist.at(0x2b) << 8 | qlist.at(0x2a);
        ui->showPhsize->setText("0x" + QString::number(elf_header.e_phentsize, 16));
        //解析程序头的个数
        elf_header.e_phnum = qlist.at(0x2d) << 8 | qlist.at(0x2c);
        ui->showPhnum->setText("0x" + QString::number(elf_header.e_phnum, 16));
        //解析段表的大小
        elf_header.e_shentsize = qlist.at(0x2f) << 8 | qlist.at(0x2e);
        ui->showShsize->setText("0x" + QString::number(elf_header.e_shentsize, 16));
        //解析段表的个数
        elf_header.e_shnum = qlist.at(0x31) << 8 | qlist.at(0x30);
        ui->showShnum->setText("0x" + QString::number(elf_header.e_shnum, 16));
        //解析段表-字符串表索引
        elf_header.e_shstrndx = qlist.at(0x33) << 8 | qlist.at(0x32);
        ui->showShstrndex->setText("0x" + QString::number(elf_header.e_shstrndx, 16));
    }
    else if(elf_header.e_ident[5] == ELFDATA2MSB)
    {
        ui->showData->setText("大端");
    }
    else
    {
        ui->showData->setText("未知!");
    }
    //解析类型
    elf_header.e_type = qlist.at(0x11) << 8 | qlist.at(0x10);
    if(elf_header.e_type == ET_REL)
    {
        ui->showType->setText("可重定位文件");
    }
    else if(elf_header.e_type == ET_EXEC)
    {
        ui->showType->setText("可执行文件");
    }
    else if(elf_header.e_type == ET_DYN)
    {
        ui->showType->setText("so文件");
    }
    else if(elf_header.e_type == ET_CORE)
    {
        ui->showType->setText("core文件");
    }
    else
    {
        ui->showType->setText("未知!");
    }
    //解析cpu平台
    elf_header.e_machine = qlist.at(0x13) << 8 | qlist.at(0x12);
    if(elf_header.e_machine == EM_386)
    {
        ui->showMachine->setText("Intel 80386");
    }
    else if(elf_header.e_machine == EM_ARM)
    {
        ui->showMachine->setText("ARM");
    }
    else if(elf_header.e_machine == EM_X86_64)
    {
        ui->showMachine->setText("AMD x86-64 architecture");
    }
    else
    {
        ui->showMachine->setText("其他未知平台!");
    }
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    //如果是文件，则支持拖放
    if(event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    //ui->filePath->setText("123");
    //获取文件路径显示到filePath中
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
    {
        ui->filePath->setText("获取路径失败");
    }
    else
    {
        QString file_name = urls.at(0).toLocalFile();
        ui->filePath->setText(file_name);
        ui->analyzeElf->setEnabled(true);
    }
}

void MainWindow::on_analyzeElf_clicked()
{
    //进行解析
    //获取路径
    QString strPath = ui->filePath->text();
    //读取出文件中的所有字节
    //并将数据保存在qlist中
    QFile file(strPath);
    if ( file.open( QFile::ReadOnly ) )
    {
       QDataStream stream( &file );
       uchar filebyte = '\0';
       int n = 0;
       while ( !stream.atEnd() ) {
           stream >> filebyte;
           qlist.append(filebyte);
           n++;
       }
       file.close();
    }
    //通过判断魔术，检测是否为elf文件
    for(int i = 0; i < EI_NIDENT; i++)
    {
        //并将头部解析数据保存在结构中
        elf_header.e_ident[i] = qlist.at(i);
    }
    if(elf_header.e_ident[0] == ELFMAG0 &&
            elf_header.e_ident[1] == ELFMAG1 &&
            elf_header.e_ident[2] == ELFMAG2 &&
            elf_header.e_ident[3] == ELFMAG3)
    {
        //解析elf_header
        parseElfHeader();
    }
    else
    {
        QMessageBox::information(this,
                                 "tips",
                                 "不是elf文件，解析失败",
                                 QMessageBox::StandardButton::Ok);
    }

    //设置段表和程序表可以解析
    ui->pBtnSection->setEnabled(true);
    ui->pBtnProgram->setEnabled(true);
}

void MainWindow::on_filePath_textChanged(const QString &arg1)
{
    if(!arg1.isEmpty())
    {
        ui->analyzeElf->setEnabled(true);
    }
    else
    {
        ui->analyzeElf->setEnabled(false);
    }
    //如果输入文件路径改变按钮失效
    ui->pBtnSection->setEnabled(false);
    ui->pBtnProgram->setEnabled(false);
    ui->pBtnSymbol->setEnabled(false);
    ui->pBtnReloc->setEnabled(false);
    ui->pBtnString->setEnabled(false);
}

void MainWindow::on_filePath_textEdited(const QString &arg1)
{
    if(!arg1.isEmpty())
    {
        ui->analyzeElf->setEnabled(true);
    }
    else
    {
        ui->analyzeElf->setEnabled(false);
    }
    //如果输入文件路径被修该按钮失效
    ui->pBtnSection->setEnabled(false);
    ui->pBtnProgram->setEnabled(false);
    ui->pBtnSymbol->setEnabled(false);
    ui->pBtnReloc->setEnabled(false);
    ui->pBtnString->setEnabled(false);
}

//解析段表
void MainWindow::on_pBtnSection_clicked()
{
    //设置列表行数
    ui->tableSection->setRowCount(elf_header.e_shnum);
    //循环解析所有段表
    for(int i = 0; i < elf_header.e_shnum; i++)
    {
        //解析一个段表
        //解析名称
        elf_section.sh_name = qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x3) << 24
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x2) << 16
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x1) << 8
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize));
        //类型
        elf_section.sh_type = qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x7) << 24
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x6) << 16
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x5) << 8
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x4);
        //标志
        elf_section.sh_flags = qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0xb) << 24
                             | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0xa) << 16
                             | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x9) << 8
                             | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x8);
        //虚拟地址
        elf_section.sh_addr = qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0xf) << 24
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0xe) << 16
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0xd) << 8
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0xc);
        //文件偏移
        elf_section.sh_offset = qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x13) << 24
                              | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x12) << 16
                              | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x11) << 8
                              | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x10);
        //段大小
        elf_section.sh_size = qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x17) << 24
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x16) << 16
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x15) << 8
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x14);
        //link
        elf_section.sh_link = qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x1b) << 24
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x1a) << 16
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x19) << 8
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x18);
        //info
        elf_section.sh_info = qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x1f) << 24
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x1e) << 16
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x1d) << 8
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x1c);
        //对齐
        elf_section.sh_addralign = qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x23) << 24
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x22) << 16
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x21) << 8
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x20);
        //表项大小
        elf_section.sh_entsize = qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x27) << 24
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x26) << 16
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x25) << 8
                            | qlist.at((elf_header.e_shoff + i * elf_header.e_shentsize) + 0x24);

        //根据当前的偏移或索引获取对应的值，
        //然后分别解析每个字段，
        //并将结果设置到界面
        /*sh_name，获取elf_header中shstrtab的索引，计算shstrtab在文件中的偏移*/
        int shstartab_off = (elf_header.e_shoff
                             + elf_header.e_shstrndx
                             * elf_header.e_shentsize) + 0x10;
        int shstartaboff_value = qlist.at(shstartab_off + 0x3) << 24
                               | qlist.at(shstartab_off + 0x2) << 16
                               | qlist.at(shstartab_off + 0x1) << 8
                               | qlist.at(shstartab_off);
        int sh_name_off = elf_section.sh_name + shstartaboff_value;
        //cout << i <<":"<< hex << sh_name_off << endl;
        char sh_name[24] = {'\0'};
        int off = 0;
        while(qlist.at(sh_name_off + off) != '\0')
        {
            sh_name[off] = qlist.at(sh_name_off + off);
            off++;
        }
        //将字段结果循环设置到界面
        //序号
        QString str_index;
        str_index.setNum(i);
        ui->tableSection->setItem(i, 0, new QTableWidgetItem(str_index));
        QString str_name(sh_name);
        ui->tableSection->setItem(i, 1, new QTableWidgetItem(str_name));

        /*类型*/
        switch(elf_section.sh_type)
        {
        case SHT_NULL:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_NULL(未使用)"));
            break;
        case SHT_PROGBITS:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_PROGBITS(程序数据)"));
            break;
        case SHT_SYMTAB:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_SYMTAB(符号表)"));
            break;
        case SHT_STRTAB:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_STRTAB(字符串表)"));
            break;
        case SHT_RELA:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_RELA(重定位表带地址)"));
            break;
        case SHT_HASH:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_HASH(哈希表)"));
            break;
        case SHT_DYNAMIC:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_DYNAMIC(动态连接信息)"));
            break;
        case SHT_NOTE:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_NOTE(注释)"));
            break;
        case SHT_NOBITS:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_NOBITS(bss)"));
            break;
        case SHT_REL:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_REL(重定位表无地址)"));
            break;
        case SHT_SHLIB:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_SHLIB(Reserved)"));
            break;
        case SHT_DYNSYM:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_DYNSYM(动态连接符号表)"));
            break;
        case SHT_INIT_ARRAY:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_INIT_ARRAY(构造)"));
            break;
        case SHT_FINI_ARRAY:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_FINI_ARRAY(析构)"));
            break;
        case SHT_PREINIT_ARRAY:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_PREINIT_ARRAY(预构造)"));
            break;
        case SHT_GROUP:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_GROUP(Section group)"));
            break;
        case SHT_SYMTAB_SHNDX:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_SYMTAB_SHNDX(Extended section indeces)"));
            break;
        case SHT_NUM:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_NUM(Number of defined types)"));
            break;
        case SHT_LOOS:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_LOOS(Start OS-specific)"));
            break;
        case SHT_HIOS:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_HIOS(End OS-specific type)"));
            break;
        case SHT_LOPROC:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_LOPROC(Start of processor-specific)"));
            break;
        case SHT_HIPROC:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("SHT_HIPROC(End of processor-specific)"));
            break;
        default:
            ui->tableSection->setItem(i, 2, new QTableWidgetItem("0x" + QString::number(elf_section.sh_type, 16)));
            break;
        }

        /*标志*/
        switch(elf_section.sh_flags)
        {
        case SHF_WRITE:
            ui->tableSection->setItem(i, 3, new QTableWidgetItem("SHF_WRITE(W)"));
            break;
        case SHF_ALLOC:
            ui->tableSection->setItem(i, 3, new QTableWidgetItem("SHF_ALLOC(A)"));
            break;
        case SHF_EXECINSTR:
            ui->tableSection->setItem(i, 3, new QTableWidgetItem("SHF_EXECINSTR(X)"));
            break;
        case SHF_WRITE | SHF_ALLOC:
            ui->tableSection->setItem(i, 3, new QTableWidgetItem("SHF_WRITE_ALLOC(WA)"));
            break;
        case SHF_WRITE | SHF_EXECINSTR:
            ui->tableSection->setItem(i, 3, new QTableWidgetItem("SHF_WRITE_EXECINSTR(WE)"));
            break;
        case SHF_ALLOC | SHF_EXECINSTR:
            ui->tableSection->setItem(i, 3, new QTableWidgetItem("SHF_ALLOC_EXECINSTR(AE)"));
            break;
        case SHF_ALLOC | SHF_EXECINSTR | SHF_WRITE:
            ui->tableSection->setItem(i, 3, new QTableWidgetItem("SHF_WRITE_ALLOC_EXECINSTR(WAE)"));
            break;
        case SHF_MERGE:
            ui->tableSection->setItem(i, 3, new QTableWidgetItem("SHF_MERGE(M)"));
            break;
        case SHF_STRINGS:
            ui->tableSection->setItem(i, 3, new QTableWidgetItem("SHF_STRINGS(S)"));
            break;
        case SHF_INFO_LINK:
            ui->tableSection->setItem(i, 3, new QTableWidgetItem("SHF_INFO_LINK(L)"));
            break;
        default:
            ui->tableSection->setItem(i, 3, new QTableWidgetItem("0x" + QString::number(elf_section.sh_flags, 16)));
            break;
        }

        /*虚拟地址*/
        ui->tableSection->setItem(i, 4, new QTableWidgetItem("0x" + QString::number(elf_section.sh_addr, 16)));

        /*文件偏移*/
        ui->tableSection->setItem(i, 5, new QTableWidgetItem("0x" + QString::number(elf_section.sh_offset, 16)));

        /*段大小*/
        ui->tableSection->setItem(i, 6, new QTableWidgetItem("0x" + QString::number(elf_section.sh_size, 16)));

        /*link,对于不同的段具有不同的含义*/
        //SHT_DYNAMTC:
        ui->tableSection->setItem(i, 7, new QTableWidgetItem(QString::number(elf_section.sh_link)));

        /*info对于不同的段具有不同的含义*/
        ui->tableSection->setItem(i, 8, new QTableWidgetItem(QString::number(elf_section.sh_info)));

        /*对齐，对齐4，对齐16，对齐32， 0， 1无要求*/
        ui->tableSection->setItem(i, 9, new QTableWidgetItem(QString::number(elf_section.sh_addralign)));

        /*表项大小*/
        ui->tableSection->setItem(i, 10, new QTableWidgetItem(QString::number(elf_section.sh_entsize)));

        //将该段信息保存到qlistSection中
        qlistSection.append(elf_section);
    }
    ui->pBtnSymbol->setEnabled(true);
    ui->pBtnReloc->setEnabled(true);
    ui->pBtnString->setEnabled(true);
}

//解析程序头表
void MainWindow::on_pBtnProgram_clicked()
{
    //设置列表行数
    ui->tableProgram->setRowCount(elf_header.e_phnum);
    //循环解析
    for(int i = 0; i < elf_header.e_phnum; i++)
    {
        //类型
        elf_program.p_type = qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x3) << 24
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x2) << 16
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x1) << 8
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize));
        //文件偏移
        elf_program.p_offset = qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x7) << 24
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x6) << 16
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x5) << 8
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x4);
        //虚拟地址
        elf_program.p_vaddr = qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0xb) << 24
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0xa) << 16
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x9) << 8
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x8);
        //物理地址
        elf_program.p_paddr = qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0xf) << 24
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0xe) << 16
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0xd) << 8
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0xc);
        //在文件中的大小
        elf_program.p_filesz = qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x13) << 24
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x12) << 16
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x11) << 8
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x10);
        //在内存中的大小
        elf_program.p_memsz = qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x17) << 24
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x16) << 16
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x15) << 8
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x14);
        //标志
        elf_program.p_flags = qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x1b) << 24
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x1a) << 16
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x19) << 8
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x18);
        //对齐
        elf_program.p_align = qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x1f) << 24
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x1e) << 16
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x1d) << 8
                            | qlist.at((elf_header.e_phoff + i * elf_header.e_phentsize) + 0x1c);
        //序号
        QString str_index;
        str_index.setNum(i);
        ui->tableProgram->setItem(i, 0, new QTableWidgetItem(str_index));

        /*类型*/
        switch (elf_program.p_type) {
        case PT_NULL:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("未使用"));
            break;
        case PT_LOAD:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("PT_LOAD(加载Program Segment)"));
            break;
        case PT_DYNAMIC:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("PT_DYNAMIC(动态连接信息)"));
            break;
        case PT_INTERP:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("PT_INTERP(Program interpreter)"));
            break;
        case PT_NOTE:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("PT_NOTE(Auxiliary information)"));
            break;
        case PT_SHLIB:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("PT_SHLIB(Reserved)"));
            break;
        case PT_PHDR:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("PT_PHDR(Entry for header table itself)"));
            break;
        case PT_TLS:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("PT_TLS(Thread-local storage segment)"));
            break;
        case PT_NUM:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("PT_NUM(Number of defined types)"));
            break;
        case PT_LOOS:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("PT_LOOS(Start of OS-specific)"));
            break;
        case PT_GNU_EH_FRAME:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("PT_GNU_EH_FRAME(GCC .eh_frame_hdr segment)"));
            break;
        case PT_GNU_STACK:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("PT_GNU_STACK(Indicates stack executability)"));
            break;
        case PT_GNU_RELRO:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("PT_GNU_RELRO(Start of OS-specific)"));
            break;
        case PT_LOSUNW:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("PT_LOSUNW(Sun Specific segment)"));
            break;
        case PT_SUNWSTACK:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("PT_SUNWSTACK(Stack segment)"));
            break;
        default:
            ui->tableProgram->setItem(i, 1, new QTableWidgetItem("0x" + QString::number(elf_program.p_type, 16)));
            break;
        }

        /*文件偏移*/
        ui->tableProgram->setItem(i, 2, new QTableWidgetItem("0x" + QString::number(elf_program.p_offset, 16)));

        /*虚拟地址*/
        ui->tableProgram->setItem(i, 3, new QTableWidgetItem("0x" + QString::number(elf_program.p_vaddr, 16)));

        /*物理地址*/
        ui->tableProgram->setItem(i, 4, new QTableWidgetItem("0x" + QString::number(elf_program.p_paddr, 16)));

        /*在文件中的大小*/
        ui->tableProgram->setItem(i, 5, new QTableWidgetItem("0x" + QString::number(elf_program.p_filesz, 16)));

        /*在内存中的大小*/
        ui->tableProgram->setItem(i, 6, new QTableWidgetItem("0x" + QString::number(elf_program.p_memsz, 16)));

        /*标志*/
        switch (elf_program.p_flags) {
        case PF_X:
            ui->tableProgram->setItem(i, 7, new QTableWidgetItem("X"));
            break;
        case PF_W:
            ui->tableProgram->setItem(i, 7, new QTableWidgetItem("W"));
            break;
        case PF_R:
            ui->tableProgram->setItem(i, 7, new QTableWidgetItem("R"));
            break;
        case PF_R | PF_W:
            ui->tableProgram->setItem(i, 7, new QTableWidgetItem("RW"));
            break;
        case PF_R | PF_X:
            ui->tableProgram->setItem(i, 7, new QTableWidgetItem("RX"));
            break;
        case PF_X | PF_W:
            ui->tableProgram->setItem(i, 7, new QTableWidgetItem("XW"));
            break;
        case PF_X | PF_W | PF_R:
            ui->tableProgram->setItem(i, 7, new QTableWidgetItem("RWX"));
            break;
        default:
            ui->tableProgram->setItem(i, 7, new QTableWidgetItem("其他!"));
            break;
        }

        /*对齐*/
        ui->tableProgram->setItem(i, 8, new QTableWidgetItem("0x" + QString::number(elf_program.p_align, 16)));
    }
}

//显示符号表对话框
void MainWindow::on_pBtnSymbol_clicked()
{
    SymbolDlg *syd = new SymbolDlg(this);
    syd->show();
}

//显示重定位表对话框
void MainWindow::on_pBtnReloc_clicked()
{
    RelocDlg *rd = new RelocDlg(this);
    rd->show();
}

//显示串表对话框
void MainWindow::on_pBtnString_clicked()
{
    StringDlg *sd = new StringDlg(this);
    sd->show();
}

Ui::MainWindow* MainWindow::getUIObj()
{
    return this->ui;
}
