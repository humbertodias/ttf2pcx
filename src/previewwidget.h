#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QWidget>
#include <QFont>

class PreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewWidget(QWidget *parent = nullptr);
    
    void setPreviewFont(const QFont &font);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QFont previewFont;
};

#endif // PREVIEWWIDGET_H
