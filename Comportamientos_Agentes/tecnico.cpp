#include "tecnico.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoTecnico::think(Sensores sensores) {
  Action accion = IDLE;


  // Decisión del agente según el nivel
  switch (sensores.nivel) {
    case 0: accion = ComportamientoTecnicoNivel_0(sensores); break;
    case 1: accion = ComportamientoTecnicoNivel_1(sensores); break;
    case 2: accion = ComportamientoTecnicoNivel_2(sensores); break;
    case 3: accion = ComportamientoTecnicoNivel_3(sensores); break;
    case 4: accion = ComportamientoTecnicoNivel_4(sensores); break;
    case 5: accion = ComportamientoTecnicoNivel_5(sensores); break;
    case 6: accion = ComportamientoTecnicoNivel_6(sensores); break;
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
int VeoCasillaInteresanteT0(char i, char c, char d, bool zapatillas, int calorI, int calorC, int calorD){
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
char ViablePorAlturaT0(char casilla, int dif, bool zapatillas){
   if(abs(dif) <= 1)
      return casilla;
   else
      return 'P';
}

// Niveles del técnico
Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores) {
   Action accion = IDLE;
   ActualizarMapa(sensores);

   if(sensores.superficie[0] == 'U') return IDLE;

   if(sensores.superficie[0] == 'D') zapatillas = true;

   if(accion_anterior == WALK) mapaCalor[{sensores.posF, sensores.posC}]++;

   char i = ViablePorAlturaT0(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], zapatillas);
   char c = ViablePorAlturaT0(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], zapatillas);
   char d = ViablePorAlturaT0(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], zapatillas);

   if (sensores.agentes[1] != '_') i = 'P';
   if (sensores.agentes[2] != '_') c = 'P';
   if (sensores.agentes[3] != '_') d = 'P';

   int fI, cI, fC, cC, fD, cD;

   Coordenadas(sensores, fI, cI, fC, cC, fD, cD);

   int calorI = mapaCalor[{fI, cI}];
   int calorC = mapaCalor[{fC, cC}];
   int calorD = mapaCalor[{fD, cD}];

   int pos = VeoCasillaInteresanteT0(i, c, d, zapatillas, calorI, calorC, calorD);

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
      accion = TURN_SR;
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
bool ComportamientoTecnico::es_camino(unsigned char c) const {
  return (c == 'C' || c == 'D' || c == 'U');
}

int ObtenerCosteTerrenoT1(unsigned char c, bool zapatillas) {
   if (c == 'U') return 0;
   if (c == 'C' || c == 'D') return 1;
   if (c == 'S') return 2;
   if (c == 'H') return 3;
   if (c == 'B' && zapatillas) return 4;
    
   return 9999;
}

/**
 * @brief Determina la mejor opcion entre las 3 casillas que tiene delante
 * 
 * @param i terreno que hay en la posicion 1 de la superficie
 * @param c terreno que hay en la posicion 2 de la superficie
 * @param d terreno que hay en la posicion 3 de la superficie
 * @return 2 si es mejor WALK, 1 para TURN_SL y 3 para TURN_SR, 0 si no hay nada interesante
 */
int VeoCasillaInteresanteT1(char i, char c, char d, bool zapatillas, int calorI, int calorC, int calorD){
   if (!zapatillas) {
      if (c == 'D') return 2;
      else if (i == 'D') return 1;
      else if (d == 'D') return 3;
   }

   int costeI = ObtenerCosteTerrenoT1(i, zapatillas);
   int costeC = ObtenerCosteTerrenoT1(c, zapatillas);
   int costeD = ObtenerCosteTerrenoT1(d, zapatillas);

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

char ViablePorAlturaT1(char casilla, int dif, bool zapatillas){
   if(abs(dif) <= 1)
      return casilla;
   else
      return 'P';
}

/**
 * @brief Comportamiento reactivo del técnico para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_1(Sensores sensores) {
   Action accion = IDLE;
   ActualizarMapa(sensores);

   if (sensores.superficie[0] == 'D' && !zapatillas) zapatillas = true;

   if(accion_anterior == WALK) mapaCalor[{sensores.posF, sensores.posC}]++;

   char i = ViablePorAlturaT1(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], zapatillas);
   char c = ViablePorAlturaT1(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], zapatillas);
   char d = ViablePorAlturaT1(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], zapatillas);

   if (sensores.agentes[1] != '_') i = 'P';
   if (sensores.agentes[2] != '_') c = 'P';
   if (sensores.agentes[3] != '_') d = 'P';

   int fI, cI, fC, cC, fD, cD;

   Coordenadas(sensores, fI, cI, fC, cC, fD, cD);

   int calorI = mapaCalor[{fI, cI}];
   int calorC = mapaCalor[{fC, cC}];
   int calorD = mapaCalor[{fD, cD}];

   int pos = VeoCasillaInteresanteT1(i, c, d, zapatillas, calorI, calorC, calorD);

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

ubicacion SiguienteCasillaT(const ubicacion &origen) {
   ubicacion sig = origen;
   switch (origen.brujula) {
      case norte:    sig.f--; break;
      case noreste:  sig.f--; sig.c++; break;
      case este:     sig.c++; break;
      case sureste:  sig.f++; sig.c++; break;
      case sur:      sig.f++; break;
      case suroeste: sig.f++; sig.c--; break;
      case oeste:    sig.c--; break;
      case noroeste: sig.f--; sig.c--; break;
   }
   return sig;
}

bool EsMovimientoLegalT(Action accion, const EstadoT &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura, const vector<vector<unsigned char>> &agentes) {
   if (accion == TURN_SL || accion == TURN_SR) return true;
   if (accion != WALK) return false;

   ubicacion destino = SiguienteCasillaT(st.site);

   if (destino.f < 0 || destino.f >= (int)terreno.size() || destino.c < 0 || destino.c >= (int)terreno[0].size()) 
      return false;

   unsigned char t = terreno[destino.f][destino.c];
   if (t == 'M' || t == 'P') return false;
   if (t == 'B' && !st.zapatillas) return false;

   if (agentes[destino.f][destino.c] != '_') return false;

   int dif_altura = abs(altura[destino.f][destino.c] - altura[st.site.f][st.site.c]);
   if (dif_altura > 1) return false;

   return true;
}

EstadoT AplicarAccionT(Action accion, const EstadoT &st, const vector<vector<unsigned char>> &terreno) {
   EstadoT next_st = st;

   switch (accion) {
      case WALK:
         next_st.site = SiguienteCasillaT(st.site);
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

/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_2(Sensores sensores) {
   Action accion = TURN_SL;

   bool ingenieroCerca = false;
   for (int i = 1; i <= 3; i++) {
      if (sensores.agentes[i] == 'i') {
         ingenieroCerca = true;
         break;
      }
   }

   if (ingenieroCerca) {
      int dif_altura = abs(sensores.cota[2] - sensores.cota[0]);
      
      if (sensores.superficie[2] != 'M' && sensores.superficie[2] != 'P' && 
         (sensores.superficie[2] != 'B' || zapatillas) && 
          sensores.agentes[2] == '_' && dif_altura <= 1) {
         accion = WALK;
      } else {
         accion = TURN_SL;
      }
   }

   return accion;
}

int CalcularHeuristicaT(const ubicacion &actual, const ubicacion &destino) {
   return max(abs(actual.f - destino.f), abs(actual.c - destino.c));
}

int CalcularCosteT(Action accion, const EstadoT &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {
   unsigned char casilla_actual = terreno[st.site.f][st.site.c];
   int coste_base = 0;

   if (accion == WALK) {
      if (casilla_actual == 'A') coste_base = 60;
      else if (casilla_actual == 'H') coste_base = 6;
      else if (casilla_actual == 'S') coste_base = 3;
      else coste_base = 1;

      if (casilla_actual == 'A' || casilla_actual == 'H' || casilla_actual == 'S') {
         ubicacion destino = SiguienteCasillaT(st.site);
         int dif_altura = altura[destino.f][destino.c] - altura[st.site.f][st.site.c];
            
         if (dif_altura > 0) coste_base += 5;      
         else if (dif_altura < 0) coste_base -= 2; 
      }
        
   } else if (accion == TURN_SL || accion == TURN_SR) {
      if (casilla_actual == 'A') coste_base = 5;
      else if (casilla_actual == 'H') coste_base = 2;
      else if (casilla_actual == 'S') coste_base = 1;
      else coste_base = 1;
   }

   return coste_base;
}

// =========================================================================
// ALGORITMO A* PARA EL TÉCNICO (NIVEL 3)
// =========================================================================

list<Action> A_Estrella_Tecnico(const EstadoT &inicio, const ubicacion &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura, const vector<vector<unsigned char>> &agentes) {
   priority_queue<NodoT, vector<NodoT>, ComparaNodosT> frontier;
   map<EstadoT, int> costos_minimos;

   NodoT nodo_inicial;
   nodo_inicial.estado = inicio;
   nodo_inicial.coste = 0;
   nodo_inicial.heuristica = CalcularHeuristicaT(inicio.site, final);
   
   frontier.push(nodo_inicial);
   costos_minimos[inicio] = 0;

   vector<Action> acciones_posibles = {WALK, TURN_SL, TURN_SR};

   while (!frontier.empty()) {
      NodoT current_node = frontier.top();
      frontier.pop();

      if (current_node.estado.site.f == final.f && current_node.estado.site.c == final.c)
         return current_node.secuencia;

      if (current_node.coste <= costos_minimos[current_node.estado]){

         for (Action accion : acciones_posibles) {
            if (EsMovimientoLegalT(accion, current_node.estado, terreno, altura, agentes)) {
               NodoT child;
               child.estado = AplicarAccionT(accion, current_node.estado, terreno);

               child.coste = current_node.coste + CalcularCosteT(accion, current_node.estado, terreno, altura);
               
               child.heuristica = CalcularHeuristicaT(child.estado.site, final);
               child.secuencia = current_node.secuencia;
               child.secuencia.push_back(accion);
               
               if (costos_minimos.find(child.estado) == costos_minimos.end() || child.coste < costos_minimos[child.estado]) {
                  costos_minimos[child.estado] = child.coste;
                  frontier.push(child);
               }
            }
         }
      }
   }

   return {};
}

/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_3(Sensores sensores) {
   Action accion = IDLE;

   if (matrizAgentes.empty() && mapaResultado.size() > 0)
      matrizAgentes.assign(mapaResultado.size(), vector<unsigned char>(mapaResultado[0].size(), '_'));

   if (!hayPlan) {
      EstadoT inicio;
      inicio.site.f = sensores.posF; 
      inicio.site.c = sensores.posC; 
      inicio.site.brujula = sensores.rumbo;
      inicio.zapatillas = this->zapatillas;

      ubicacion objetivo;
      objetivo.f = sensores.BelPosF; 
      objetivo.c = sensores.BelPosC;

      plan = A_Estrella_Tecnico(inicio, objetivo, mapaResultado, mapaCotas, matrizAgentes);   

      if (plan.size() > 0) {
         VisualizaPlan(inicio.site, plan);
         hayPlan = true;
      }
   }

   if (hayPlan && !plan.empty()) {
    Action a = plan.front();
    bool choque = false;
    ubicacion actual = {sensores.posF, sensores.posC, sensores.rumbo};

    if (a == WALK && sensores.agentes[2] != '_')
        choque = true;

    if (choque) {
        hayPlan = false;
        plan.clear();
    } else {
        accion = plan.front();
        plan.pop_front();
    }
}
   
   if (plan.empty()) {
      hayPlan = false;
   }

   return accion;
}

/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_4(Sensores sensores) {
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_5(Sensores sensores) {
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_6(Sensores sensores) {
  return IDLE;
}




// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoTecnico::ActualizarMapa(Sensores sensores) {
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo) {
    case norte:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
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
        for (int i = -j; i <= j; i++) {
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
        for (int i = -j; i <= j; i++) {
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
        for (int i = -j; i <= j; i++) {
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
 * @brief Determina si una casilla es transitable para el técnico.
 * En esta práctica, si el técnico tiene zapatillas, el bosque ('B') es transitable.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable.
 */
bool ComportamientoTecnico::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas) {
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size()) return false;
  return es_camino(mapaResultado[f][c]);  // Solo 'C', 'S', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el técnico: desnivel máximo siempre 1.
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoTecnico::EsAccesiblePorAltura(const ubicacion &actual) {
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size()) return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (desnivel > 1) return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoTecnico::Delante(const ubicacion &actual) const {
  ubicacion delante = actual;
  switch (actual.brujula) {
    case 0: delante.f--; break;                        // norte
    case 1: delante.f--; delante.c++; break;     // noreste
    case 2: delante.c++; break;                     // este
    case 3: delante.f++; delante.c++; break;     // sureste
    case 4: delante.f++; break;                        // sur
    case 5: delante.f++; delante.c--; break;     // suroeste
    case 6: delante.c--; break;                     // oeste
    case 7: delante.f--; delante.c--; break;     // noroeste
  }
  return delante;
}


/**
 * @brief Imprime por consola la secuencia de acciones de un plan.
 *
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::PintaPlan(const list<Action> &plan)
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
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa 2D.
 *
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::VisualizaPlan(const ubicacion &st,
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


