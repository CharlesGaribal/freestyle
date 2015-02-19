#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include <openglwidget.h>
#include "parametersdialog.h"
#include "toolsdialog.h"

#include "sculptor.h"
#include "openglwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static const char* APP_NAME;
    static const char* MANUAL_PATH;
    static const char* ABOUT_TEXT;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    ToolsDialog* getToolsDialog() {
        return toolsDialog;
    }

    ParametersDialog* getParametersDialog() {
        return parametersDialog;
    }

    OpenGLWidget* getOGLWidget() {
        return openGLWidget;
    }

    SculptorController* getSculptorController() {
        return sculptorController;
    }

private slots:
    void newSculpt();
    void open();
    void save();
    void saveAs();

    void resetCamera();
    void switchRenderingMode(bool);

    void switchToolsVisibility(bool);
    void openParameters();

    void openManual();
    void openAbout();

private:
    void loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    void reset();

    Ui::MainWindow *ui;

    OpenGLWidget *openGLWidget;

    SculptorController *sculptorController;
    ToolsDialog *toolsDialog;
    ParametersDialog *parametersDialog;

    QString curFile;
    QString nameFile;

    Sculptor *sculptor;
    QString path;
};

#endif // MAINWINDOW_H
