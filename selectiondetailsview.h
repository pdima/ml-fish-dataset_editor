#ifndef SELECTIONDETAILSVIEW_H
#define SELECTIONDETAILSVIEW_H

#include <QWidget>

class SelectionModel;

namespace Ui {
class SelectionDetailsView;
}

class SelectionDetailsView : public QWidget
{
    Q_OBJECT

public:
    explicit SelectionDetailsView(QWidget *parent = 0);
    ~SelectionDetailsView();

    void setModel(SelectionModel* model);

private slots:
    void updateBoxes();
    void updateModelFromBoxes();

private:
    Ui::SelectionDetailsView *ui;
    SelectionModel* m_model;
};

#endif // SELECTIONDETAILSVIEW_H
