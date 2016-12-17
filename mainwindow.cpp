#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionExit, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
    connect(ui->actionOpen_Directory, SIGNAL(triggered(bool)), SLOT(openDirectory()));
    connect(ui->actionNext_Image, SIGNAL(triggered(bool)), SLOT(nextImage()));
    connect(ui->actionPrev_Image, SIGNAL(triggered(bool)), SLOT(prevImage()));
    connect(ui->actionSelect_Image, SIGNAL(triggered(bool)), SLOT(selectImage()));

    connect(ui->actionClear, SIGNAL(triggered(bool)), ui->selectionOnImageWidget, SLOT(clearSelections()));
    connect(ui->actionClear_Last_Selection, SIGNAL(triggered(bool)), ui->selectionOnImageWidget, SLOT(clearLastSelection()));
    connect(ui->selectionOnImageWidget, SIGNAL(nextImageRequested()), SLOT(nextImage()));
    connect(ui->selectionOnImageWidget, SIGNAL(prevImageRequested()), SLOT(prevImage()));

    openDirectory(lastDir());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Directory with images", lastDir());

    if (!dir.isEmpty())
    {
        setLastDir(dir);
        openDirectory(dir);
    }
}

void MainWindow::openDirectory(const QString &dir)
{
    m_dir = dir;
    QDir d(dir);
    m_files = d.entryInfoList({"*.jpg"});

    int lastIdxWithSel = 0;
    for (int i=1; i<m_files.size(); i++)
    {
        QString selPath = SelectionOnImage::selectionsPath(m_files[i].filePath());
        if (!QFileInfo::exists(selPath))
        {
            lastIdxWithSel = i-1;
            break;
        }
    }
    // load the first image without selections saved
    selectImage(lastIdxWithSel);
}

void MainWindow::selectImage()
{
    QStringList items;
    for (auto fi: m_files)
    {
        items.append(fi.fileName());
    }

    QString item = QInputDialog::getItem(this, "Select Image", "Image to jump to", items, m_currentFile);

    for (int i=0; i<items.size(); i++)
    {
        if (item == items[i])
            selectImage(i);
    }
}

void MainWindow::selectImage(int idx)
{
    if (idx >= 0 && idx < m_files.count())
    {
        m_currentFile = idx;
        ui->selectionOnImageWidget->load(m_files[m_currentFile]);
        setWindowTitle(m_files[m_currentFile].fileName() + " " + m_dir);
    }
}

void MainWindow::prevImage()
{
    selectImage(m_currentFile-1);
}

void MainWindow::nextImage()
{
    selectImage(m_currentFile+1);
}


QString MainWindow::lastDir() const
{
    QSettings s;
    return s.value("last_dir", ".").toString();
}

void MainWindow::setLastDir(const QString &dir)
{
    QSettings s;
    s.setValue("last_dir", dir);
}

