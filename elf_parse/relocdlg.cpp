#include "mainwindow.h"
#include "relocdlg.h"
#include "ui_relocdlg.h"

RelocDlg::RelocDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RelocDlg)
{
    ui->setupUi(this);
    //解析reloc
    ParseReloc(parent);
}

RelocDlg::~RelocDlg()
{
    delete ui;
}

void RelocDlg::ParseReloc(QWidget *parent)
{
    //总的表项数目
    int rel_num = 0;
    //所有重定位表的索引
    QList<int> qlistRelNdx;
    MainWindow* pmainwindow = (MainWindow*)parent;
    //获取表项数
    for(int i = 0; i < pmainwindow->qlistSection.size(); i++)
    {
        //判断段表中的的重定位表根据SHT_REL进行区分
        if(pmainwindow->qlistSection.at(i).sh_type == SHT_REL)
        {
            //获取该段的信息，索引（用来区别在段表中的位置），表项数（区别输出多少行）
            rel_num += pmainwindow->qlistSection.at(i).sh_entsize;
            qlistRelNdx.append(i);
        }
    }
    //设置列表行数
    ui->tableReloc->setRowCount(rel_num);
    int count = 0;
    for(int j = 0; j < qlistRelNdx.size(); j++)
    {
        //获取该段的信息，名称，偏移，大小
        int shstartab_off = (pmainwindow->elf_header.e_shoff
                             + pmainwindow->elf_header.e_shstrndx
                             * pmainwindow->elf_header.e_shentsize) + 0x10;
        int shstartaboff_value = pmainwindow->qlist.at(shstartab_off + 0x3) << 24
                               | pmainwindow->qlist.at(shstartab_off + 0x2) << 16
                               | pmainwindow->qlist.at(shstartab_off + 0x1) << 8
                               | pmainwindow->qlist.at(shstartab_off);
        int sh_name_off = pmainwindow->qlistSection.at(qlistRelNdx.at(j)).sh_name + shstartaboff_value;
        //cout << i <<":"<< hex << sh_name_off << endl;
        char sh_name[24] = {'\0'};
        int off = 0;
        while(pmainwindow->qlist.at(sh_name_off + off) != '\0')
        {
            sh_name[off] = pmainwindow->qlist.at(sh_name_off + off);
            off++;
        }


        for(int m = 0; m < pmainwindow->qlistSection.at(qlistRelNdx.at(j)).sh_entsize; m++)
        {
            //获取偏移
            int sh_rel_off = pmainwindow->qlist.at((pmainwindow->qlistSection.at(qlistRelNdx.at(j)).sh_offset + m * sizeof elf_rel) + 0x3) << 24
                           | pmainwindow->qlist.at((pmainwindow->qlistSection.at(qlistRelNdx.at(j)).sh_offset + m * sizeof elf_rel) + 0x2) << 16
                           | pmainwindow->qlist.at((pmainwindow->qlistSection.at(qlistRelNdx.at(j)).sh_offset + m * sizeof elf_rel) + 0x1) << 8
                           | pmainwindow->qlist.at(pmainwindow->qlistSection.at(qlistRelNdx.at(j)).sh_offset + m * sizeof elf_rel);
            //序号
            QString str_index;
            str_index.setNum(count);
            ui->tableReloc->setItem(count, 0, new QTableWidgetItem(str_index));
            //设置名称
            QString str_name(sh_name);
            ui->tableReloc->setItem(count, 1, new QTableWidgetItem(str_name));
            ui->tableReloc->setItem(count, 2, new QTableWidgetItem("0x" + QString::number(sh_rel_off, 16)));

            //info
            int sh_rel_info_off = pmainwindow->qlist.at((pmainwindow->qlistSection.at(qlistRelNdx.at(j)).sh_offset + m * sizeof elf_rel) + 0x7) << 24
                                | pmainwindow->qlist.at((pmainwindow->qlistSection.at(qlistRelNdx.at(j)).sh_offset + m * sizeof elf_rel) + 0x6) << 16
                                | pmainwindow->qlist.at((pmainwindow->qlistSection.at(qlistRelNdx.at(j)).sh_offset + m * sizeof elf_rel) + 0x5) << 8
                                | pmainwindow->qlist.at((pmainwindow->qlistSection.at(qlistRelNdx.at(j)).sh_offset + m * sizeof elf_rel) + 0x4);
            //设置info值
            ui->tableReloc->setItem(count, 3, new QTableWidgetItem("0x" + QString::number(sh_rel_info_off, 16)));

            //类型ARM,需要根据平台判断是386，还是arm，或者其他平台，然后进行分别判断,

            switch(ELF32_R_TYPE(sh_rel_info_off))
            {
            case R_ARM_NONE:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_NONE"));
                break;
            case R_ARM_PC24:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_PC24"));
                break;
            case R_ARM_ABS32:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_ABS32"));
                break;
            case R_ARM_REL32:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_REL32"));
                break;
            case R_ARM_PC13:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_PC13"));
                break;
            case R_ARM_ABS16:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_ABS16"));
                break;
            case R_ARM_ABS12:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_ABS12"));
                break;
            case R_ARM_THM_ABS5:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_THM_ABS5"));
                break;
            case R_ARM_ABS8:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_ABS8"));
                break;
            case R_ARM_SBREL32:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_SBREL32"));
                break;
            case R_ARM_THM_PC22:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_THM_PC22"));
                break;
            case R_ARM_THM_PC8:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_THM_PC8"));
                break;
            case R_ARM_AMP_VCALL9:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_AMP_VCALL9"));
                break;
            case R_ARM_TLS_DESC:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_TLS_DESC"));
                break;
            case R_ARM_THM_SWI8:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_THM_SWI8"));
                break;
            case R_ARM_XPC25:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_XPC25"));
                break;
            case R_ARM_THM_XPC22:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_THM_XPC22"));
                break;
            case R_ARM_TLS_DTPMOD32:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_TLS_DTPMOD32"));
                break;
            case R_ARM_TLS_TPOFF32:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_TLS_TPOFF32"));
                break;
            case R_ARM_COPY:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_COPY"));
                break;
            case R_ARM_GLOB_DAT:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_GLOB_DAT"));
                break;
            case R_ARM_JUMP_SLOT:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_JUMP_SLOT"));
                break;
            case R_ARM_RELATIVE:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_RELATIVE"));
                break;
            case R_ARM_GOTOFF:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_GOTOFF"));
                break;
            case R_ARM_GOTPC:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_GOTPC"));
                break;
            case R_ARM_GOT32:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("R_ARM_GOT32"));
                break;
            default:
                ui->tableReloc->setItem(count, 4, new QTableWidgetItem("0x" + QString::number(ELF32_R_TYPE(sh_rel_info_off), 16)));
                break;
            }

            //Sym.ndx,该项在符号表内的索引
            int sym_ndx = ELF32_R_SYM(sh_rel_info_off);
            ui->tableReloc->setItem(count, 5, new QTableWidgetItem(QString::number(sym_ndx)));
            count++;
        }

    }
}
