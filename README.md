# Projet Système et Réseau

L'objectif du projet est de mettre en place une architecture type "botnet" afin de contrôler et déployer à distance des tâches sur différentes machines.  

## Pour commencer

Vous trouverez tous les fichiers et dossiers liés à ce projet sur ce dépôt git. Clonez l'ensemble de notre répertoire à l'aide de la commande ``git clone https://archives.plil.fr/egury/botnetFauchoisGury.git`` pour passer à la suite.

Présentons rapidement les différents composants du projet et leur fonctionnement :

Tout d'abord, au coeur du projet se trouve le Command & Conquer (**candc/**) qui va établir la communication avec le ou les bots. Le C&C utilise le protocole TCP pour envoyer des ordres et des charges utiles aux différents bots. Les bots (**bots/**), quant à eux, utilisent le protocole UDP pour diffuser (régulièrement) leur nom et leur état. Les protocoles TCP et UDP étant regroupés dans une même librairie (**network_lib/**).

Afin d'intéragir avec le C&C, nous mettons en place deux méthodes d'administration : un administrateur local et un administrateur distant. L'administrateur local (**admin/**) est exécuté sur la même machine que le C&C et communique (pour envoyer les ordres) avec celui-ci à l'aide de files de messages IPC (**ipc_lib/**). L'administrateur distant (**webserver/**) est réalisé à partir d'un serveur web et communique avec le C&C à l'aide de signaux et de segments de mémoire partagée (**ipc_lib/**).

Concernant les tâches à réaliser, elles sont envoyées par le C&C au(x) bot(s) sous la forme de bibliothèques dynamiques comportant une fonction ``start()`` (**dynamic_lib/**).

Chaque tâche est effectuée dans un thread indépendant (**thread_lib/**).

## Pré-requis

Avant de démarrer le programme il est nécessaire de lancer la commande ``make`` en étant placé dans le répertoire principal (botnetFauchoisGury/) pour compiler l'ensemble du projet.

## Démarrage

Il n'y a pas d'importance d'ordre de lancement entre le bot et le C&C, en revanche il est nécessaire de lancer ce dernier avant l'admin.

### Bot

Depuis le répertoire **bots/**, un simple ``./bot`` permet de lancer notre bot.

### C&C

Depuis le répertoire **candc/**, un simple ``./candc`` permet de lancer notre Command & Conquer.

### Admin

Depuis le répertoire **admin/**, la commande ``./admin`` ou ``./admin HELP`` vous donnera la liste de toutes les commandes possibles d'effectuer.

À chaque saisie de commande, l'administrateur vous invitera à renseigner les informations requises pour chaque tâche.

En cas d'erreur de saisie ou d'information(s) erronée(s), le C&C enverra à l'admin le problème rencontré et vous l'affichera.

Pour envoyer une charge utile sur un bot, il est nécessaire que le .so se trouve dans le dossier **dynamic_lib/**

Exemple de commande :

    ./admin INSTALL
    
    192.168.XX.XX
    
    libcharge.so

### Serveur Web

Pour commander les bots à distance, nous avons mis en place un serveur web (sur le port TCP 4040). Il est nécessaire de se rendre dans le répertoire **webserver/** et d'exécuter la commande ``./server`` en ayant préalablement lancé le C&C.

Une fois le serveur démarré, en vous rendant simplement sur votre navigateur web, vous aurez accès l'interface web permettant l'envoi des différentes commandes de deux façons :
* Si le serveur est hébérgé sur votre machine : [127.0.0.1:4040/form.html](https://127.0.0.1:4040/form.html) ou  [localhost:4040/form.html](https://localhost:4040/form.html).
* Si le serveur est hébergé sur une machine différentes de celle sur laquelle vous voulez accéder au formulaire : [192.168.XX.XX:4040/form.html](https://192.168.XX.XX:4040/form.html).

Vous n'aurez alors qu'à compléter les différentes rubriques de la page web en renseignant les bonnes informations pour chaque tâche. Le site vous retournera le résultat de chaque commande, en cas d'erreur il vous signalera également quel est le problème.

## Avancée du projet

En l'état nous sommes capable de :
- Récupérer la liste des bots actifs avec leur ID, adresse IP et temps de vie (la liste est mise à jour toutes les 10s),
- Installer une charge utile sur un bot,
- Demander l'execution d'une charge utile sur un bot,
- Demander le resultat de l'exécution d'une charge sur un bot,
- Désinstaller une charge utile d'un bot,
- Se déconnecter d'un bot,
- Demander le statut d'un bot (temps de vie, nombre de charges utiles installées, nombre d'executions).

Toute ces tâches peuvent être réalisé depuis l'admin ou depuis le serveur web.

Sur le bot, chaque connexion est gérée dans un thread, de ce fait le bot est capable de gérer plusieurs requêtes en parallèle tout en continuant de se signaler en UDP toutes les secondes.

Le C&C est capable de gérer l'UDP et le TCP en parallèle.

## Fabriqué avec

**VIM & Atom** - Editeurs de textes

## Auteurs

* **Evan Gury**
* **Lukas Fauchois**