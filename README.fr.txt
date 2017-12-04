
﻿Inkscape. Dessiner en toute liberté.
====================================

https://inkscape.org/

Inkscape est un logiciel de dessin vectoriel libre avec des possibilités
similaires à celles d'Illustrator, Freehand ou CorelDraw et utilisant le format
standard du W3C « Scalable Vector Graphics » (SVG). Formes basiques, chemins,
texte, marqueurs, clonage, transparence, transformations, dégradés et groupage
figurent parmi les fonctionnalités SVG supportées. De plus, Inkscape supporte
les meta-données Creative Commons, l'édition de nœuds, les calques, les
opérations complexes sur les chemins, le texte suivant un chemin, et l'édition
directe de l'arbre XML. Il peut aussi importer depuis plusieurs formats comme EPS,
Postscript, JPEG, PNG, BMP et TIFF et exporter en PNG aussi bien qu'en de
nombreux formats vectoriels.

Le but principal d'Inkscape est de fournir à la communauté du logiciel libre
un outil de dessin entièrement conforme aux spécifications XML, SVG et CSS2 du
W3C mettant en valeur un noyau léger avec des fonctionnalités puissantes ajoutées
grâce à un système d'extensions, et l'établissement de processus de
développement conviviaux, ouverts et engagés pour la communauté d'utilisateurs.


Installation standard
=====================

Depuis la version 0.92, Inkscape peut être produit avec CMake :

  mkdir build
  cd build
  cmake ..
  make
  make install

Autoconf est également toujours disponible avec Inkscape 0.92
en tant que système de production alternatif, au cas où un problème
serait rencontré avec les scripts pour CMake :

  ./autogen.sh   # Optionnel, à utiliser si le fichier configure est absent
  ./configure
  make
  make install

Des instructions spécifiques à une plateforme peuvent être trouvées
dans le fichier README associé à la plateforme (par ex. README.osx.txt
pour Mac OS X).


Dépendances nécessaires
=======================

Le noyau d'Inkscape dépend de plusieurs autres bibliothèques qu'il vous faudra
installer si elle ne sont pas encore présentes sur votre système. Typiquement,
vous devriez avoir à installer :

  * Boehm-GC
  * libsigc++
  * glibmm
  * gtkmm

Veuillez consulter http://wiki.inkscape.org/wiki/index.php/CompilingInkscape
pour les dernières dépendances, cette page comportant aussi des liens
vers les fichiers sources (au format .tar.gz).


Dépendances pour les extensions
===============================

Inkscape comporte aussi un certain nombre d'extensions apportant diverses
fonctionnalités (comme le support de formats de fichiers autres que le SVG).
En théorie, toutes les extensions sont optionnelles ; cependant, en pratique,
vous souhaiterez en disposer. Le succès de l'installation peut malheureusement
beaucoup varier. Voici quelques recommandations.

D'abord, vérifiez que Python et Perl sont bien installés. Sous Windows,
installez aussi Cygwin.

Ensuite, assurez-vous que les dépendances nécessaires à chaque extension sont
présentes. Ces dépendances varient en fonction des extensions dont vous avez
besoin ; voici les plus générales :

  * XML::Parser
  * XML::XQL

Si vous installez des dépendances dans des emplacements non standards (par ex.
si vous installez XML::Parser dans votre répertoire personnel), il vous faudra 
peut-être effectuer quelques étapes supplémentaires afin d'indiquer où ces
dépendances peuvent être trouvées. Par exemple, pour les modules Perl, il vous
faut définir les variables PERLLIB ou PERL5LIB (voyez 'man perlrun').

