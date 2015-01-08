#include "toolsdialog.h"
#include "ui_toolsdialog.h"
#include "mainwindow.h"

ToolsDialog::ToolsDialog(MainWindow *mw) : QDialog(mw),
    ui(new Ui::ToolsDialog),
    mainWindow(mw)
{
    ui->setupUi(this);
}

ToolsDialog::~ToolsDialog()
{
    delete ui;
}
