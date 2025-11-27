/**
  \file bikinginterface.h
  \author Yann Thoma
  \date 05.05.2011

  Ce fichier contient la définition de la classe permettant d'interfacer
  l'application graphique avec les différents threads de l'application.
  */

#ifndef BIKINGINTERFACE_H
#define BIKINGINTERFACE_H

#include <QObject>

#include "mainwindow.h"

/**
  \brief Classe permettant aux threads d'interagir avec la partie graphique.

  Cette classe permet d'interagir avec la partie graphique de l'application.
  Elle utilise le concept de signaux et slots afin d'envoyer les commandes
  à l'interface graphique, et peut donc être appelée par des threads, ce qui
  ne serait pas possible sinon.

  Les commandes permettent de:
  \li afficher un message dans un parmi plusieurs consoles
  \li définir le nombre de vélos présents sur un site
  \li faire se déplacer un vélo entre deux sites
  \li faire se déplacer la camionette entre deux sites
  */
class BikingInterface : public QObject
{
    Q_OBJECT

public:

    /**
      \brief Constructeur simple.

      Une seule interface peut être partagée par plusieurs threads.
      */
    BikingInterface();

    /**
      \brief Initialisation à exécuter en début d'application.

      Fonction statique devant être appelée avant toute construction d'objet
      de type BikingInterface.
      \param nbConsoles Nombre de consoles d'affichage
      \param nbSites Nombre de sites où peuvent être trouvés les vélos
      */
    static void initialize(unsigned int nbConsoles,unsigned int nbSites);

    /**
      \brief Fonction permettant d'afficher du texte dans une console.
      \param consoleId Identifiant de la console. Attention, doit être compris
             entre 0 et nombre_de_consoles-1.
      \param text Texte à ajouter à la console.
      */
    void consoleAppendText(unsigned int consoleId,QString text);

    /**
      \brief Définition du nombre de vélos sur un site.

      Fonction permettant de définir le nombre de vélos présents sur un site.
      \param site Identifiant du site. Attention, doit être compris entre
             0 et nombre_de_sites. Le site d'identifiant nombre_de_sites
             correspond au local de maintenance.
      \param nbBike Nombre de vélos à affecter.
      */
    void setBikes(unsigned int site,unsigned int nbBike);

    /**
      \brief Définition du nombre de vélos sur un site.

      Fonction permettant de définir le nombre de vélos présents sur un site.
      Ne doit être appelé que depuis le main() avant le lancement de la
      boucle de gestion des événements.
      \param site Identifiant du site. Attention, doit être compris entre
             0 et nombre_de_sites. Le site d'identifiant nombre_de_sites
             correspond au local de maintenance.
      \param nbBike Nombre de vélos à affecter.
      */
    void setInitBikes(unsigned int site,unsigned int nbBike);


    /**
      \brief Place une personne sur un site.

      Fonction permettant de définir la position d'une personne.
      Ne doit être appelé que depuis le main() avant le lancement de la
      boucle de gestion des événements. Elle ne doit pas être appelée
      de nouveau.
      \param site Identifiant du site. Attention, doit être compris entre
             0 et nombre_de_sites. Le site d'identifiant nombre_de_sites
             correspond au local de maintenance.
      \param personID Identifiant de la personne.
      */
    void setInitPerson(unsigned int site,unsigned int personID);

    /**
      \brief Déplace un vélo d'un site à l'autre.

      Fonction permettant de visualiser le déplacement d'un vélo d'un site à
      l'autre. Le déplacement prend un certain nombre de millisecondes, et la
      fonction retourne lorsque le déplacement est terminé.
      \param personId Identifiant de la personne empruntant le vélo
      \param site1 Identifiant du site de départ. Attention, doit être compris
             entre 0 et nombre_de_sites. Le site d'identifiant nombre_de_sites
             correspond au local de maintenance.
      \param site2 Identifiant du site d'arrivée. Attention, doit être compris
             entre 0 et nombre_de_sites. Le site d'identifiant nombre_de_sites
             correspond au local de maintenance.
      \param ms Nombre de millisecondes de l'animation.
      */
    void travel(unsigned int personId,unsigned int site1, unsigned int site2,unsigned int ms);

    void walk(unsigned int personId,
              unsigned int site1,
              unsigned int site2,
              unsigned int ms);
    /**
      \brief Déplace la camionette d'un site à l'autre

      Fonction permettant de visualiser le déplacement de la camionette de
      maintenance d'un site à l'autre.
      Le déplacement prend un certain nombre de millisecondes, et la
      fonction retourne lorsque le déplacement est terminé.
      Pour une application exploitant N sites, le site numéro N correspond au
      local de maintenance. Les sites standards ont les numéros de 0 à N-1.
      \param site1 Identifiant du site de départ. Attention, doit être compris
             entre 0 et nombre_de_sites. Le site d'identifiant nombre_de_sites
             correspond au local de maintenance.
      \param site2 Identifiant du site d'arrivée. Attention, doit être compris
             entre 0 et nombre_de_sites. Le site d'identifiant nombre_de_sites
             correspond au local de maintenance.
      \param ms Nombre de millisecondes de l'animation.
     */
    void vanTravel(unsigned int site1, unsigned int site2,unsigned int ms);

private:

    //! Indique si la fonction d'initialisation a déjà été appelée
    static bool sm_didInitialize;
    //! Fenêtre principale de l'application
    static MainWindow *mainWindow;

signals:
    /**
      Signal envoyé à la fenêtre principale pour l'ajout d'un message
      \param consoleId Identifiant de la console. Attention, doit être compris
             entre 0 et nombre_de_consoles-1.
      \param text Texte à ajouter à la console.
      */
    void sig_consoleAppendText(unsigned int consoleId,QString text);

    /**
      Signal envoyé à la fenêtre principale pour la définition du nombre de
      vélos présents sur un site.
      \param site Identifiant du site. Attention, doit être compris entre
             0 et nombre_de_sites. Le site d'identifiant nombre_de_sites
             correspond au local de maintenance.
      \param nbBike Nombre de vélos à affecter.
      */
    void sig_setBikes(unsigned int site,unsigned int nbBike);

    /**
      Signal envoyé à la fenêtre principale pour déplacer un vélo d'un site à
      l'autre.
      \param personId Identifiant de la personne empruntant le vélo
      \param site1 Identifiant du site de départ. Attention, doit être compris
             entre 0 et nombre_de_sites. Le site d'identifiant nombre_de_sites
             correspond au local de maintenance.
      \param site2 Identifiant du site d'arrivée. Attention, doit être compris
             entre 0 et nombre_de_sites. Le site d'identifiant nombre_de_sites
             correspond au local de maintenance.
      \param ms Nombre de millisecondes de l'animation.
      */
    void sig_travel(unsigned int personId,unsigned int site1, unsigned int site2,unsigned int ms);

    void sig_walk(unsigned int,unsigned int,unsigned int,unsigned int);

    /**
      Signal envoyé à la fenêtre principale pour déplacer la camionette de
      maintenance d'un site à l'autre.
      \param site1 Identifiant du site de départ. Attention, doit être compris
             entre 0 et nombre_de_sites. Le site d'identifiant nombre_de_sites
             correspond au local de maintenance.
      \param site2 Identifiant du site d'arrivée. Attention, doit être compris
             entre 0 et nombre_de_sites. Le site d'identifiant nombre_de_sites
             correspond au local de maintenance.
      \param ms Nombre de millisecondes de l'animation.
      */
    void sig_vanTravel(unsigned int site1, unsigned int site2,unsigned int ms);
};

#endif // BIKINGINTERFACE_H
