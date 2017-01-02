#include "selectiondetailsview.h"
#include "ui_selectiondetailsview.h"

#include "selectionmodel.h"

SelectionDetailsView::SelectionDetailsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectionDetailsView)
{
    ui->setupUi(this);

    connect(ui->checkSmallPart, SIGNAL(clicked(bool)), SLOT(updateModelFromBoxes()));
    connect(ui->checkWrongSpecies, SIGNAL(clicked(bool)), SLOT(updateModelFromBoxes()));
    connect(ui->checkVeryLowQuality, SIGNAL(clicked(bool)), SLOT(updateModelFromBoxes()));
}

SelectionDetailsView::~SelectionDetailsView()
{
    delete ui;
}

void SelectionDetailsView::setModel(SelectionModel *model)
{
    m_model = model;
    ui->selectionCropView->setModel(model);
    ui->selectionMaskView->setModel(model);

    connect(model, SIGNAL(changed()), SLOT(updateBoxes()));
}

void SelectionDetailsView::updateBoxes()
{
    if (m_model->isEmpty())
    {
        setEnabled(false);
        ui->checkSmallPart->setChecked(false);
        ui->checkWrongSpecies->setChecked(false);
        ui->checkVeryLowQuality->setChecked(false);
    }
    else
    {
        setEnabled(true);
        ui->checkSmallPart->setChecked(m_model->currentSelection().smallPart);
        ui->checkWrongSpecies->setChecked(m_model->currentSelection().wrongSpecies);
        ui->checkVeryLowQuality->setChecked(m_model->currentSelection().lowQuality);
    }
}

void SelectionDetailsView::updateModelFromBoxes()
{
    m_model->currentSelection().smallPart = ui->checkSmallPart->isChecked();
    m_model->currentSelection().wrongSpecies = ui->checkWrongSpecies->isChecked();
    m_model->currentSelection().lowQuality = ui->checkVeryLowQuality->isChecked();
    m_model->save();
    m_model->update();
}
