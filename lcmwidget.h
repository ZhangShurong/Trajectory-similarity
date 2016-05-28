#ifndef LCMWIDGET_H
#define LCMWIDGET_H

#include <QWidget>
#include <QSpinBox>
#include <QLayout>
#include "mapwindow.h"

class lcmWidget : public QWidget
{
    Q_OBJECT
public:
    explicit lcmWidget(QWidget *parent = 0);

private:
    void setupUi();
    void setupActions();

    MapWindow   *map;

    QSlider     *thresholdSlider;
    QLabel      *thresholdLabel;
    QPushButton *refreshButton;

    QSpinBox    *upperLimBox;
    QSpinBox    *lowerLimBox;

    QPushButton *fileOpenButtion[2];
    QLabel      *fileNameLabel[2];

    /****** Non-UI part ********/
    Sequence *raw_seq[2], *common_seq[2];
    double threshold;

signals:

public slots:
    void openFile(int i);
    void calcLcmSequence();
    void drawSequences();
    void onLowerLimChanged(int value);
    void onUpperLimChanged(int value);
    void onRefreshButtonClicked();
    void updateThreshold();
};


#endif // LCMWIDGET_H
