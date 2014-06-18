#include "grapharea.h"
#include "ui_grapharea.h"

GraphArea::GraphArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphArea)
{
    ui->setupUi(this);
}

GraphArea::~GraphArea()
{
    delete ui;
}
