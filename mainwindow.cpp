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

    ui->fullImageViewWidget->setModel(&m_model);
    ui->selectionDetailsView->setModel(&m_model);

    connect(ui->actionExit, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
    connect(ui->actionOpen_Directory, SIGNAL(triggered(bool)), SLOT(openDirectory()));
    connect(ui->actionNext_Image, SIGNAL(triggered(bool)), SLOT(nextImage()));
    connect(ui->actionPrev_Image, SIGNAL(triggered(bool)), SLOT(prevImage()));
    connect(ui->actionSelect_Image, SIGNAL(triggered(bool)), SLOT(selectImage()));

    connect(ui->actionClear, SIGNAL(triggered(bool)), &m_model, SLOT(clearSelections()));
    connect(ui->actionClear_Current_Selection, SIGNAL(triggered(bool)), &m_model, SLOT(clearCurrentSelection()));

    connect(ui->fullImageViewWidget, SIGNAL(nextImageRequested()), SLOT(nextImage()));
    connect(ui->fullImageViewWidget, SIGNAL(prevImageRequested()), SLOT(prevImage()));
    connect(ui->actionSelect_unset_species, SIGNAL(triggered(bool)), SLOT(findNextUnsetSpecies()));

    connect(&m_model, SIGNAL(nextImageRequested()), SLOT(nextImage()));
    connect(&m_model, SIGNAL(prevImageRequested()), SLOT(prevImage()));

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
        QString selPath = SelectionModel::selectionsPath(m_files[i].filePath());
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
        m_model.load(m_files[m_currentFile]);
        setWindowTitle(QString("%1 %2  %3/%4").arg(m_files[m_currentFile].fileName()).arg(m_dir).arg(idx+1).arg(m_files.count()));
    }
}

void MainWindow::prevImage()
{
    selectImage(m_currentFile-1);
}

void MainWindow::findNextUnsetSpecies()
{
    while(m_currentFile < m_files.count()-1)
    {
        int idx = m_model.missingWrongSpeciesSelectionIdx();
        if (idx >= 0)
        {
            m_model.setCurrentSelection(idx);
            return;
        }
        nextImage();
    }
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

