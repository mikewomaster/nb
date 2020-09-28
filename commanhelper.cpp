#include "commanhelper.h"

#include <QTime>
#include <QApplication>

CommanHelper::CommanHelper()
{

}

void CommanHelper::sleep(unsigned msec)
{
    QTime reachTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < reachTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
