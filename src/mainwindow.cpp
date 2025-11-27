/******************************************************************************
  \file mainwindow.cpp
  \author Yann Thoma
  \date 05.05.2011

  Fichier faisant partie du labo vélo.
  ****************************************************************************/

#include <QToolBar>
#include <QAction>
#include <QCoreApplication>
#include "mainwindow.h"

#define min(a,b) ((a<b)?(a):(b))

extern std::array<BikeStation*, NB_SITES_TOTAL>* globalStations;

extern void stopSimulation();

MainWindow::MainWindow(unsigned int nbConsoles,unsigned int nbSite,
                       QWidget *parent)
    : QMainWindow(parent)
{
    m_nbConsoles=nbConsoles;
    m_consoles=new QTextEdit*[nbConsoles];
    for(unsigned int i=0;i<nbConsoles;i++) {
        m_consoles[i]=new QTextEdit(this);
        m_consoles[i]->setMinimumWidth(200);
    }
    m_docks=new QDockWidget*[nbConsoles];
    for(unsigned int i=0;i<nbConsoles;i++) {
        m_docks[i]=new QDockWidget(this);
        m_docks[i]->setWidget(m_consoles[i]);
    }
    for(unsigned int i=0;i<nbConsoles/2;i++) {
        this->addDockWidget(Qt::LeftDockWidgetArea,m_docks[i]);
    }
    for(unsigned int i=nbConsoles/2;i<nbConsoles;i++) {
        this->addDockWidget(Qt::RightDockWidgetArea,m_docks[i]);
    }

    for(unsigned int i=0;i<nbConsoles;i++)
        setConsoleTitle(i,QString("Console number : %1").arg(i));
    m_display=new BikeDisplay(nbSite,this);
    setCentralWidget(m_display);

    QToolBar* toolbar = addToolBar("Controls");

    QAction* stopAction = toolbar->addAction("Stop simulation");
    connect(stopAction, &QAction::triggered,
            this, &MainWindow::onStopClicked);

    QAction* closeAction = toolbar->addAction("Close simulation");
    connect(closeAction, &QAction::triggered,
            this, &MainWindow::onEndClicked);

    QAction* plusDepot = toolbar->addAction("+1 depot");
    connect(plusDepot, &QAction::triggered,
            this, &MainWindow::onDepotPlusClicked);

    QAction* minusDepot = toolbar->addAction("-1 depot");
    connect(minusDepot, &QAction::triggered,
            this, &MainWindow::onDepotMinusClicked);
}

void MainWindow::onEndClicked()
{
    if (!m_stopped) {
        return;
    }
    QCoreApplication::quit();
}

void MainWindow::onStopClicked()
{
    stopSimulation();
    if (!m_stopped) {
        m_stopped = true;
    }
}

void MainWindow::onDepotPlusClicked()
{
    if (!globalStations) return;

    BikeStation* depot = (*globalStations)[DEPOT_ID];

    // Create a new bike and add it to the depot
    auto* bike = new Bike;
    static thread_local std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, Bike::nbBikeTypes - 1);
    bike->bikeType = dist(rng);

    depot->putBike(bike);

    // Update GUI directly
    m_display->setBikes(DEPOT_ID, depot->nbBikes());
}

void MainWindow::onDepotMinusClicked()
{
    if (!globalStations) return;

    BikeStation* depot = (*globalStations)[DEPOT_ID];

    // Try to remove one bike from depot
    auto bikes = depot->getBikes(1);
    if (!bikes.empty()) {
        delete bikes[0]; // bike is no longer in any station, we can delete it
    }

    // Update GUI
    m_display->setBikes(DEPOT_ID, depot->nbBikes());
}

void MainWindow::walk(unsigned int personId,
                      unsigned int site1,
                      unsigned int site2,
                      unsigned int ms)
{
    m_display->walk(personId, site1, site2, ms);
}


void MainWindow::setConsoleTitle(unsigned int consoleId,QString title)
{
    if (consoleId>=m_nbConsoles)
        return;
    m_docks[consoleId]->setWindowTitle(title);
}

void MainWindow::consoleAppendText(unsigned int consoleId,QString text)
{
    if (consoleId>=m_nbConsoles)
        return;
    m_consoles[consoleId]->append(text);
}


void MainWindow::setBikes(unsigned int site,unsigned int nbBike)
{
    m_display->setBikes(site,nbBike);
}


void MainWindow::setPerson(unsigned int site, unsigned int personID)
{
    m_display->setPerson(site,personID);
}

void MainWindow::travel(unsigned int personId,unsigned int site1, unsigned int site2,unsigned int ms)
{
    m_display->travel(personId,site1,site2,ms);
}

void MainWindow::vanTravel(unsigned int site1, unsigned int site2,
                           unsigned int ms)
{
    m_display->vanTravel(site1,site2,ms);
}

MainWindow::~MainWindow() = default;
