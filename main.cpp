#include "VideoListTimeLine.h"
#include <QApplication>
#include <QPair>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VideoListTimeLine w;

    QList< QPair<QTime, QTime> > videoListSegment;
    videoListSegment.append( qMakePair(QTime(7, 20, 0), QTime(10, 50, 40)) );
    videoListSegment.append( qMakePair(QTime(11, 30, 0), QTime(11, 59, 20)) );
    videoListSegment.append( qMakePair(QTime(13, 18, 0), QTime(17, 20, 35)) );
    videoListSegment.append( qMakePair(QTime(20, 20, 0), QTime(23, 50, 40)) );

    w.setVideoListTime(videoListSegment);
    w.setScaleFactor(1);
    w.show();

    return a.exec();
}
