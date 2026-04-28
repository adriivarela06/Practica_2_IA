#include "ingeniero.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>
#include <cmath>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoIngeniero::think(Sensores sensores)
{
  Action accion = IDLE;

  // Decisión del agente según el nivel
  switch (sensores.nivel)
  {
  case 0:
    accion = ComportamientoIngenieroNivel_0(sensores);
    break;
  case 1:
    accion = ComportamientoIngenieroNivel_1(sensores);
    break;
  case 2:
    accion = ComportamientoIngenieroNivel_2(sensores);
    break;
  case 3:
    accion = ComportamientoIngenieroNivel_3(sensores);
    break;
  case 4:
    accion = ComportamientoIngenieroNivel_4(sensores);
    break;
  case 5:
    accion = ComportamientoIngenieroNivel_5(sensores);
    break;
  case 6:
    accion = ComportamientoIngenieroNivel_6(sensores);
    break;
  }

  return accion;
}

static void Coordenadas(Sensores sensores, int &fI, int &cI, int &fC, int &cC, int &fD, int &cD){
   switch (sensores.rumbo) {
      case norte:
         fI = sensores.posF - 1; cI = sensores.posC - 1;
         fC = sensores.posF - 1; cC = sensores.posC;
         fD = sensores.posF - 1; cD = sensores.posC + 1;
         break;
      case noreste:
         fI = sensores.posF - 1; cI = sensores.posC;
         fC = sensores.posF - 1; cC = sensores.posC + 1;
         fD = sensores.posF;     cD = sensores.posC + 1;
         break;
      case este:
         fI = sensores.posF - 1; cI = sensores.posC + 1;
         fC = sensores.posF;     cC = sensores.posC + 1;
         fD = sensores.posF + 1; cD = sensores.posC + 1;
         break;
      case sureste:
         fI = sensores.posF;     cI = sensores.posC + 1;
         fC = sensores.posF + 1; cC = sensores.posC + 1;
         fD = sensores.posF + 1; cD = sensores.posC;
         break;
      case sur:
         fI = sensores.posF + 1; cI = sensores.posC + 1;
         fC = sensores.posF + 1; cC = sensores.posC;
         fD = sensores.posF + 1; cD = sensores.posC - 1;
         break;
      case suroeste:
         fI = sensores.posF + 1; cI = sensores.posC;
         fC = sensores.posF + 1; cC = sensores.posC - 1;
         fD = sensores.posF;     cD = sensores.posC - 1;
         break;
      case oeste:
         fI = sensores.posF + 1; cI = sensores.posC - 1;
         fC = sensores.posF;     cC = sensores.posC - 1;
         fD = sensores.posF - 1; cD = sensores.posC - 1;
         break;
      case noroeste:
         fI = sensores.posF;     cI = sensores.posC - 1;
         fC = sensores.posF - 1; cC = sensores.posC - 1;
         fD = sensores.posF - 1; cD = sensores.posC;
         break;
   }
}

/**
 * @brief Determina la mejor opcion entre las 3 casillas que tiene delante
 * 
 * @param i terreno que hay en la posicion 1 de la superficie
 * @param c terreno que hay en la posicion 2 de la superficie
 * @param d terreno que hay en la posicion 3 de la superficie
 * @return 2 si es mejor WALK, 1 para TURN_SL y 3 para TURN_SR, 0 si no hay nada interesante
 */
int VeoCasillaInteresanteI0(char i, char c, char d, bool zapatillas, int calorI, int calorC, int calorD){
   if(c == 'U') return 2;
   else if(i == 'U') return 1;
   else if(d == 'U') return 3;

   if(!zapatillas){
      if(c == 'D') return 2;
      else if(i == 'D') return 1;
      else if(d == 'D') return 3;
   }

   bool viableI = (i == 'C' || i == 'D');
   bool viableC = (c == 'C' || c == 'D');
   bool viableD = (d == 'C' || d == 'D');

   if (!viableI && !viableC && !viableD) return 0;

   int mejorOpcion = 0;
   int menorCalor = 9999;

   if (viableC && calorC <= menorCalor) { 
      menorCalor = calorC; 
      mejorOpcion = 2; 
   }
   
   if (viableI && calorI < menorCalor) { 
      menorCalor = calorI; 
      mejorOpcion = 1; 
   }
   
   if (viableD && calorD < menorCalor) { 
      menorCalor = calorD; 
      mejorOpcion = 3; 
   }

   return mejorOpcion;
}

/**
 * @brief Determina si la casilla es viable por altura.
 * @param casilla tipo de terreno
 * @param dif diferencia de altura entre casillas
 * @param zapatillas indica si tengo las zapatillas
 * @return 'P' si no es accesible por altura y casilla en otro caso
 */
char ViablePorAlturaI0(char casilla, int dif, bool zapatillas){
   if(abs(dif) <= 1 || (zapatillas && abs(dif) <= 2))
      return casilla;
   else
      return 'P';
}

// Niveles iniciales (Comportamientos reactivos simples)
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores)
{
   Action accion = IDLE;
   ActualizarMapa(sensores);

   if(sensores.superficie[0] == 'U') return IDLE;

   if(sensores.superficie[0] == 'D' && !zapatillas) zapatillas = true;

   if(accion_anterior == WALK) mapaCalor[{sensores.posF, sensores.posC}]++;


   char i = ViablePorAlturaI0(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], zapatillas);
   char c = ViablePorAlturaI0(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], zapatillas);
   char d = ViablePorAlturaI0(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], zapatillas);

   if (sensores.agentes[1] != '_') i = 'P';
   if (sensores.agentes[2] != '_') c = 'P';
   if (sensores.agentes[3] != '_') d = 'P';

   int fI, cI, fC, cC, fD, cD;

   Coordenadas(sensores, fI, cI, fC, cC, fD, cD);

   int calorI = mapaCalor[{fI, cI}];
   int calorC = mapaCalor[{fC, cC}];
   int calorD = mapaCalor[{fD, cD}];

   int pos = VeoCasillaInteresanteI0(i, c, d, zapatillas, calorI, calorC, calorD);

   switch (pos)
   {
   case 2:
      accion = WALK;
      break;
   case 1:
      accion = TURN_SL;
      break;
   case 3:
      accion = TURN_SR;
      break;
   default:
      accion = TURN_SL;
      break;
   }

   accion_anterior = accion;
   return accion;
}

/**
 * @brief Comprueba si una celda es de tipo camino transitable.
 * @param c Carácter que representa el tipo de superficie.
 * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
 */
bool ComportamientoIngeniero::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

/**
 * @brief Función auxiliar para darle un valor de "esfuerzo" a cada terreno.
 * @return Devuelve el "coste" de pisar ese terreno
 */
int ObtenerCosteTerrenoI1(unsigned char c, bool zapatillas) {
   if (c == 'U') return 0;
   if (c == 'C' || c == 'D') return 1;
   if (c == 'S') return 2;
   if (c == 'H') return 3;
    
   return 9999;
}

/**
 * @brief Determina la mejor opcion entre las 3 casillas que tiene delante
 * 
 * @param i terreno que hay en la posicion 1 de la superficie
 * @param c terreno que hay en la posicion 2 de la superficie
 * @param d terreno que hay en la posicion 3 de la superficie
 * @param calorI Veces que se ha pisado la casilla izquierda
 * @param calorC Veces que se ha pisado la casilla delantera
 * @param calorD Veces que se ha pisado la casilla derecha
 * @return 2 si es mejor WALK, 1 para TURN_SL y 3 para TURN_SR, 0 si no hay nada interesante
 */
int VeoCasillaInteresanteI1(char i, char c, char d, bool zapatillas, int calorI, int calorC, int calorD){
   if (!zapatillas) {
      if (c == 'D') return 2;
      else if (i == 'D') return 1;
      else if (d == 'D') return 3;
   }

   int costeI = ObtenerCosteTerrenoI1(i, zapatillas);
   int costeC = ObtenerCosteTerrenoI1(c, zapatillas);
   int costeD = ObtenerCosteTerrenoI1(d, zapatillas);

   if (costeI == 9999 && costeC == 9999 && costeD == 9999) return 0;

   costeI += (calorI * 10);
   costeC += (calorC * 10);
   costeD += (calorD * 10);
   
   int mejorOpcion = 0;
   int mejorCoste = 99999;

   if (costeI < mejorCoste) { 
      mejorCoste = costeI; 
      mejorOpcion = 1;
   }

   if (costeC <= mejorCoste) { 
      mejorCoste = costeC; 
      mejorOpcion = 2;
   }
   
   if (costeD < mejorCoste) { 
      mejorCoste = costeD; 
      mejorOpcion = 3;
   }

   return mejorOpcion;
}

/**
 * @brief Determina si la casilla es viable por altura.
 * @param casilla tipo de terreno
 * @param dif diferencia de altura entre casillas
 * @param zapatillas indica si tengo las zapatillas
 * @return 'P' si no es accesible por altura y casilla en otro caso
 */
char ViablePorAlturaI1(char casilla, int dif, bool zapatillas){
   if(abs(dif) <= 1 || (zapatillas && abs(dif) <= 2))
      return casilla;
   else
      return 'P';
}

/**
 * @brief Comportamiento reactivo del ingeniero para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_1(Sensores sensores)
{
   Action accion = IDLE;
   ActualizarMapa(sensores);

   if (sensores.superficie[0] == 'D' && !zapatillas) zapatillas = true;

   if(accion_anterior == WALK) mapaCalor[{sensores.posF, sensores.posC}]++;

   char i = ViablePorAlturaI1(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], zapatillas);
   char c = ViablePorAlturaI1(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], zapatillas);
   char d = ViablePorAlturaI1(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], zapatillas);

   if (sensores.agentes[1] != '_') i = 'P';
   if (sensores.agentes[2] != '_') c = 'P';
   if (sensores.agentes[3] != '_') d = 'P';

   int fI, cI, fC, cC, fD, cD;

   Coordenadas(sensores, fI, cI, fC, cC, fD, cD);

   int calorI = mapaCalor[{fI, cI}];
   int calorC = mapaCalor[{fC, cC}];
   int calorD = mapaCalor[{fD, cD}];

   int pos = VeoCasillaInteresanteI1(i, c, d, zapatillas, calorI, calorC, calorD);

   switch (pos)
   {
   case 2:
      accion = WALK;
      break;
   case 1:
      accion = TURN_SL;
      break;
   case 3:
      accion = TURN_SR;
      break;
   default:
      accion = TURN_SL;
      break;
   }

   accion_anterior = accion;
   return accion;
}

// Niveles avanzados (Uso de búsqueda)

ubicacion SiguienteCasilla(const ubicacion &origen, int pasos = 1) {
   ubicacion sig = origen;
   switch (origen.brujula) {
      case norte:    sig.f -= pasos; break;
      case noreste:  sig.f -= pasos; sig.c += pasos; break;
      case este:     sig.c += pasos; break;
      case sureste:  sig.f += pasos; sig.c += pasos; break;
      case sur:      sig.f += pasos; break;
      case suroeste: sig.f += pasos; sig.c -= pasos; break;
      case oeste:    sig.c -= pasos; break;
      case noroeste: sig.f -= pasos; sig.c -= pasos; break;
   }
   return sig;
}

bool EsMovimientoLegal(Action accion, const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura, const vector<vector<unsigned char>> &agentes) {
   if (accion == TURN_SL || accion == TURN_SR) return true;

   ubicacion origen = st.site;
   ubicacion destino;
   
   if (accion == WALK)
      destino = SiguienteCasilla(origen, 1);
   else if (accion == JUMP) {
      destino = SiguienteCasilla(origen, 2);
      ubicacion intermedia = SiguienteCasilla(origen, 1);
      
      unsigned char terr_int = terreno[intermedia.f][intermedia.c];
      if (terr_int == 'M' || terr_int == 'P' || terr_int == 'B') return false;
      if (agentes[intermedia.f][intermedia.c] != '_') return false; 
   } else {
      return false;
   }

   if (destino.f < 0 || destino.f >= terreno.size() || destino.c < 0 || destino.c >= terreno[0].size()) return false;

   unsigned char terr_dest = terreno[destino.f][destino.c];
   if (terr_dest == 'M' || terr_dest == 'P' || terr_dest == 'B') return false;

   int alt_origen = altura[origen.f][origen.c];
   int alt_dest = altura[destino.f][destino.c];
   int dif_altura = abs(alt_dest - alt_origen);
   
   int dif_maxima;
   if (st.zapatillas == true)
      dif_maxima = 2;
   else
      dif_maxima = 1;
   
   
   if (dif_altura > dif_maxima) return false;

   return true;
}

EstadoI AplicarAccion(Action accion, const EstadoI &st, const vector<vector<unsigned char>> &terreno) {
   EstadoI next_st = st;

   switch (accion) {
      case WALK:
         next_st.site = SiguienteCasilla(st.site, 1);
         break;
      case JUMP:
         next_st.site = SiguienteCasilla(st.site, 2);
         break;
      case TURN_SL:
         next_st.site.brujula = (Orientacion)(((int)st.site.brujula + 7) % 8);
         break;
      case TURN_SR:
         next_st.site.brujula = (Orientacion)(((int)st.site.brujula + 1) % 8);
         break;
   }

   if (terreno[next_st.site.f][next_st.site.c] == 'D')
      next_st.zapatillas = true;

   return next_st;
}

// =========================================================================
// ALGORITMO DE BÚSQUEDA EN ANCHURA (BFS)
// =========================================================================

list<Action> BusquedaAnchuraNivel2(const EstadoI &inicio, const ubicacion &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura, const vector<vector<unsigned char>> &agentes) {
   queue<NodoI> frontier;
   set<EstadoI> explored;
   list<Action> path;

   NodoI nodo_inicial;
   nodo_inicial.estado = inicio;
   
   frontier.push(nodo_inicial);
   explored.insert(inicio);

   vector<Action> acciones_posibles = {WALK, JUMP, TURN_SL, TURN_SR};

   while (!frontier.empty()) {
      NodoI current_node = frontier.front();
      frontier.pop();

      if (current_node.estado.site.f == final.f && current_node.estado.site.c == final.c) {
         return current_node.secuencia;
      }

      for (Action accion : acciones_posibles) {
         if (EsMovimientoLegal(accion, current_node.estado, terreno, altura, agentes)) {
            NodoI child;
            child.estado = AplicarAccion(accion, current_node.estado, terreno);
            
            if (explored.find(child.estado) == explored.end()) {
               child.secuencia = current_node.secuencia;
               child.secuencia.push_back(accion);
               
               frontier.push(child);
               explored.insert(child.estado);
            }
         }
      }
   }

   return path;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 2 (búsqueda).
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_2(Sensores sensores)
{
  Action accion = IDLE;

   if (!hayPlan) {
      EstadoI inicio;
      inicio.site.f = sensores.posF;
      inicio.site.c = sensores.posC;
      inicio.site.brujula = sensores.rumbo;
      inicio.zapatillas = false; 
      
      ubicacion objetivo;
      objetivo.f = sensores.BelPosF;
      objetivo.c = sensores.BelPosC;

      vector<vector<unsigned char>> mapaAgentes(mapaResultado.size(), vector<unsigned char>(mapaResultado[0].size(), '_')); 
      
      plan = BusquedaAnchuraNivel2(inicio, objetivo, mapaResultado, mapaCotas, mapaAgentes);
      
      if (plan.size() > 0) {
         VisualizaPlan(inicio.site, plan);
         hayPlan = true;
      }
   }

   if (hayPlan && plan.size() > 0) {
      accion = plan.front();
      plan.pop_front();
   }

   if (plan.size() == 0)
      hayPlan = false;

   return accion;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_3(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_4(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_5(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_6(Sensores sensores)
{
  return IDLE;
}

// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoIngeniero::ActualizarMapa(Sensores sensores)
{
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo)
  {
  case norte:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF - j][sensores.posC + i] = sensores.superficie[pos];
        mapaCotas[sensores.posF - j][sensores.posC + i] = sensores.cota[pos++];
      }
    break;
  case noreste:
    mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[1];
    mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[1];
    mapaResultado[sensores.posF - 1][sensores.posC + 1] = sensores.superficie[2];
    mapaCotas[sensores.posF - 1][sensores.posC + 1] = sensores.cota[2];
    mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[3];
    mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[3];
    mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[4];
    mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[4];
    mapaResultado[sensores.posF - 2][sensores.posC + 1] = sensores.superficie[5];
    mapaCotas[sensores.posF - 2][sensores.posC + 1] = sensores.cota[5];
    mapaResultado[sensores.posF - 2][sensores.posC + 2] = sensores.superficie[6];
    mapaCotas[sensores.posF - 2][sensores.posC + 2] = sensores.cota[6];
    mapaResultado[sensores.posF - 1][sensores.posC + 2] = sensores.superficie[7];
    mapaCotas[sensores.posF - 1][sensores.posC + 2] = sensores.cota[7];
    mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[8];
    mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[8];
    mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[9];
    mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[9];
    mapaResultado[sensores.posF - 3][sensores.posC + 1] = sensores.superficie[10];
    mapaCotas[sensores.posF - 3][sensores.posC + 1] = sensores.cota[10];
    mapaResultado[sensores.posF - 3][sensores.posC + 2] = sensores.superficie[11];
    mapaCotas[sensores.posF - 3][sensores.posC + 2] = sensores.cota[11];
    mapaResultado[sensores.posF - 3][sensores.posC + 3] = sensores.superficie[12];
    mapaCotas[sensores.posF - 3][sensores.posC + 3] = sensores.cota[12];
    mapaResultado[sensores.posF - 2][sensores.posC + 3] = sensores.superficie[13];
    mapaCotas[sensores.posF - 2][sensores.posC + 3] = sensores.cota[13];
    mapaResultado[sensores.posF - 1][sensores.posC + 3] = sensores.superficie[14];
    mapaCotas[sensores.posF - 1][sensores.posC + 3] = sensores.cota[14];
    mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[15];
    mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[15];
    break;
  case este:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF + i][sensores.posC + j] = sensores.superficie[pos];
        mapaCotas[sensores.posF + i][sensores.posC + j] = sensores.cota[pos++];
      }
    break;
  case sureste:
    mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[1];
    mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[1];
    mapaResultado[sensores.posF + 1][sensores.posC + 1] = sensores.superficie[2];
    mapaCotas[sensores.posF + 1][sensores.posC + 1] = sensores.cota[2];
    mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[3];
    mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[3];
    mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[4];
    mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[4];
    mapaResultado[sensores.posF + 1][sensores.posC + 2] = sensores.superficie[5];
    mapaCotas[sensores.posF + 1][sensores.posC + 2] = sensores.cota[5];
    mapaResultado[sensores.posF + 2][sensores.posC + 2] = sensores.superficie[6];
    mapaCotas[sensores.posF + 2][sensores.posC + 2] = sensores.cota[6];
    mapaResultado[sensores.posF + 2][sensores.posC + 1] = sensores.superficie[7];
    mapaCotas[sensores.posF + 2][sensores.posC + 1] = sensores.cota[7];
    mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[8];
    mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[8];
    mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[9];
    mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[9];
    mapaResultado[sensores.posF + 1][sensores.posC + 3] = sensores.superficie[10];
    mapaCotas[sensores.posF + 1][sensores.posC + 3] = sensores.cota[10];
    mapaResultado[sensores.posF + 2][sensores.posC + 3] = sensores.superficie[11];
    mapaCotas[sensores.posF + 2][sensores.posC + 3] = sensores.cota[11];
    mapaResultado[sensores.posF + 3][sensores.posC + 3] = sensores.superficie[12];
    mapaCotas[sensores.posF + 3][sensores.posC + 3] = sensores.cota[12];
    mapaResultado[sensores.posF + 3][sensores.posC + 2] = sensores.superficie[13];
    mapaCotas[sensores.posF + 3][sensores.posC + 2] = sensores.cota[13];
    mapaResultado[sensores.posF + 3][sensores.posC + 1] = sensores.superficie[14];
    mapaCotas[sensores.posF + 3][sensores.posC + 1] = sensores.cota[14];
    mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[15];
    mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[15];
    break;
  case sur:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF + j][sensores.posC - i] = sensores.superficie[pos];
        mapaCotas[sensores.posF + j][sensores.posC - i] = sensores.cota[pos++];
      }
    break;
  case suroeste:
    mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[1];
    mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[1];
    mapaResultado[sensores.posF + 1][sensores.posC - 1] = sensores.superficie[2];
    mapaCotas[sensores.posF + 1][sensores.posC - 1] = sensores.cota[2];
    mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[3];
    mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[3];
    mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[4];
    mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[4];
    mapaResultado[sensores.posF + 2][sensores.posC - 1] = sensores.superficie[5];
    mapaCotas[sensores.posF + 2][sensores.posC - 1] = sensores.cota[5];
    mapaResultado[sensores.posF + 2][sensores.posC - 2] = sensores.superficie[6];
    mapaCotas[sensores.posF + 2][sensores.posC - 2] = sensores.cota[6];
    mapaResultado[sensores.posF + 1][sensores.posC - 2] = sensores.superficie[7];
    mapaCotas[sensores.posF + 1][sensores.posC - 2] = sensores.cota[7];
    mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[8];
    mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[8];
    mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[9];
    mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[9];
    mapaResultado[sensores.posF + 3][sensores.posC - 1] = sensores.superficie[10];
    mapaCotas[sensores.posF + 3][sensores.posC - 1] = sensores.cota[10];
    mapaResultado[sensores.posF + 3][sensores.posC - 2] = sensores.superficie[11];
    mapaCotas[sensores.posF + 3][sensores.posC - 2] = sensores.cota[11];
    mapaResultado[sensores.posF + 3][sensores.posC - 3] = sensores.superficie[12];
    mapaCotas[sensores.posF + 3][sensores.posC - 3] = sensores.cota[12];
    mapaResultado[sensores.posF + 2][sensores.posC - 3] = sensores.superficie[13];
    mapaCotas[sensores.posF + 2][sensores.posC - 3] = sensores.cota[13];
    mapaResultado[sensores.posF + 1][sensores.posC - 3] = sensores.superficie[14];
    mapaCotas[sensores.posF + 1][sensores.posC - 3] = sensores.cota[14];
    mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[15];
    mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[15];
    break;
  case oeste:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF - i][sensores.posC - j] = sensores.superficie[pos];
        mapaCotas[sensores.posF - i][sensores.posC - j] = sensores.cota[pos++];
      }
    break;
  case noroeste:
    mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[1];
    mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[1];
    mapaResultado[sensores.posF - 1][sensores.posC - 1] = sensores.superficie[2];
    mapaCotas[sensores.posF - 1][sensores.posC - 1] = sensores.cota[2];
    mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[3];
    mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[3];
    mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[4];
    mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[4];
    mapaResultado[sensores.posF - 1][sensores.posC - 2] = sensores.superficie[5];
    mapaCotas[sensores.posF - 1][sensores.posC - 2] = sensores.cota[5];
    mapaResultado[sensores.posF - 2][sensores.posC - 2] = sensores.superficie[6];
    mapaCotas[sensores.posF - 2][sensores.posC - 2] = sensores.cota[6];
    mapaResultado[sensores.posF - 2][sensores.posC - 1] = sensores.superficie[7];
    mapaCotas[sensores.posF - 2][sensores.posC - 1] = sensores.cota[7];
    mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[8];
    mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[8];
    mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[9];
    mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[9];
    mapaResultado[sensores.posF - 1][sensores.posC - 3] = sensores.superficie[10];
    mapaCotas[sensores.posF - 1][sensores.posC - 3] = sensores.cota[10];
    mapaResultado[sensores.posF - 2][sensores.posC - 3] = sensores.superficie[11];
    mapaCotas[sensores.posF - 2][sensores.posC - 3] = sensores.cota[11];
    mapaResultado[sensores.posF - 3][sensores.posC - 3] = sensores.superficie[12];
    mapaCotas[sensores.posF - 3][sensores.posC - 3] = sensores.cota[12];
    mapaResultado[sensores.posF - 3][sensores.posC - 2] = sensores.superficie[13];
    mapaCotas[sensores.posF - 3][sensores.posC - 2] = sensores.cota[13];
    mapaResultado[sensores.posF - 3][sensores.posC - 1] = sensores.superficie[14];
    mapaCotas[sensores.posF - 3][sensores.posC - 1] = sensores.cota[14];
    mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[15];
    mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[15];
    break;
  }
}

/**
 * @brief Determina si una casilla es transitable para el ingeniero.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable (no es muro ni precipicio).
 */
bool ComportamientoIngeniero::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas)
{
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size())
    return false;
  return es_camino(mapaResultado[f][c]); // Solo 'C', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el ingeniero: desnivel máximo 1 sin zapatillas, 2 con zapatillas.
 * @param actual Estado actual del agente (fila, columna, orientacion, zap).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoIngeniero::EsAccesiblePorAltura(const ubicacion &actual, bool zap)
{
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size())
    return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (zap && desnivel > 2)
    return false;
  if (!zap && desnivel > 1)
    return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoIngeniero::Delante(const ubicacion &actual) const
{
  ubicacion delante = actual;
  switch (actual.brujula)
  {
  case 0:
    delante.f--;
    break; // norte
  case 1:
    delante.f--;
    delante.c++;
    break; // noreste
  case 2:
    delante.c++;
    break; // este
  case 3:
    delante.f++;
    delante.c++;
    break; // sureste
  case 4:
    delante.f++;
    break; // sur
  case 5:
    delante.f++;
    delante.c--;
    break; // suroeste
  case 6:
    delante.c--;
    break; // oeste
  case 7:
    delante.f--;
    delante.c--;
    break; // noroeste
  }
  return delante;
}

/**
 * @brief Imprime por consola la secuencia de acciones de un plan.
 *
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoIngeniero::PintaPlan(const list<Action> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    if (*it == WALK)
    {
      cout << "W ";
    }
    else if (*it == JUMP)
    {
      cout << "J ";
    }
    else if (*it == TURN_SR)
    {
      cout << "r ";
    }
    else if (*it == TURN_SL)
    {
      cout << "l ";
    }
    else if (*it == COME)
    {
      cout << "C ";
    }
    else if (*it == IDLE)
    {
      cout << "I ";
    }
    else
    {
      cout << "-_ ";
    }
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}

/**
 * @brief Imprime las coordenadas y operaciones de un plan de tubería.
 *
 * @param plan  Lista de pasos (fila, columna, operación),
 *              donde operacion = -1 (DIG), operación = 1 (RAISE).
 */
void ComportamientoIngeniero::PintaPlan(const list<Paso> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    cout << it->fil << ", " << it->col << " (" << it->op << ")\n";
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}

/**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa 2D.
 *
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoIngeniero::VisualizaPlan(const ubicacion &st,
                                            const list<Action> &plan)
{
  listaPlanCasillas.clear();
  ubicacion cst = st;

  listaPlanCasillas.push_back({cst.f, cst.c, WALK});
  auto it = plan.begin();
  while (it != plan.end())
  {

    switch (*it)
    {
    case JUMP:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, JUMP});
    case WALK:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, WALK});
      break;
    case TURN_SR:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 1) % 8);
      break;
    case TURN_SL:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 7) % 8);
      break;
    }
    it++;
  }
}

/**
 * @brief Convierte un plan de tubería en la lista de casillas usada
 *        por el sistema de visualización.
 *
 * @param st    Estado de partida (no utilizado directamente).
 * @param plan  Lista de pasos del plan de tubería.
 */
void ComportamientoIngeniero::VisualizaRedTuberias(const list<Paso> &plan)
{
  listaCanalizacionTuberias.clear();
  auto it = plan.begin();
  while (it != plan.end())
  {
    listaCanalizacionTuberias.push_back({it->fil, it->col, it->op});
    it++;
  }
}
