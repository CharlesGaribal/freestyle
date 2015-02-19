#ifndef TOOLSDIALOG_H
#define TOOLSDIALOG_H

#include <QDialog>

class SculptorController;

namespace Ui {
class ToolsDialog;
}

class MainWindow;

class ToolsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolsDialog(QWidget *parent, SculptorController *controller);
    ~ToolsDialog();

    void setToolRadius(float toolRadius);

protected slots:
    void on_radiusSlider_valueChanged(int value);
    void on_radiusSpinBox_valueChanged(double value);

private:
    float getFloatRadiusValue(int value);
    int getIntRadiusValue(double value);

    Ui::ToolsDialog *ui;

    SculptorController *controller;
};

#endif // TOOLSDIALOG_H
