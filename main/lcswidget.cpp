#include "lcswidget.h"

LcsWidget::LcsWidget(QWidget *parent) : QWidget(parent)
{
    raw_seq[0] = raw_seq[1] = common_seq[0] = common_seq[1] = NULL;
    setupUi();
    setupActions();
}

void LcsWidget::setupUi()
{
    QLabel *lowerLimLabel = new QLabel(tr("阈值下界"));
    QLabel *upperLimLabel = new QLabel(tr("阈值上界"));
    lowerLimBox = new QSpinBox;
    upperLimBox = new QSpinBox;
    lowerLimBox->setValue(0);
    upperLimBox->setValue(10);
    //lowerLimLabel->setAlignment(Qt::AlignCenter);
    //upperLimLabel->setAlignment(Qt::AlignCenter);
    thresholdSlider = new QSlider(Qt::Horizontal);
    thresholdSlider->setValue(5);
    QHBoxLayout *adjustLayout = new QHBoxLayout;
    adjustLayout->addWidget(lowerLimLabel);
    adjustLayout->addWidget(lowerLimBox);
    adjustLayout->addWidget(thresholdSlider);
    adjustLayout->addWidget(upperLimLabel);
    adjustLayout->addWidget(upperLimBox);
    //adjustLayout->addWidget(thresholdLabel);

    map = new MapWindow;
    map->setHtml("./html/lcm.html");
    map->setJs("./html/lcm.js");
    map->initJS();
    map->setDefaultCentralPt();
    map->reload();
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(adjustLayout);
    leftLayout->addWidget(map);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    for (int i = 0; i < 2; ++i) {
        fileNameLabel[i] = new QLabel;
        fileNameLabel[i]->setText(tr("未选择文件"));
        fileNameLabel[i]->setAlignment(Qt::AlignCenter);
        fileOpenButtion[i] = new QPushButton;
        fileOpenButtion[i]->setText(tr("打开文件 ") + (i + 'A'));

        rightLayout->addWidget(fileNameLabel[i]);
        rightLayout->addWidget(fileOpenButtion[i]);

        if (i == 1) { rightLayout->addSpacing(20); }
    }

    refreshButton = new QPushButton;
    refreshButton->setText(tr("开始分析"));
    QHBoxLayout *calcLayout = new QHBoxLayout;
    calcLayout->addSpacing(110);
    calcLayout->addWidget(refreshButton);
    calcLayout->addSpacing(110);

    groupBox = new QGroupBox(tr("详细信息"));
    QVBoxLayout *groupLayout = new QVBoxLayout;
    groupBox->setLayout(groupLayout);

    rightLayout->addWidget(groupBox);
    rightLayout->addLayout(calcLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout, 4);
    mainLayout->addLayout(rightLayout, 1);
    this->setLayout(mainLayout);

    attrThreshold = new AttrEdit(this, tr("当前阈值："));
    groupLayout->addWidget(attrThreshold);
    attrOriginSimilarity = new AttrEdit(this, tr("原始 Fréchet 距离："));
    groupLayout->addWidget(attrOriginSimilarity);
    for (int i = 0; i < 2; ++i) {
        attrOriginPointCount[i] = new AttrEdit(this, tr("轨迹") + (i + 'A') + tr("点数量："));
        groupLayout->addWidget((attrOriginPointCount[i]));
    }
    attrCommonSimilarity = new AttrEdit(this, tr("筛选后 Fréchet 距离："));
    groupLayout->addWidget(attrCommonSimilarity);
    attrCommonPointCount = new AttrEdit(this, tr("匹配区域点数量："));
    groupLayout->addWidget(attrCommonPointCount);

    updateThreshold();
}

void LcsWidget::setupActions()
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

void LcsWidget::openFile(int i)
{
    QString fileName = QFileDialog::getOpenFileName(NULL,tr("打开文件"),
                       "", "CSV Files(*.csv)", 0);
    if (fileName.isNull() || fileName.isEmpty()) {
        return;
    }
    QFileInfo fi(fileName);
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

void LcsWidget::calcLcmSequence()
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
            Core core;
            if (core.euclideanDistance(&p[i - 1],&q[j - 1]) <= threshold) {
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

void LcsWidget::drawSequences()
{
    QPair<double, double> centralPoint(0.0, 0.0);
    Sequence *seqs[] = {raw_seq[0], raw_seq[1]/*, common_seq[0], common_seq[1]*/};
    QVector<Sequence> seq_vec;

    for (size_t i = 0; i < sizeof(seqs) / sizeof(seqs[0]); ++i) {
        if (seqs[i] != NULL) {
            seq_vec.push_back(*seqs[i]);
        }
    }

    if (seq_vec.isEmpty()) {
        return;
    }

    for (int i = 0; i < seq_vec.size(); ++i) {
        centralPoint.first  += seq_vec[i].getCentralPoint().longitude;
        centralPoint.second += seq_vec[i].getCentralPoint().latitude;
    }

    centralPoint.first  /= seq_vec.size();
    centralPoint.second /= seq_vec.size();

    map->initJS();
    map->setDefaultCentralPt();
    map->showPoints(true);
    map->setCentralPoint(centralPoint.first, centralPoint.second, 5);

    map->drawSequences(seq_vec);
}

void LcsWidget::onLowerLimChanged(int value)
{
    if (++value >= 90) {
        upperLimBox->setMinimum(value);
    }
    updateThreshold();
}

void LcsWidget::onUpperLimChanged(int value)
{
    if (--value >= 0) {
        lowerLimBox->setMaximum(value);
    }
    updateThreshold();
}

void LcsWidget::onRefreshButtonClicked()
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

void LcsWidget::updateThreshold()
{
    double rangeLength = upperLimBox->value() - lowerLimBox->value();
    threshold = lowerLimBox->value() + rangeLength *
                thresholdSlider->value() / (double)thresholdSlider->maximum();
    //thresholdLabel->setText(tr("当前阈值: ") + QString::number(threshold));
}
