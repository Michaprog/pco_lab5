#include <iostream>
#include <stdlib.h>

using namespace std;

#include "bikinginterface.h"
#include <QMessageBox>
#include <QThread>

bool BikingInterface::sm_didInitialize=false;
MainWindow *BikingInterface::mainWindow=0;

BikingInterface::BikingInterface()
{
    if (!sm_didInitialize) {
        cout << "Vous devez appeler BikingInteface::initialize()" << endl;
        QMessageBox::warning(0,"Erreur","Vous devez appeler "
                             "BikingInterface::initialize() avant de créer un "
                             "objet BikingInterface");
        exit(-1);
    }

    QObject::connect(this,
                     SIGNAL(sig_consoleAppendText(unsigned int,QString)),
                     mainWindow,
                     SLOT(consoleAppendText(unsigned int,QString)));
    QObject::connect(this,
                     SIGNAL(sig_setBikes(unsigned int,unsigned int)),
                     mainWindow,
                     SLOT(setBikes(unsigned int,unsigned int)));
    QObject::connect(this,
                     SIGNAL(sig_travel(unsigned int,unsigned int,unsigned int,unsigned int)),
                     mainWindow,
                     SLOT(travel(unsigned int,unsigned int,unsigned int,unsigned int)));
    QObject::connect(this,
                     SIGNAL(sig_vanTravel(unsigned int,unsigned int,
                                          unsigned int)),
                     mainWindow,
                     SLOT(vanTravel(unsigned int,unsigned int,unsigned int)));
    QObject::connect(this,
                     SIGNAL(sig_walk(unsigned int,unsigned int,unsigned int,unsigned int)),
                     mainWindow,
                     SLOT(walk(unsigned int,unsigned int,unsigned int,unsigned int)));
}


#include <QTest>

void BikingInterface::travel(unsigned int personId,unsigned int site1, unsigned int site2,
                             unsigned int ms)
{
    emit sig_travel(personId,site1,site2,ms);
    QTest::qSleep(ms);
}

void BikingInterface::walk(unsigned int personId,
                           unsigned int site1,
                           unsigned int site2,
                           unsigned int ms)
{
    emit sig_walk(personId, site1, site2, ms);
    QTest::qSleep(ms);
}

void BikingInterface::vanTravel(unsigned int site1, unsigned int site2,
                                unsigned int ms)
{
    emit sig_vanTravel(site1,site2,ms);
    QTest::qSleep(ms);
}

void BikingInterface::consoleAppendText(unsigned int consoleId,QString text) {
    emit sig_consoleAppendText(consoleId,text);
}

void BikingInterface::setBikes(unsigned int site,unsigned int nbBike) {
    emit sig_setBikes(site,nbBike);
}

void BikingInterface::setInitBikes(unsigned int site,unsigned int nbBike) {
    mainWindow->setBikes(site,nbBike);
}

void BikingInterface::setInitPerson(unsigned int site,unsigned int personID) {
    mainWindow->setPerson(site,personID);
}

void BikingInterface::initialize(unsigned int nbConsoles,unsigned int nbSites)
{
    if (sm_didInitialize) {
        cout << "Vous devez ne devriez appeler BikingInteface::initialize()"
             << " qu'une seule fois" << endl;
        QMessageBox::warning(0,"Erreur","Vous ne devriez appeler "
                             "BikingInterface::initialize() "
                             "qu'une seule fois");
        return;
    }
    mainWindow= new MainWindow(nbConsoles,nbSites,0);
    mainWindow->show();
    sm_didInitialize=true;
}
