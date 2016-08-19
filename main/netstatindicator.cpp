#include "netstatindicator.h"

const QString NetStatIndicator::okStyleStr = "QLabel { color : white; }";
const QString NetStatIndicator::badStyleStr = "QLabel { color : red; }";

NetStatIndicator::NetStatIndicator(QWidget *parent) :
    QWidget(parent), alerted(false), timer(this)
{
    /** Ui part **/
    this->netspeedLabel = new QLabel;
    this->netspeedLabel->setStyleSheet(okStyleStr);
    this->netspeedLabel->setText(tr("N/A"));

    this->netStatLabel = new QLabel;
    this->netStatLabel->setStyleSheet(okStyleStr);
    this->netspeedLabel->setAlignment(Qt::AlignRight);
    this->netspeedLabel->setText(tr("Unknown network status"));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(this->netStatLabel);
    layout->addSpacing(10); 	// 10 pixels at least
    layout->addWidget(this->netspeedLabel);
    this->setLayout(layout);

    /** Actions **/
    timer.setInterval(NetStatIndicator::interval);
    timer.setSingleShot(false);
    this->updateNetStat();
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateNetStat()));
    timer.start();
}

QString NetStatIndicator::toHumanSpeed(double speed)
{
    const double kBytes = 1024;
    const double mBytes = 1024 * kBytes;
    if (speed >= mBytes) {
        return QString::number(speed / mBytes, 'f', 2) + tr(" MiB/s");
    } else if (speed >= kBytes) {
        return QString::number(speed / kBytes, 'f', 1) + tr(" KiB/s");
    } else {
        return QString::number(speed, 'f', 0) + tr(" B/s");
    }
}

QString NetStatIndicator::toDescription(double speed)
{
    const double ok_threshold = 8 * 1024;
    const double good_threshold = 32 * 1024;

    if (speed < ok_threshold) {
        return tr("网络连接尚可");
    } else if (speed < good_threshold) {
        return tr("网络连接良好");
    } else {
        return tr("网络连接通畅");
    }
}

void NetStatIndicator::updateNetStat()
{
    QTcpSocket socket;
    QUrl url(tr("http://www.baidu.com/"));
    QElapsedTimer timer;
    timer.start();
    socket.connectToHost(url.host(), 80);
    if (socket.waitForConnected(NetStatIndicator::timeout)) {
        socket.write("HEAD " + url.path().toUtf8() + " HTTP/1.1\r\n"
                     "Host: " + url.host().toUtf8() + "\r\n\r\n");
        if (socket.waitForReadyRead()) {
            QByteArray bytes = socket.readAll();
            //if (bytes.contains("200 OK")) {
            this->netspeed = bytes.size() * 1000.0 / timer.elapsed();
            this->alerted = false;
            this->netStatLabel->setStyleSheet(okStyleStr);
            this->updateUi();
            return;
            //}
        }
    }
    this->alertProperly();
}

void NetStatIndicator::updateUi()
{
    netStatLabel->setText(toDescription(netspeed));
    netspeedLabel->setText(toHumanSpeed(netspeed));
}

void NetStatIndicator::alertProperly()
{
    if (!this->alerted) {
        this->netspeedLabel->setText(tr("0 B/s"));
        this->netStatLabel->setText(tr("网络断开"));
//        QMessageBox::information(this, tr("网络连接中断"),
//                             tr("网络状态差，地图可能无法正确绘制！"), QMessageBox::Yes);
        this->netStatLabel->setStyleSheet(badStyleStr);
        alerted = true;
    }
}
