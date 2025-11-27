/******************************************************************************
  \file display.cpp
  \author Yann Thoma
  \date 05.05.2011

  Fichier faisant partie du labo vélo.
  ****************************************************************************/

#include "display.h"

#include <QPaintEvent>
#include <QPainter>

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QEventLoop>
#include <QMutex>


#include <cmath>

#define RADIUS 250.0
#define SCENEOFFSET 50.0
#define SITERADIUS 25.0
#define BIKEWIDTH 30.0

#define VANWIDTH 50.0

#define NBPERSONICONS 30

BikeItem::BikeItem() = default;

PersonItem::PersonItem() = default;

BikeDisplay::BikeDisplay(unsigned int nbSite,QWidget *parent):
    QGraphicsView(parent)
{
    m_sitePos=new QPointF[nbSite+1];
    for(unsigned int i=0;i<nbSite;i++)
    {
        m_sitePos[i]=
                QPointF(SCENEOFFSET+RADIUS+RADIUS*cos(2.0*3.14/((float)nbSite)
                                                      *((float)i)),
                        SCENEOFFSET+RADIUS+RADIUS*sin(2.0*3.14/((float)nbSite)
                                                      *((float)i)));
    }
    m_sitePos[nbSite]=
            QPointF(SCENEOFFSET+RADIUS,
                    SCENEOFFSET+RADIUS);
    m_scene=new QGraphicsScene(this);
    this->setRenderHints(QPainter::Antialiasing |
                         QPainter::SmoothPixmapTransform);
    this->setMinimumHeight(2*SCENEOFFSET+2*RADIUS+10.0);
    this->setMinimumWidth(2*SCENEOFFSET+2*RADIUS+10.0);
    m_scene->setSceneRect(0,0,2*SCENEOFFSET+2*RADIUS,2*SCENEOFFSET+2*RADIUS);
    this->setScene(m_scene);
    m_nbSite=nbSite;

    QPen pen;
    QBrush brush(QColor(100,255,100));
    for(unsigned int i=0;i<nbSite;i++) {
        m_scene->addEllipse(m_sitePos[i].x()-SITERADIUS,
                            m_sitePos[i].y()-SITERADIUS,
                            2*SITERADIUS,
                            2*SITERADIUS,
                            pen,brush);
    }
    brush=QBrush(QColor(255,100,100));
    m_scene->addEllipse(m_sitePos[nbSite].x()-SITERADIUS,
                        m_sitePos[nbSite].y()-SITERADIUS,
                        2*SITERADIUS,
                        2*SITERADIUS,
                        pen,brush);
    m_sites=new QList<BikeItem*>[nbSite+1];

    QPixmap img("images/camionette.png");
    QPixmap vanPixmap;
    vanPixmap=img.scaledToWidth(VANWIDTH);
    m_van=new BikeItem();
    m_van->setPixmap(vanPixmap);
    m_scene->addItem(m_van);
    m_van->setPos(m_sitePos[nbSite]);
}


BikeItem *BikeDisplay::getFreeBike()
{
    if (m_freeBikes.count()>0)
    {

        BikeItem *bike=m_freeBikes.first();
        m_freeBikes.removeFirst();
        return bike;
    }
    else {
        QPixmap img("images/velo.png");
        QPixmap bikePixmap;
        bikePixmap=img.scaledToWidth(BIKEWIDTH);
        auto *bike=new BikeItem();
        bike->setPixmap(bikePixmap);
        m_scene->addItem(bike);
        bike->hide();
        return bike;
    }
}

void BikeDisplay::setFreeBike(BikeItem *bike)
{
    m_freeBikes << bike;
}


void BikeDisplay::vanTravel(unsigned int site1,unsigned int site2,
                            unsigned int ms)
{
    static QMutex mutex;
    mutex.lock();
    m_van->show();
    auto *animation=new QPropertyAnimation(m_van, "pos");
    animation->setDuration(ms-10);
    animation->setStartValue(m_sitePos[site1]-QPointF(VANWIDTH/2,VANWIDTH/2));
    animation->setEndValue(m_sitePos[site2]-QPointF(VANWIDTH/2,VANWIDTH/2));
    animation->start();

    QObject::connect(animation, SIGNAL(finished()), this,
                     SLOT(finishedVanAnimation()));

    mutex.unlock();
}

void BikeDisplay::walk(unsigned int personId,
                       unsigned int site1,
                       unsigned int site2,
                       unsigned int ms)
{
    static QMutex mutex;
    mutex.lock();

    auto *group = new QParallelAnimationGroup(this);

    {
        PersonItem *person = getPerson(personId);
        person->show();
        auto *animation = new QPropertyAnimation(person, "pos");
        animation->setDuration(ms - 10);
        animation->setStartValue(m_sitePos[site1] -
                                 QPointF(BIKEWIDTH/2, BIKEWIDTH*1.2));
        animation->setEndValue(m_sitePos[site2] -
                               QPointF(BIKEWIDTH/2, BIKEWIDTH*1.2));
        group->addAnimation(animation);
    }

    group->start();

    QObject::connect(group, SIGNAL(finished()), this,
                     SLOT(finishedAnimation()));

    mutex.unlock();
}


void BikeDisplay::finishedVanAnimation()
{
    auto *animation=dynamic_cast<QPropertyAnimation*>(sender());
    delete animation;
}

void BikeDisplay::setBikes(unsigned int site,unsigned int nbBike)
{
    if (site>m_nbSite)
        return;
    BikeItem *bike = nullptr;
    while ((m_sites[site].count()>0)&&(m_sites[site].count()>(int)nbBike)) {
        bike=getFreeBike();BikeItem *bike=m_sites[site].first();
        m_sites[site].removeFirst();
        setFreeBike(bike);
        bike->hide();
    }
    while(m_sites[site].count()<(int)nbBike) {
        bike=getFreeBike();
        m_sites[site] << bike;
        bike->show();
    }
    for(int i=0;i<m_sites[site].count();i++) {
        m_sites[site].at(i)->setPos(m_sitePos[site]-
                                    QPointF(BIKEWIDTH/2-i*10.0,
                                            BIKEWIDTH/2+i*10.0));
    }
}

void BikeDisplay::setPerson(unsigned int site, unsigned int personID)
{
    PersonItem *person = getPerson(personID);
    QPointF curPos = m_sitePos[site];
    float angle = rand();
    person->setPos(curPos.x() + 40*cos(angle),curPos.y() + 40*sin(angle));
    person->show();
}


PersonItem *BikeDisplay::getPerson(unsigned int personId)
{
    while ((unsigned int)(m_persons.size()) <= personId)
    {
        QPixmap img(QString("images/32x32/p%1.png").arg(m_persons.size() % NBPERSONICONS));
        QPixmap personPixmap;
        personPixmap=img.scaledToWidth(BIKEWIDTH);
        auto *person=new PersonItem();
        person->setPixmap(personPixmap);
        m_scene->addItem(person);
        m_persons.append(person);
        person->hide();
    }
    return m_persons.at(personId);
}


void BikeDisplay::travel(unsigned int personId,unsigned int site1, unsigned int site2,unsigned int ms)
{
    static QMutex mutex;
    mutex.lock();


    auto *group=new QParallelAnimationGroup(this);

    {
        BikeItem *bike=getFreeBike();
        bike->show();
        auto *animation=new QPropertyAnimation(bike, "pos");
        animation->setDuration(ms-10);
        animation->setStartValue(m_sitePos[site1]-
                                 QPointF(BIKEWIDTH/2,BIKEWIDTH/2));
        animation->setEndValue(m_sitePos[site2]-
                               QPointF(BIKEWIDTH/2,BIKEWIDTH/2));

        group->addAnimation(animation);
    }
    {
        PersonItem *person=getPerson(personId);
        person->show();
        auto *animation=new QPropertyAnimation(person, "pos");
        animation->setDuration(ms-10);
        animation->setStartValue(m_sitePos[site1]-
                                 QPointF(BIKEWIDTH/2,BIKEWIDTH*1.2));
        animation->setEndValue(m_sitePos[site2]-
                               QPointF(BIKEWIDTH/2,BIKEWIDTH*1.2));

        group->addAnimation(animation);
    }

    group->start();

    QObject::connect(group, SIGNAL(finished()), this,
                     SLOT(finishedAnimation()));

    mutex.unlock();
}

void BikeDisplay::finishedAnimation()
{
    auto *group=dynamic_cast<QParallelAnimationGroup*>(sender());
    for(int i=0;i<group->animationCount();i++) {
        auto *animation=dynamic_cast<QPropertyAnimation*>(group->animationAt(i));
        auto *bike=dynamic_cast<BikeItem*>(animation->targetObject());
        if (bike) {
            bike->hide();
            setFreeBike(bike);
        }
        auto *person=dynamic_cast<PersonItem*>(animation->targetObject());
        if (person) {
            QPointF curPos = person->pos()+ QPointF(BIKEWIDTH/2,BIKEWIDTH*1.2);
            float angle = rand();
            person->setPos(curPos.x() + 40*cos(angle),curPos.y() + 40*sin(angle));
        }
    }
    delete group;
}


