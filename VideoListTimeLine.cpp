#include <QPainter>
#include <QTime>
#include <QDateTime>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QToolTip>
#include <QPair>
#include <QDateTime>
#include "VideoListTimeLine.h"

const int SECONDS_IN_DAY = 60 * 60 *24;

VideoListTimeLine::VideoListTimeLine(int height, QWidget *parent)
      : QWidget(parent)
{
    timeLineHeight = height;

    setMouseTracking(true);
    setFixedHeight(height);
    setCursor(Qt::PointingHandCursor);
}

VideoListTimeLine::~VideoListTimeLine()
{
}

void VideoListTimeLine::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    paintTimeSegment();
    paintTimeRuler();
}

void VideoListTimeLine::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    //setCursor(Qt::PointingHandCursor);
}

void VideoListTimeLine::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    //setCursor(Qt::ArrowCursor);
}

void VideoListTimeLine::mouseMoveEvent(QMouseEvent *event)
{
    if ( true == mouseRightButtonClicked && false == mousePressedPosition.isNull())
    {
        int moveWidth = event->pos().x() - mousePressedPosition.x();
        startPosX += event->pos().x() - mousePressedPosition.x();
        if ( rightHiddenWidth + moveWidth > rightHiddenMaxWidth)
        {
            startPosX = 0;
            rightHiddenWidth = rightHiddenMaxWidth;
        }
        else if ( rightHiddenWidth + moveWidth < rightHiddenMiniWidth)
        {
            startPosX = -rightHiddenMaxWidth;
            rightHiddenWidth = 0;
        }
        else
        {
            rightHiddenWidth += event->pos().x() - mousePressedPosition.x();
        }
        mousePressedPosition = event->pos();
        update();
    }
    else
    {
        double x = event->pos().x();
        int curSec = static_cast<int>( (x - startPosX) / timeLineWidth * SECONDS_IN_DAY );
        QTime curTime(0, 0, 0);
        curDate = QDate::currentDate();
        QDateTime curDateTime(curDate, curTime.addSecs(curSec));
        QToolTip::showText(event->globalPos(), curDateTime.toString("yyyy.MM.dd-hh:mm:ss"), this);
    }
}

void VideoListTimeLine::mouseDoubleClickEvent(QMouseEvent *event)
{
    double x = event->pos().x();
    int curSec = static_cast<int>( (x - startPosX) / timeLineWidth * SECONDS_IN_DAY);
    QTime curTime(0, 0, 0);
    qDebug() << curTime.addSecs(curSec);
}

void VideoListTimeLine::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    timeLineWidth = this->width() * scaleFactor;
    startPosX = 0;
    rightHiddenWidth = timeLineWidth - this->width();
    rightHiddenMiniWidth = 0;
    rightHiddenMaxWidth = timeLineWidth - this->width();
}

void VideoListTimeLine::setVideoListTime(QList< QPair<QTime, QTime> > recordList)
{
    videoList.clear();
    for (int i=0; i<recordList.size(); i++)
    {
        /* 剔除无效数据 */
        if ( recordList.at(i).first < recordList.at(i).second)
        {
            videoList.append(recordList.at(i));
        }
    }
}

/* 将录像的开始时间和结束时间转换为对应的Rect，用于稍后绘制 */
QList<QRectF> VideoListTimeLine::convertTimeSegmentToRect()
{
    int beginY = 0;
    int endY = timeLineHeight;
    int width = timeLineWidth;

    QList<QRectF> timeRectSegment;
    for (int i=0; i< videoList.size(); i++)
    {
        double beginSec = videoList.at(i).first.msecsSinceStartOfDay() / 1000;
        double beginX = beginSec / SECONDS_IN_DAY * width;
        QPointF beginPoint = QPointF(startPosX + beginX, beginY);

        double endSec = videoList.at(i).second.msecsSinceStartOfDay() / 1000;
        double endX = endSec / SECONDS_IN_DAY * width;
        QPointF endPoint = QPointF(startPosX + endX, endY);

        timeRectSegment.append(QRectF(beginPoint, endPoint));
    }

    return timeRectSegment;
}

void VideoListTimeLine::paintTimeSegment()
{
    QList<QRectF> timeRectList = convertTimeSegmentToRect();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    /* 绘制背景 */
    painter.setPen(QPen(QColor(108, 108, 108), 0));
    painter.setBrush(QColor(108, 108, 108));
    /* 背景颜色始终是控件的区域大小，不需要添加偏移量 */
    //painter.drawRect(startPosX + 0 ,0, startPosX + timeLineWidth, this->height());
    painter.drawRect( 0 ,0, timeLineWidth, this->height());

    /* 绘制前景 */
    painter.setPen(QPen(QColor(102, 163, 52), 0));
    painter.setBrush(QColor(102, 163, 52));
    for (int i=0; i<timeRectList.size(); i++)
    {
        QRectF rect = timeRectList.at(i);
        painter.drawRect(rect);
    }
}

void VideoListTimeLine::paintTimeRuler()
{
    /* 小时刻度默认一定有 */
    paintOneHourRuler();

    if ( true == _30minutesRulerFlag)
        paintHalfHourRuler();
    if ( true == _15minutesRulerFlag)
        paint15minutesRuler();
    if ( true == _5minutesRulerFlag)
        paint5minutesRuler();
    if (true == _1minuteRulerFlag)
        paint1minuteRuler();
}

void VideoListTimeLine::mousePressEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::RightButton)
    {
        mouseRightButtonClicked = true;
        mousePressedPosition = event->pos();
    }
}

void VideoListTimeLine::mouseReleaseEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::RightButton)
    {
        mouseRightButtonClicked = false;
        mousePressedPosition = QPoint();
    }
}

void VideoListTimeLine::setTimeRulerFlag()
{
    _30minutesRulerFlag = false;
    _15minutesRulerFlag = false;
    _5minutesRulerFlag = false;
    _1minuteRulerFlag = false;

    switch (scaleFactor)
    {
        case 1:
            _30minutesRulerFlag = true;
            break;
        case 2:
            _30minutesRulerFlag = true;
            _15minutesRulerFlag = true;
            break;
        case 4:
            _30minutesRulerFlag = true;
            _5minutesRulerFlag = true;
            break;
        case 8:
            _30minutesRulerFlag = true;
            _1minuteRulerFlag = true;
            break;
        default:
            _30minutesRulerFlag = true;
            break;
    }
}

void VideoListTimeLine::setScaleFactor(int scaleFactor)
{
    scaleFactor = (scaleFactor >= 8) ? 8 : scaleFactor;
    this->scaleFactor = scaleFactor;
    setTimeRulerFlag();

    timeLineWidth = this->width() * scaleFactor;
    startPosX = 0;
    rightHiddenWidth = timeLineHeight - this->width();
    rightHiddenMiniWidth = 0;
    rightHiddenMaxWidth = timeLineHeight - this->width();

    update();
}

void VideoListTimeLine::paintOneHourRuler()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QFont font = painter.font();
    font.setPixelSize(15);
    painter.setFont(font);
    painter.setPen(QPen(QColor(255, 255, 255), 2));

    double hourWidth = timeLineWidth / 24.0;
    double halfHourWidth = timeLineWidth / 48.0;
    int hourRulerBeginY = this->height() * 3 / 5;
    int hourRulerEndY = this->height();

    for (int i=1; i<24; i++)
    {
        double hourRulerX = hourWidth * i;
        painter.drawLine(QPointF(startPosX + hourRulerX, hourRulerBeginY),
                         QPointF(startPosX + hourRulerX, hourRulerEndY));

        QString strHour = QString("%1").arg(i, 2, 10, QChar('0'));
        QPoint  topLeftPoint = QPoint( static_cast<int>(startPosX + hourRulerX - halfHourWidth), 0);
        QPoint  rightBottomPoint = QPoint( static_cast<int>(startPosX + hourRulerX + halfHourWidth), hourRulerBeginY);
        QRect   strRect = QRect(topLeftPoint, rightBottomPoint);
        painter.drawText(strRect, Qt::AlignCenter, strHour);
    }
}

void VideoListTimeLine::paintHalfHourRuler()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor(255, 255, 255), 2));

    double hourWidth = timeLineWidth / 24.0;
    double halfHourWidth = timeLineWidth / 48.0;
    int halfhourRulerBeginY = this->height() * 4 / 5 - 1;
    int halfHourRulerEndY = this->height();

    for (int i=0; i<24; i++)
    {
        double halfHourRulerX = halfHourWidth + hourWidth * (i);
        painter.drawLine(QPointF(startPosX + halfHourRulerX, halfhourRulerBeginY),
                         QPointF(startPosX + halfHourRulerX, halfHourRulerEndY));
    }
}

void VideoListTimeLine::paint15minutesRuler()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor(255, 255, 255), 2));

    double _15minutesWidth = timeLineWidth / 96.0;
    int _15minutesRulerBeginY = this->height() * 4 / 5 + 2;
    int _15minutesRulerEndY = this->height();

    for (int i=0; i<96; i++)
    {
        double _15minutesRulerX = _15minutesWidth + _15minutesWidth * (i);
        painter.drawLine(QPointF(startPosX + _15minutesRulerX, _15minutesRulerBeginY),
                         QPointF(startPosX + _15minutesRulerX, _15minutesRulerEndY));
    }
}

void VideoListTimeLine::paint5minutesRuler()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor(255, 255, 255), 2));

    double _5minutesWidth = timeLineWidth / 288.0;
    int _5minutesRulerBeginY = this->height() * 4 / 5 + 2;
    int _5minutesRulerEndY = this->height();

    for (int i=0; i<288; i++)
    {
        double _5minutesRulerX = _5minutesWidth + _5minutesWidth * (i);
        painter.drawLine(QPointF(startPosX + _5minutesRulerX, _5minutesRulerBeginY),
                         QPointF(startPosX + _5minutesRulerX, _5minutesRulerEndY));
    }
}

void VideoListTimeLine::paint1minuteRuler()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor(255, 255, 255), 2));

    double _1minuteWidth = timeLineWidth / 1440.0;
    int _1minuteRulerBeginY = this->height() * 4 / 5 + 2;
    int _1minuteRulerEndY = this->height();

    for (int i=0; i<1440; i++)
    {
        double _1minuteRulerX = _1minuteWidth + _1minuteWidth * (i);
        painter.drawLine(QPointF(startPosX + _1minuteRulerX, _1minuteRulerBeginY),
                         QPointF(startPosX + _1minuteRulerX, _1minuteRulerEndY));
    }
}
