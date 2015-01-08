#ifndef TOOLSDIALOG_H
#define TOOLSDIALOG_H

#include <QDialog>

namespace Ui {
class ToolsDialog;
}

class MainWindow;

class ToolsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolsDialog(MainWindow *mw);
    ~ToolsDialog();

protected slots:
    /*void on_sweepButton_clicked() {
        parentWidget()->activateWindow();
    }*/

private:
    Ui::ToolsDialog *ui;

    MainWindow *mainWindow;
};

#endif // TOOLSDIALOG_H
