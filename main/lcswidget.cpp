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
        /* Label */
        fileNameLabel[i] = new QLabel(tr("未选择文件"));
        fileNameLabel[i]->setAlignment(Qt::AlignCenter);
        fileOpenButtion[i] = new QPushButton;
        fileOpenButtion[i]->setText(tr("打开文件 ") + (i + 'A'));

        rightLayout->addWidget(fileNameLabel[i]);

        /* Toolbar and buttion */
        QHBoxLayout *fileLayout = new QHBoxLayout;
        fileLayout->addWidget(toolBar[i] = new QToolBar, 1);
        // fileLayout->addSpacing(15);
        fileLayout->addWidget(fileOpenButtion[i], 5);
        fileLayout->addSpacing(20);

        rightLayout->addLayout(fileLayout);

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
        attrOriginPointCount[i] = new AttrEdit(this, tr("轨迹") + (i + 'A') +
                                               tr("样本点数量："));
        groupLayout->addWidget((attrOriginPointCount[i]));
    }
    attrCommonSimilarity = new AttrEdit(this, tr("筛选后 Fréchet 距离："));
    groupLayout->addWidget(attrCommonSimilarity);
    attrCommonPointCount = new AttrEdit(this, tr("匹配区域样本点数量："));
    groupLayout->addWidget(attrCommonPointCount);

    updateThreshold();
}

void LcsWidget::setupActions()
{
    /** 关联 打开按钮 和 打开动作 **/
    QSignalMapper *openSigMap   = new QSignalMapper(this);
    QSignalMapper *exportSigMap = new QSignalMapper(this);
    for (int i = 0; i < 2; ++i) {
        /** Open file **/
        connect(fileOpenButtion[i], SIGNAL(pressed()), openSigMap, SLOT(map()));
        openSigMap->setMapping(fileOpenButtion[i], i);

        /** Export file **/
        exportAction[i] = new QAction(
                    style()->standardIcon(QStyle::SP_DialogSaveButton),
                    tr("导出(&E)"),
                    this);
        exportAction[i]->setDisabled(true);
        connect(exportAction[i], SIGNAL(triggered()),
                exportSigMap, SLOT(map()));
        exportSigMap->setMapping(exportAction[i], i);
        toolBar[i]->addAction(exportAction[i]);
    }
    connect(openSigMap, SIGNAL(mapped(int)), this, SLOT(openFile(int)));
    connect(exportSigMap, SIGNAL(mapped(int)), this, SLOT(exportFile(int)));

    /** lowerLim upperLim 关系 **/
    connect(lowerLimBox, SIGNAL(valueChanged(int)), this,
            SLOT(onLowerLimChanged(int)));
    connect(upperLimBox, SIGNAL(valueChanged(int)), this,
            SLOT(onUpperLimChanged(int)));
    connect(thresholdSlider, SIGNAL(valueChanged(int)), this,
            SLOT(updateThreshold()));
    connect(refreshButton, SIGNAL(pressed()), this,
            SLOT(onRefreshButtonClicked()));

}

void LcsWidget::openFile(int i)
{
    QString fileName = QFileDialog::getOpenFileName(NULL, tr("打开文件"), "",
                                                    "CSV Files(*.csv)", 0);
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
    exportAction[i]->setEnabled(true);
}

void LcsWidget::exportFile(int i)
{
    if (common_seq[i] == NULL) {
        QMessageBox::warning(this, tr("无法导出"),
                             tr("请进行分析后再导出筛选过的 文件") + ('A' + i) +
                             tr(" 的目标 CSV！"),
                             QMessageBox::Yes);
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("导出筛选结果") +
                                                    ('A' + i), "",
                                                    tr("CSV Files (*.csv)"));
    if (fileName.isNull() || fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream os(&file);
    os << tr("经度,纬度\n");
    for (int j = 0; j < common_seq[i]->pointsNum; ++j) {
        os << (*common_seq[i])[j].longitude << ","
           << (*common_seq[i])[j].latitude << "\n";
    }
    file.close();
    QMessageBox::information(this, tr("保存成功"),
                             tr("已成功导出为 CSV 文件至 ") + fileName,
                             QMessageBox::Yes);
}

void LcsWidget::calcLcmSequence()
{
    for (int i = 0; i < 2; ++i) {
        if (common_seq[i] != NULL) {
            delete common_seq[i];
        }
        common_seq[i] = new Sequence;
    }

    Sequence &p   = *raw_seq[0],    &q   = *raw_seq[1];
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
    /* Draw original seuquences only */
    Sequence *seqs[] = {raw_seq[0], raw_seq[1]};
    QVector<Sequence> seq_vec;

    for (size_t i = 0; i < sizeof(seqs) / sizeof(seqs[0]); ++i) {
        if (seqs[i] != NULL) {
            seq_vec.push_back(*seqs[i]);
        }
    }

    if (seq_vec.isEmpty())
        return;

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
    if (++value >= 90)
        upperLimBox->setMinimum(value);
    updateThreshold();
}

void LcsWidget::onUpperLimChanged(int value)
{
    if (--value >= 0)
        lowerLimBox->setMaximum(value);
    updateThreshold();
}

void LcsWidget::onRefreshButtonClicked()
{
    if (raw_seq[0] == NULL || raw_seq[1] == NULL) {
        QMessageBox::warning(this, tr("缺少计算对象"),
                             tr("计算公共轨迹需要选择两条轨迹"),
                             QMessageBox::Yes);
    } else {
        Core core;
        calcLcmSequence();
        drawSequences();
        map->drawSequencePair(common_seq[0], common_seq[1], 1);
        attrOriginSimilarity->valLineEdit->setText(QString::number(
                        core.computeDiscreteFrechet(raw_seq[0], raw_seq[1])));
        for (int i = 0; i < 2; ++i) {
            attrOriginPointCount[i]->valLineEdit->setText(
                        QString::number(raw_seq[i]->getNum()));
        }
        attrCommonSimilarity->valLineEdit->setText(
                    QString::number(core.computeDiscreteFrechet(
                                        common_seq[0], common_seq[1])));
        attrCommonPointCount->valLineEdit->setText(
                    QString::number(common_seq[0]->getNum()));

        exportAction[0]->setEnabled(true);
        exportAction[1]->setEnabled(true);
        return;
    }
    drawSequences();
}

void LcsWidget::updateThreshold()
{
    double rangeLength = upperLimBox->value() - lowerLimBox->value();
    threshold = lowerLimBox->value() + rangeLength *
                thresholdSlider->value() / (double)thresholdSlider->maximum();
    attrThreshold->valLineEdit->setText(QString::number(threshold));
}
