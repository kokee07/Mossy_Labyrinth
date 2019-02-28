# Mossy_Labyrinth
Repositorio para el juego en OpenGL

#Contexto
El juego trata de un desafortunado buscatesoros que durante una excursión cae dentro de un antiguo laberinto, el cual está resguardado por 4 antiguos guardianes que juraron detener a los codiciosos ladrones que cayeran en su templo.
Los 4 guardianes poseen tanto poder que ellos mismos se pelean por quién será el encargado de detener al intruso, por lo que ante cada turno sólo habra un guardían activo (con textura distinta).

#Rol
El jugador cumple el rol del buscatesoros y su deber es escapar del laberinto evadiendo los intentos de cada guardian de impedirlo.

#Jugabilidad
El jugador sólo puede mover 1 casilla a la vez, siguiendo las teclas habituales de [w,a,s,d]. Ante cada movimiento del jugador un guardian creará una caja que impide el paso. 
## ¿Dónde caerá cada caja? 
Eso depende de qué guardián tenga el poder en ese momento, cayendo siempre del lado contrario del guardián.
Es decir, si el guardian de arriba está activo, el próximo bloque caerá siempre al sur del jugador. Si el activo es el de la derecha, entonces el bloque caerá a la izquierda del jugador.

## ¿Desafío? 
Lograr evitar ser encerrado por las cajas, y llegar ileso a la casilla roja del nivel, la cual puede estar en 3 posiciones. La dificultad está en preveer los movimientos.

## ¿Qué pasa si quedo encerrado?
Los guardianes te permiten comenzar nuevamente la travesía por los pasillos al presionar la tecla "RePag" del teclado. Lo mismo podrás hacer una vez termines la partida y quieras jugar nuevamente.

## Problemas
Lamentamos la falta de menú e instrucciones in-game pero no pudimos hacerlo a tiempo. Enjoy it~



