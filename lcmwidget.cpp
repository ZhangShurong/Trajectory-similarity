#include "lcmwidget.h"

lcmWidget::lcmWidget(QWidget *parent) : QWidget(parent)
{
    raw_seq[0] = raw_seq[1] = common_seq[0] = common_seq[1] = NULL;
    setupUi();
    setupActions();
}

void lcmWidget::setupUi()
{
    QLabel      *lowerLimLabel = new QLabel;
    QLabel      *upperLimLabel = new QLabel;
    lowerLimBox = new QSpinBox;
    upperLimBox = new QSpinBox;
    lowerLimBox->setValue(0);
    upperLimBox->setValue(10);
    lowerLimLabel->setText(tr("阈值下界"));
    upperLimLabel->setText(tr("阈值上界"));
    lowerLimLabel->setAlignment(Qt::AlignRight);
    upperLimLabel->setAlignment(Qt::AlignRight);
    thresholdLabel = new QLabel;
    refreshButton = new QPushButton;
    refreshButton->setText(tr("开始分析"));
    QHBoxLayout *adjustBarLayout = new QHBoxLayout;
    adjustBarLayout->addWidget(lowerLimLabel);
    adjustBarLayout->addWidget(lowerLimBox);
    adjustBarLayout->addWidget(upperLimLabel);
    adjustBarLayout->addWidget(upperLimBox);
    adjustBarLayout->addWidget(thresholdLabel);
    adjustBarLayout->addWidget(refreshButton);

    QHBoxLayout *fileBarLayout = new QHBoxLayout;
    for (int i = 0; i < 2; ++i) {
        fileNameLabel[i] = new QLabel;
        fileNameLabel[i]->setText(tr("未选择文件"));
        fileNameLabel[i]->setAlignment(Qt::AlignCenter);
        fileOpenButtion[i] = new QPushButton;
        fileOpenButtion[i]->setText(tr("打开文件 ") + QString::number(i + 1));

        fileBarLayout->addWidget(fileNameLabel[i]);
        fileBarLayout->addWidget(fileOpenButtion[i]);
    }

    map = new MapWindow;
    map->setHtml("./html/lcm.html");
    map->setJs("./html/lcm.js");
    map->initJS();
    map->setDefaultCentralPt();
    map->reload();

    thresholdSlider = new QSlider;
    QHBoxLayout *mapLayout = new QHBoxLayout;
    mapLayout->addWidget(map);
    mapLayout->addWidget(thresholdSlider);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(fileBarLayout);
    mainLayout->addLayout(adjustBarLayout);
    mainLayout->addLayout(mapLayout);
    this->setLayout(mainLayout);
}

void lcmWidget::setupActions()
{
    /** 关联 打开按钮 和 打开动作 **/
    QSignalMapper *signalMapper = new QSignalMapper(this);
    for (int i = 0; i < 2; ++i) {
        connect(fileOpenButtion[i], SIGNAL(pressed()), signalMapper, SLOT(map()));
        signalMapper->setMapping(fileOpenButtion[i], i);
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(openFile(int)));

    /** lowerLim upperLim 关系 **/
    connect(lowerLimBox, SIGNAL(valueChanged(int)), this, SLOT(onLowerLimChanged(int)));
    connect(upperLimBox, SIGNAL(valueChanged(int)), this, SLOT(onUpperLimChanged(int)));
    connect(thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(updateThreshold()));
    connect(refreshButton, SIGNAL(pressed()), this, SLOT(onRefreshButtonClicked()));
}

void lcmWidget::openFile(int i)
{
    QString fileName = QFileDialog::getOpenFileName(NULL,tr("Open File"),
                                                    "", "CSV Files(*.csv)", 0);

    QFileInfo fi =   QFileInfo(fileName);
    QString name = fi.fileName();
    fileNameLabel[i]->setText(name);

    ifstream in(fileName.toLocal8Bit().data());
    Csv csv(in);
    if (raw_seq[i] != NULL) {
        delete raw_seq[i];
    }
    getSquFromFile(&csv, raw_seq[i] = new Sequence);
    in.close();
    raw_seq[i]->setID(QString::number(i + 1));
}

void lcmWidget::calcLcmSequence()
{
    for (int i = 0; i < 2; ++i) {
        if (common_seq[i] != NULL) {
            delete common_seq[i];
        }
        common_seq[i] = new Sequence;
    }

    Sequence &p   = *raw_seq[0], &q   = *raw_seq[1];
    Sequence &c_p = *common_seq[0], &c_q = *common_seq[1];

    QVector< QVector <QPair<size_t, char> > > m(p.pointsNum + 1);

    for (int i = 0; i < m.size(); ++i) {
        m[i].resize(q.pointsNum + 1);
    }

    for (int i = 1; i <= p.pointsNum; ++i) {
        for (int j = 1; j <= p.pointsNum; ++j) {
            if (euclideanDistance(p[i - 1], q[j - 1]) <= threshold) {
                m[i][j].first = m[i - 1][j - 1].first + 1;
                m[i][j].second = '\\';
            } else if (m[i][j - 1].first > m[i - 1][j].first) {
                m[i][j].first = m[i][j - 1].first;
                m[i][j].second = '<';
            } else {
                m[i][j].first = m[i - 1][j].first;
                m[i][j].second = '^';
            }
        }
    }

    int i = p.pointsNum, j = q.pointsNum;
    while (m[i][j].first > 0) {
        switch (m[i][j].second) {
        case '<':
            --j;
            break;
        case '^':
            --i;
            break;
        case '\\':
            c_p.appendPt(&p[i - 1]);
            c_q.appendPt(&q[i - 1]);
            --i;
            --j;
            break;
        default:
            cerr << "Unrecognized character: " << m[i][j].first << endl;
            break;
        }
    }
}

void lcmWidget::drawSequences()
{
    QPair<double, double> centralPoint(0.0, 0.0);
    int noneNullCount = 0;
    Sequence *seqs[] = {raw_seq[0], raw_seq[1]/*, common_seq[0], common_seq[1]*/};
    for (size_t i = 0; i < sizeof(seqs) / sizeof(seqs[0]); ++i) {
        if (seqs[i] == NULL)
            continue;
        ++noneNullCount;
    }
    if (noneNullCount == 0) {
        //cerr << "No sequence to draw!" << endl;
        return;
    }
    for (size_t i = 0; i < sizeof(seqs) / sizeof(seqs[0]); ++i) {
        if (seqs[i] == NULL)
            continue;
        centralPoint.first  += seqs[i]->getCentralPoint().longitude;
        centralPoint.second += seqs[i]->getCentralPoint().latitude;
    }
    centralPoint.first  /= noneNullCount;
    centralPoint.second /= noneNullCount;

    map->initJS();
    map->setDefaultCentralPt();
    map->showPoints(true);
    map->setCentralPoint(centralPoint.first, centralPoint.second, 10);
    for (size_t i = 0; i < sizeof(seqs) / sizeof(seqs[0]); ++i) {
        if (seqs[i] == NULL)
            continue;
        map->drawSequence(*seqs[i], i);
    }
}

void lcmWidget::onLowerLimChanged(int value)
{
    if (++value >= 90) {
        upperLimBox->setMinimum(value);
    }
    updateThreshold();
}

void lcmWidget::onUpperLimChanged(int value)
{
    if (--value >= 0) {
        lowerLimBox->setMaximum(value);
    }
    updateThreshold();
}

void lcmWidget::onRefreshButtonClicked()
{
    if (raw_seq[0] == NULL || raw_seq[1] == NULL) {
        QMessageBox::warning(this, tr("缺少计算对象"),
                             tr("计算公共轨迹需要选择两条轨迹"), QMessageBox::Yes);
    } else {
        calcLcmSequence();
        drawSequences();
        map->drawSequencePair(common_seq[0], common_seq[1], 1);
        return;
    }
    drawSequences();// */
}

void lcmWidget::updateThreshold()
{
    double rangeLength = upperLimBox->value() - lowerLimBox->value();
    threshold = lowerLimBox->value() + rangeLength *
            thresholdSlider->value() / (double)thresholdSlider->maximum();
    thresholdLabel->setText(tr("当前阈值: ") + QString::number(threshold));
}
