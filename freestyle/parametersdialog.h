#ifndef PARAMETERSDIALOG_H
#define PARAMETERSDIALOG_H

#include <QDialog>
#include "sculptorcontroller.h"

class SculptorParameters;

namespace Ui {
class ParametersDialog;
}

class MainWindow;

class ParametersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParametersDialog(QWidget *parent, SculptorController *controller);
    ~ParametersDialog();

    void setParameters(const SculptorParameters &params);
private:
    Ui::ParametersDialog *ui;

    SculptorController *controller;
};

#endif // PARAMETERSDIALOG_H
