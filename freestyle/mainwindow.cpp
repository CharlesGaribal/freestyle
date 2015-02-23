#include "../engine/opengl.h"
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

#include "assimpexporter.h"

#include <iostream>

using namespace vortex::util;

const char* MainWindow::APP_NAME = "Freestyle";
const char* MainWindow::MANUAL_PATH = "./manual/index.pdf";
const char* MainWindow::ABOUT_TEXT = "<center><h1>Freestyle editor</h1></center>\n<center>Charles Garibal - Maxime Robinot - Mathieu Dachy</center>\n<center>Masterpiece 2014/2015</center>\n<center>Version 0.0.1</center>";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
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
    openGLWidget->setMouseTracking(true);

    sculptorController = new SculptorController(this);

    toolsDialog = new ToolsDialog(this, sculptorController);
    parametersDialog = new ParametersDialog(this, sculptorController);

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

    //loadFile("../data/bimba.off");
    loadFile("../data/1sphere.dae");
}

MainWindow::~MainWindow() {
    delete openGLWidget;
    delete toolsDialog;
    delete parametersDialog;
    delete sculptorController;
    delete ui;
}

void MainWindow::newSculpt()
{
    loadFile("../data/bimba.off");
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open 3D scene", path);

    if (!fileName.isEmpty()) {
        loadFile(fileName);

        // keep track of last valid dir
        path = QFileInfo(fileName).absolutePath();
    }
}

void MainWindow::loadFile(const QString &fileName)
{
    if (!fileName.isEmpty())
    {
        // keep track of last valid dir
        path = QFileInfo(fileName).absolutePath();
    }
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

        sculptorController->sceneLoaded();
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

void MainWindow::save()
{
    vortex::assimpexporter exporter;
    int i = curFile.lastIndexOf(".");
    QString extension = curFile;
    extension.remove(0, i+1);

    if(exporter.exportScene(curFile.toStdString(), extension.toStdString(), openGLWidget->sceneManager()))
        QMessageBox::warning(this, tr("Sauvegarde"), tr("Save successful !"));
    else
        QMessageBox::warning(this, tr("Sauvegarde"), tr("Save failed ! %1").arg(curFile));
}

void MainWindow::saveAs()
{
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this, "Choose a file to save into", path, "*.obj;;*.dae;;*.stl;;*.ply", &selectedFilter);
    selectedFilter.remove(0, 1);

    if (!fileName.isEmpty())
    {
        if(!fileName.endsWith(selectedFilter))
            fileName.insert(fileName.length(), selectedFilter);

        QString old_cur_File = curFile;
        curFile = fileName;
        save();
        curFile = old_cur_File;
    }
}

void MainWindow::resetCamera() {
    openGLWidget->resetCamera();
    openGLWidget->updateGL();
}

void MainWindow::switchRenderingMode(bool on) {
    openGLWidget->switchRenderingMode(!on);
}

void MainWindow::switchToolsVisibility(bool on) {
    if (on)
        toolsDialog->show();
    else
        toolsDialog->hide();

    ui->actionShowHideTools->setChecked(on);
}

void MainWindow::openParameters() {
    parametersDialog->show();
}

void MainWindow::openManual() {
    QDesktopServices::openUrl(QUrl(MANUAL_PATH));
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
