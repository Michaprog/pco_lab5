/******************************************************************************
  \file display.h
  \author Yann Thoma
  \date 05.05.2011

  Fichier faisant partie du labo vélo.
  ****************************************************************************/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <QGraphicsView>
#include <QGraphicsItem>


class BikeItem :  public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    BikeItem();

};


class PersonItem :  public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    PersonItem();

};

class BikeDisplay : public QGraphicsView
{
    Q_OBJECT
public:
    BikeDisplay(unsigned int nbSite,QWidget *parent=0);
    unsigned int m_nbSite;
    QList<BikeItem *> *m_sites;
    QPointF *m_sitePos;
private:
    QList<BikeItem *>m_freeBikes;
    QList<BikeItem *>m_occupiedBikes;
    QGraphicsScene *m_scene;
    BikeItem *m_van;
    QList<PersonItem *> m_persons;

    BikeItem *getFreeBike();
    void setFreeBike(BikeItem *bike);

    PersonItem *getPerson(unsigned int personId);

public slots:
    void setBikes(unsigned int site,unsigned int nbBike);
    void setPerson(unsigned int site, unsigned int personID);
    void travel(unsigned int personId,unsigned int site1, unsigned int site2,unsigned int ms);
    void walk(unsigned int personId,unsigned int site1, unsigned int site2,unsigned int ms);
    void finishedAnimation();
    void vanTravel(unsigned int site1, unsigned int site2,unsigned int ms);
    void finishedVanAnimation();
};

#endif // DISPLAY_H
