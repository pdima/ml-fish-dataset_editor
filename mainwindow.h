#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openDirectory();
    void openDirectory(const QString& dir);
    void selectImage();
    void nextImage();
    void prevImage();

private:
    QString lastDir() const;
    void setLastDir(const QString& dir);
    void selectImage(int idx);


    Ui::MainWindow *ui;
    QString m_dir;
    int m_currentFile;
    QFileInfoList m_files;
};

#endif // MAINWINDOW_H
