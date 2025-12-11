#include "previewwidget.h"
#include <QPainter>

PreviewWidget::PreviewWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(50);
    setAutoFillBackground(true);
    
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

void PreviewWidget::setPreviewFont(const QFont &font)
{
    previewFont = font;
    update();
}

void PreviewWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setFont(previewFont);
    painter.setPen(Qt::black);
    painter.drawText(rect(), Qt::AlignVCenter | Qt::AlignLeft, "  ABCDEFG abcdefg");
}
