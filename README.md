# 42-webserv

## Objectif

Le serveur implémente un modèle événementiel concurrent sans threads par connexion : il utilise `epoll` pour surveiller des événements sur des sockets et autres fds, et traite chaque événement de façon non bloquante. Cela permet de gérer un grand nombre de connexions un seul thread/processus.

## Concepts-clés

- Epoll : mécanisme Linux pour surveiller efficacement un grand nombre de descripteurs et savoir lesquels sont prêts pour lecture/écriture.
- Non-bloquant : chaque socket est configurée en mode non-bloquant (`O_NONBLOCK`) pour éviter que les appels `read`/`write` ne bloquent l'exécution principale.
- FdContext : structure de contexte attachée à chaque fd surveillé par `epoll`. Elle stocke les métadonnées nécessaires au traitement (type du fd, buffers, état, pointeur vers la structure plus haute (ex : `HttpConnection`), etc.).

> Remarque : quelques suppositions raisonnables sont faites (exposées plus bas) car la définition exacte de `FdContext` n'est pas fournie dans ce fichier. Le but ici est d'expliquer les usages et le pattern général.

## Vue d'ensemble de l'architecture

1. Le processus démarre et crée une socket d'écoute (listen socket).
2. La socket d'écoute est mise en non-bloquant et enregistrée auprès d'un `epoll` via `epoll_ctl` (généralement avec `EPOLLIN`).
3. La boucle principale appelle `epoll_wait` pour obtenir les événements prêts.
4. Pour chaque événement retourné :
   - le serveur récupère le `FdContext` associé (stocké dans `epoll_event.data.ptr`) ;
   - selon le type (LISTEN, CLIENT, CGI_IN, CGI_OUT, etc.), il appelle la logique correspondante : accepter la connexion, lire une requête, écrire une réponse, traiter des pipes CGI, etc.;
   - les opérations de lecture/écriture sont faites en mode non-bloquant, et par la gestion d'evenement par epoll elles ne peuvent pas renvoyer EAGAIN/EWOULDBLOCK -> de ce fait une erreur sur un send ou write est forcement une erreur "grave" et non juste un "rien a lire **pour le moment**".


## Rôle précis de `FdContext`

FdContext est la « petite fiche » attachée à chaque descripteur surveillé. Elle contient typiquement :

- le type/role (LISTEN, CLIENT, CGI_IN, CGI_OUT, etc.) ;
- le numéro de fd (dans le cas de listen ou d'un client) ;
- des pointeurs/indices vers une `HttpConnection` ou une structure de haut niveau qui gère la session (pour une cgi) ;

Pourquoi c'est utile : quand `epoll_wait` fournit un événement pour un fd, le serveur doit connaître rapidement quoi faire avec ce fd (accepter une connexion, parser une requête, compléter une écriture, lire depuis un pipe CGI). Le `FdContext` rassemble ces informations et évite des recherches lentes.

## Epoll + Non-bloquant — Pattern d'utilisation

1. Création et configuration :
   - appeler `epoll_create(1)` pour obtenir un `epoll_fd` ;
   - configurer `listen_fd` avec `fcntl(listen_fd, F_SETFL, O_NONBLOCK)` ;
   - préparer un `FdContext` pour la socket d'écoute et l'ajouter à epoll avec `EPOLLIN`.

2. Boucle d'événements :
   - `while (running) { int n = epoll_wait(epoll_fd, events, maxevents, timeout); for (i=0..n-1) handle_event(events[i]); }`

3. Traitement d'un événement :
   - obtenir `FdContext *ctx = (FdContext*)events[i].data.ptr;`
   - si ctx->type == LISTEN et event & EPOLLIN : faire `accept` dans une boucle jusqu'à EAGAIN (à cause du non-bloquant) et pour chaque `client_fd` :
     - mettre `client_fd` en non-bloquant ;
     - créer un `FdContext`/`HttpConnection` correspondant ;
     - ajouter `client_fd` à epoll avec `EPOLLIN` (et éventuellement `EPOLLET` selon stratégie).
   - si ctx->type == CLIENT et event & EPOLLIN : lire autant que possible (loop read jusqu'à EAGAIN), parser la requête HTTP (garder données incomplètes dans le buffer du contexte). Si la requête est complète, préparer la réponse et changer les flags epoll pour écouter EPOLLOUT afin d'envoyer la réponse.
   - si ctx->type == CLIENT et event & EPOLLOUT : écrire autant que possible depuis le buffer de sortie (loop write jusqu'à EAGAIN ou tout envoyé). Si tout est envoyé et que la connexion doit rester ouverte (keep-alive), repasser à EPOLLIN; sinon fermer et nettoyer.


## Exemple de cycle pour une requête client

1. Le client se connecte (listen FD notifie EPOLLIN). Le serveur `accept()` la connexion et ajoute le `client_fd` à epoll.
2. Le client envoie une requête ; `epoll_wait` rapporte EPOLLIN sur `client_fd`.
3. Le serveur lit toutes les données disponibles dans la connexion relié au fd ; si la requête est complète, il crée une `HttpRequest`, puis une `HttpResponse` et forme les headers et potentiellement le body adapté a la requete.
5. Lorsqu'EPOLLOUT arrive, le serveur écrit un petit morceau de reponse, puis attend un nouvel appel de epoll pour envoyer la suite.
6. Ensuite, selon la connexion (Keep-Alive ou Close), il attend pour une nouvelle requête ou ferme la connexion.

## Traitement des CGI / pipes

Les CGI utilisent des pipes (descripteurs supplémentaires) pour communiquer avec le processus enfant. Ces fds sont aussi inscrits dans `epoll` et ont leur propre `FdContext` (par ex. `CGI_IN`, `CGI_OUT`) :

- `CGI_IN` : on écrit dans l'entrée standard du CGI ; on surveille la possibilité d'écrire (EPOLLOUT) ;
- `CGI_OUT` : on lit la sortie du CGI (EPOLLIN) et on tasse les données dans le buffer de réponse à envoyer au client.

Le code présenté dans `Server.hpp` montre des méthodes pour ajouter/supprimer ces fds au monitoring (`addCgiInFd`, `addCgiOutFd`, `removeCgiFd`).

## A faire et a savoir

- Toujours mettre les sockets en non-bloquant avant de les ajouter à `epoll`.
- Ne pas supposer que `read`/`write` transfère toute la donnée d'un coup ; gérer offsets et buffers partiellement remplis.
- Éviter les fuites : s'assurer de faire `close(fd)` et de retirer l'inscription epoll lors d'erreurs ou de fermetures.

---

Résumé : ce serveur est structuré autour d'une boucle `epoll` + sockets non-bloquantes, et `FdContext` est la pièce centrale pour suivre l'état de chaque fd (connexion client, pipe CGI, ...).