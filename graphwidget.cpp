#include "graphwidget.h"
#include "ui_graphwidget.h"

#include <QDebug>

GraphWidget::GraphWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphWidget)
{
    ui->setupUi(this);
    connect(ui->graphArea, SIGNAL(scrollChanged()), this, SLOT(onScrollChanged()));
    xScaleMin = yScaleMin = 0.01;
    xScaleMax = yScaleMax = 100;
    ui->xScale->setValue(ui->xScale->maximum()/2);
    ui->yScale->setValue(ui->yScale->maximum()/2);

}

GraphWidget::~GraphWidget()
{
    delete ui;
}

GraphArea* GraphWidget::graph()
{
    return ui->graphArea;
}

void GraphWidget::setXScaleBounds(float min, float max)
{
    xScaleMin = min;
    xScaleMax = max;
    updateScale();
}

void GraphWidget::setYScaleBounds(float min, float max)
{
    //yScaleMin = min;
    //yScaleMax = max;
    ui->yScale->setMinimum(log(min)/log(1.2));
    ui->yScale->setMaximum(ceil(log(max)/log(1.2)));
    ui->yScale->setValue((ui->yScale->maximum()-ui->yScale->minimum())/2);
    //updateScale();
}

void GraphWidget::setXScale(float value)
{
    //ui->xScale->setValue(log(value)/log(1.2));
}

void GraphWidget::setYScale(float value)
{
    ui->yScale->setValue(log(value)/log(1.2));
}

void GraphWidget::updateScale()
{
    //qDebug() << "onScale";
    //ui->graphArea->setScale(exp(ui->xScale->value()/10.0), exp(ui->yScale->value()/10.0));
    /*ui->graphArea->setScale(
                xScaleMin + ui->xScale->value()*(xScaleMax - xScaleMin)/ui->xScale->maximum(),
                yScaleMin + ui->yScale->value()*(yScaleMax - yScaleMin)/ui->yScale->maximum());
    */
    ui->graphArea->setScale(
                xScaleMin + ui->xScale->value()*(xScaleMax - xScaleMin)/ui->xScale->maximum(),
                exp(ui->yScale->value()*log(1.2)));

}

void GraphWidget::onScrollChanged()
{
    //qDebug() << "onScrollChanged()";
    ui->xScroll->setMaximum(ui->graphArea->getMaxScroll());
    ui->xScroll->setPageStep(ui->graphArea->getScrollPage());
    ui->xScroll->setValue(ui->xScroll->maximum() - ui->graphArea->getScroll());
}

void GraphWidget::on_xScale_valueChanged(int value)
{
    updateScale();
}

void GraphWidget::on_yScale_valueChanged(int value)
{
    updateScale();
}

void GraphWidget::on_xScroll_valueChanged(int value)
{
    //qDebug() << "on_xScroll_valueChanged()";
    ui->graphArea->setScroll(ui->xScroll->maximum() - value);
}

void GraphWidget::on_isVarInterval_clicked(bool checked)
{
    ui->graphArea->setVariableInterval(checked);
}
