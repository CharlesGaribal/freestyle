#include "toolsdialog.h"
#include "ui_toolsdialog.h"
#include "mainwindow.h"
#include "sculptorcontroller.h"

ToolsDialog::ToolsDialog(QWidget *parent, SculptorController *controller) : QDialog(parent),
    ui(new Ui::ToolsDialog),
    controller(controller)
{
    ui->setupUi(this);

    ui->radiusSpinBox->blockSignals(true);
    ui->radiusSpinBox->setMinimum(controller->getMinToolRadius());
    ui->radiusSpinBox->setMaximum(controller->getMaxToolRadius());
    ui->radiusSpinBox->blockSignals(false);

    setToolRadius(controller->getToolRadius());
    setToolSelected(controller->getToolSelected());
}

ToolsDialog::~ToolsDialog() {
    delete ui;
}

void ToolsDialog::setToolRadius(float toolRadius) {
    ui->radiusSlider->blockSignals(true);
    ui->radiusSlider->setValue(getIntRadiusValue(toolRadius));
    ui->radiusSlider->blockSignals(false);

    ui->radiusSpinBox->blockSignals(true);
    ui->radiusSpinBox->setValue(toolRadius);
    ui->radiusSpinBox->blockSignals(false);
}

void ToolsDialog::setToolSelected(SculptorController::OperatorType type) {
   switch (type) {
        case SculptorController::OperatorType::SWEEP:
            ui->defInfButton->setChecked(false);
            ui->sweepButton->setChecked(true);
            ui->twistButton->setChecked(false);
            break;
        case SculptorController::OperatorType::INFDEFLATE:
            ui->defInfButton->setChecked(true);
            ui->sweepButton->setChecked(false);
            ui->twistButton->setChecked(false);
            break;
        case SculptorController::OperatorType::TWIST:
            ui->defInfButton->setChecked(false);
            ui->sweepButton->setChecked(false);
            ui->twistButton->setChecked(true);
            break;
        default:
            ui->defInfButton->setChecked(false);
            ui->sweepButton->setChecked(false);
            ui->twistButton->setChecked(false);
            break;
    }
}

void ToolsDialog::on_radiusSlider_valueChanged(int value) {
    float radius = getFloatRadiusValue(value);
    controller->toolRadiusChanged(radius);
}

void ToolsDialog::on_radiusSpinBox_valueChanged(double value) {
    controller->toolRadiusChanged(value);
}

void ToolsDialog::on_sweepButton_clicked() {
    controller->sweepSelected();
}

void ToolsDialog::on_defInfButton_clicked() {
    controller->infDefSelected();
}

void ToolsDialog::on_twistButton_clicked() {
    controller->twistSelected();
}

void ToolsDialog::hideEvent(QHideEvent *event) {
    controller->toolDialogHidden();
}

float ToolsDialog::getFloatRadiusValue(int value) {
    float minInt = ui->radiusSlider->minimum();
    float maxInt = ui->radiusSlider->maximum();
    float min = controller->getMinToolRadius();
    float max = controller->getMaxToolRadius();

    return (max-min) * ((value-minInt)/(maxInt-minInt)) + min;
}

int ToolsDialog::getIntRadiusValue(double value) {
    float minInt = ui->radiusSlider->minimum();
    float maxInt = ui->radiusSlider->maximum();
    float min = controller->getMinToolRadius();
    float max = controller->getMaxToolRadius();

    return (maxInt-minInt) * ((value-min)/(max-min)) + minInt;
}
