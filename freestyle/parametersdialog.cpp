#include "parametersdialog.h"
#include "ui_parametersdialog.h"
#include "mainwindow.h"

ParametersDialog::ParametersDialog(QWidget *parent, SculptorController *controller) : QDialog(parent),
    ui(new Ui::ParametersDialog),
    controller(controller)
{
    ui->setupUi(this);

    setParameters(controller->getParameters());
}

ParametersDialog::~ParametersDialog() {
    delete ui;
}

void ParametersDialog::setParameters(const SculptorParameters &params) {
    ui->minEdgeSpinBox->setValue(params.getMinEdgeLength());
    ui->maxEdgeSpinBox->setValue(params.getMaxEdgeLength());
    ui->thicknessSpinBox->setValue(params.getDThickness());
    ui->dispSpinBox->setValue(params.getDMove());
}
