#include "parametersdialog.h"
#include "ui_parametersdialog.h"
#include "mainwindow.h"

ParametersDialog::ParametersDialog(MainWindow *mw) : QDialog(mw),
    ui(new Ui::ParametersDialog),
    mainWindow(mw)
{
    ui->setupUi(this);
}

ParametersDialog::~ParametersDialog()
{
    delete ui;
}
