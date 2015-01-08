#include "opengl.h"
#include "ftylrenderer.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "openglwidget.h"
#include <QtWidgets>
#include <QtGui>
#include <QMessageBox>
#include <QStatusBar>
#include <QGLFormat>
#include <QApplication>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>

#include <iostream>

using namespace vortex::util;

const char* MainWindow::APP_NAME = "Freestyle";
const char* MainWindow::MANUAL_PATH = "./manual/index.html";
const char* MainWindow::ABOUT_TEXT = "<center><h1>Freestyle editor</h1></center>\n<center>Charles Garibal - Maxime Robinot - Mathieu Dachy</center>\n<center>Masterpiece 2014/2015</center>\n<center>Version 0.0.1</center>";

const float MainWindow::DEFAULT_MAX_EDGE_LENGTH = 1.0;
const float MainWindow::DEFAULT_RATIO_MAX_MIN = 2.05;
const float MainWindow::DEFAULT_DTHICKNESS = 0.5;
const float MainWindow::DEFAULT_DMOVE = 0.15;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow),
    toolsDialog(this),
    parametersDialog(this),
    sculptor(SculptorParameters(DEFAULT_MAX_EDGE_LENGTH/DEFAULT_RATIO_MAX_MIN, DEFAULT_MAX_EDGE_LENGTH, DEFAULT_DMOVE, DEFAULT_DTHICKNESS))
{
    ui->setupUi(this);

    QGLFormat fmt;
    fmt.setDoubleBuffer(true);
    fmt.setDirectRendering(true);
    fmt.setRgba(true);
    fmt.setStencil(true);
    fmt.setDepth(true);
    fmt.setAlpha(true);
    fmt.setVersion(3,2);
    fmt.setProfile(QGLFormat::CoreProfile);
    QGLFormat::setDefaultFormat(fmt);

    openGLWidget = new OpenGLWidget(this);

    if (!(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_2))
        std::cerr << "error context OpenGL" << std::endl;

    setCentralWidget(openGLWidget);

    setCurrentFile("");

    connect(ui->actionNewSculpt, SIGNAL(triggered()), SLOT(newSculpt()));
    connect(ui->actionOpen, SIGNAL(triggered()), SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered()), SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), SLOT(saveAs()));
    connect(ui->actionExit, SIGNAL(triggered()), SLOT(close()));

    connect(ui->actionResetCamera, SIGNAL(triggered()), SLOT(resetCamera()));
    ui->actionFillWireframe->setChecked(true);
    connect(ui->actionFillWireframe, SIGNAL(triggered(bool)), SLOT(switchRenderingMode(bool)));

    connect(ui->actionShowHideTools, SIGNAL(triggered(bool)), SLOT(switchToolsVisibility(bool)));
    connect(ui->actionParameters, SIGNAL(triggered()), SLOT(openParameters()));

    connect(ui->actionManual, SIGNAL(triggered()), SLOT(openManual()));
    connect(ui->actionAbout, SIGNAL(triggered()), SLOT(openAbout()));

    addAction(ui->actionNewSculpt);
    addAction(ui->actionOpen);
    addAction(ui->actionSave);
    addAction(ui->actionSaveAs);
    addAction(ui->actionExit);
    addAction(ui->actionResetCamera);
    addAction(ui->actionFillWireframe);
    addAction(ui->actionShowHideTools);
    addAction(ui->actionParameters);
    addAction(ui->actionManual);
    addAction(ui->actionAbout);
}

MainWindow::~MainWindow() {
    delete openGLWidget;
    delete ui;
}

void MainWindow::newSculpt() {

}

void MainWindow::open() {
    static QString path("");
    QString fileName = QFileDialog::getOpenFileName(this, "Open 3D scene", path);

    if (!fileName.isEmpty()) {
        loadFile(fileName);
        // keep track of last valid dir
        path = QFileInfo(fileName).absolutePath();
    }
}

void MainWindow::loadFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning (this, tr(APP_NAME),
                              tr("Cannot read file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    openGLWidget->makeCurrent();

    bool succeed = openGLWidget->loadScene(fileName.toStdString());

    QApplication::restoreOverrideCursor();

    if (succeed) {
        reset();
        setCurrentFile(fileName);
        openGLWidget->updateGL();
    } else
        QMessageBox::warning(this, tr(APP_NAME), tr("Cannot read file %1\n%2").arg(fileName).arg(openGLWidget->sceneManager()->getLastErrorString()));
}

void MainWindow::setCurrentFile(const QString &fileName) {
    curFile = fileName;

    QString shownName;
    if (curFile.isEmpty())
        shownName = "untitled.obj";
    else
        shownName = strippedName(curFile);

    // save the name of the file without extension
    nameFile = shownName;
    nameFile.truncate(shownName.indexOf(".", 0));
    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr(APP_NAME)));
}

QString MainWindow::strippedName(const QString &fullFileName) {
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::save() {

}

void MainWindow::saveAs() {

}

void MainWindow::resetCamera() {
    openGLWidget->resetCamera();
    openGLWidget->updateGL();
}

void MainWindow::switchRenderingMode(bool on) {
    openGLWidget->switchRenderingMode(!on);
}

void MainWindow::switchToolsVisibility(bool) {
    toolsDialog.show();
}

void MainWindow::openParameters() {
    parametersDialog.show();
}

void MainWindow::openManual() {
    QDesktopServices::openUrl(QUrl("manual/index.html"));
}

void MainWindow::openAbout() {
    QString s = "<center><h1>Freestyle editor</h1></center>\n";
    s += "<center>Charles Garibal - Maxime Robinot - Mathieu Dachy</center>\n";
    s += "<center>Masterpiece 2014/2015</center>\n";
    s += "<center>Version 0.0.1</center>";
    QMessageBox::about(this, "Freestyle", s);
}

void MainWindow::reset(){
    resetCamera();
}
