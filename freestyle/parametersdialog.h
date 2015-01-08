#ifndef PARAMETERSDIALOG_H
#define PARAMETERSDIALOG_H

#include <QDialog>

namespace Ui {
class ParametersDialog;
}

class MainWindow;

class ParametersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParametersDialog(MainWindow *mw);
    ~ParametersDialog();

private:
    Ui::ParametersDialog *ui;


    MainWindow *mainWindow;
};

#endif // PARAMETERSDIALOG_H
