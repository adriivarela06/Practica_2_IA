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

   if (t == 'M' || t == 'P' || t == '?') return false;
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

   if (sensores.superficie[0] == 'D') zapatillas = true;

   bool ingenieroCerca = false;
   for (int i = 1; i <= 8; i++) {
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

   if (sensores.superficie[0] == 'D') zapatillas = true;

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
      accion = plan.front();
      ubicacion actual = {sensores.posF, sensores.posC, sensores.rumbo};

      if (accion == WALK && sensores.agentes[2] != '_')
         accion = IDLE;
      else
         plan.pop_front();
   }
   
   if (plan.empty())
      hayPlan = false;

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

int CalcularEnergiaTuberiaT(unsigned char terreno, int op) {
   int energia = 0;
   
   if (terreno == 'A') energia += 60;
   else if (terreno == 'H') energia += 45;
   else if (terreno == 'S') energia += 25;
   else if (terreno == 'C' || terreno == 'U') energia += 15;
   else energia += 30; 

   if (op == 1) {
      if (terreno == 'H') energia += 55;
      else if (terreno == 'S') energia += 30;
      else if (terreno == 'C' || terreno == 'U') energia += 10;
      else energia += 40;
   }
   else if (op == -1) {
      if (terreno == 'H') energia += 65;
      else if (terreno == 'S') energia += 40;
      else if (terreno == 'C' || terreno == 'U') energia += 25;
      else energia += 50;
   }
   return energia;
}

int CalcularEcoTuberiaT(unsigned char terreno, int op) {
   int eco = 0;
    
   if (terreno == 'A') eco += 50;
   else if (terreno == 'H') eco += 45;
   else if (terreno == 'S') eco += 25;
   else if (terreno == 'C' || terreno == 'U') eco += 15;
   else eco += 30;

   if (op == 1) {
      if (terreno == 'H') eco += 55;
      else if (terreno == 'S') eco += 30;
      else if (terreno == 'C' || terreno == 'U') eco += 10;
      else eco += 40;
   }
   else if (op == -1) {
      if (terreno == 'H') eco += 65;
      else if (terreno == 'S') eco += 40;
      else if (terreno == 'C' || terreno == 'U') eco += 25;
      else eco += 50;
   }
   return eco;
}

list<Paso> Algoritmo_Tuberias_Tecnico(int inicio_f, int inicio_c, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura, int max_energia, int max_eco) {
   priority_queue<NodoTuberiaT, vector<NodoTuberiaT>, greater<NodoTuberiaT>> frontier;
   int filas = terreno.size(), cols = terreno[0].size();
   map<PadreT, int> min_eco_visto; 

   for (int i = -1; i <= 1; i++) {
      unsigned char t_ini = terreno[inicio_f][inicio_c];
      if ((i == 0 || t_ini != 'A') && (i != 1 || altura[inicio_f][inicio_c] < 9) && (i != -1 || altura[inicio_f][inicio_c] > 1)) {
         NodoTuberiaT inicial;
         inicial.f = inicio_f; inicial.c = inicio_c; inicial.op = i;
         inicial.altura_mod = altura[inicio_f][inicio_c] + i;
         inicial.dist = 1;
         
         int coste_base_ene = CalcularEnergiaTuberiaT(t_ini, 0);
         int coste_base_eco = CalcularEcoTuberiaT(t_ini, 0);
         inicial.energia = CalcularEnergiaTuberiaT(t_ini, i) - coste_base_ene;
         inicial.eco = CalcularEcoTuberiaT(t_ini, i) - coste_base_eco;

         if (inicial.energia <= max_energia && inicial.eco <= max_eco) {
            inicial.camino.push_back({inicio_f, inicio_c, i});
            frontier.push(inicial);
            min_eco_visto[{inicio_f, inicio_c, i}] = inicial.eco;
         }
      }
   }

   int df[] = {-1, 1, 0, 0}, dc[] = {0, 0, 1, -1};

   while (!frontier.empty()) {
      NodoTuberiaT actual = frontier.top();
      frontier.pop();

      if (terreno[actual.f][actual.c] == 'U') return actual.camino;

      for (int i = 0; i < 4; i++) {
         int nf = actual.f + df[i], nc = actual.c + dc[i];
         if (nf >= 0 && nf < filas && nc >= 0 && nc < cols) {
            unsigned char t = terreno[nf][nc];
            // PROTECCIÓN NIVEL 6
            if (t != 'M' && t != 'P' && t != 'B' && t != '?') {
               for (int nop = -1; nop <= 1; nop++) {
                  if (!(nop != 0 && t == 'A') && !(nop == 1 && altura[nf][nc] >= 9) && !(nop == -1 && altura[nf][nc] <= 1)) {
                     int nalt = altura[nf][nc] + nop;
                     int dif_alt = actual.altura_mod - nalt;
                     
                     if (dif_alt == 0 || dif_alt == 1) {
                        int n_energia = actual.energia + CalcularEnergiaTuberiaT(t, nop);
                        int eco_inst_actual = CalcularEcoTuberiaT(terreno[actual.f][actual.c], 0);
                        int n_eco = actual.eco + eco_inst_actual + CalcularEcoTuberiaT(t, nop);
                        
                        if (n_energia <= max_energia && n_eco <= max_eco) {
                           PadreT estado_hijo = {nf, nc, nop};
                           if (min_eco_visto.find(estado_hijo) == min_eco_visto.end() || n_eco < min_eco_visto[estado_hijo]) {
                              min_eco_visto[estado_hijo] = n_eco;
                              
                              NodoTuberiaT hijo;
                              hijo.f = nf; hijo.c = nc; hijo.op = nop;
                              hijo.altura_mod = nalt;
                              hijo.energia = n_energia; hijo.eco = n_eco; 
                              hijo.dist = actual.dist + 1;
                              hijo.camino = actual.camino; 
                              hijo.camino.push_back({nf, nc, nop}); 
                              frontier.push(hijo);
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
   return {}; 
}

Orientacion CalcularOrientacionHaciaObjetivoT(int fila_origen, int col_origen, int fila_destino, int col_destino) {
   if (fila_destino < fila_origen && col_destino == col_origen) return norte;
   if (fila_destino > fila_origen && col_destino == col_origen) return sur;
   if (fila_destino == fila_origen && col_destino < col_origen) return oeste;
   if (fila_destino == fila_origen && col_destino > col_origen) return este;
   return norte; 
}

Action NavegarHaciaDestinoT(Sensores &sensores, ubicacion objetivo, bool tiene_zapatillas, const vector<vector<unsigned char>> &mapaResultado, const vector<vector<unsigned char>> &mapaCotas, const vector<Paso> &plan, int idx_ingeniero, list<Action> &ruta_nav, int &intentos_bloqueo) {
   if (ruta_nav.empty()) {
      EstadoT inicio;
      inicio.site.f = sensores.posF; inicio.site.c = sensores.posC; inicio.site.brujula = sensores.rumbo; inicio.zapatillas = tiene_zapatillas;
      vector<vector<unsigned char>> mapa_agentes(mapaResultado.size(), vector<unsigned char>(mapaResultado[0].size(), '_'));
      if (idx_ingeniero >= 0 && idx_ingeniero < plan.size())
         mapa_agentes[plan[idx_ingeniero].fil][plan[idx_ingeniero].col] = 'i';

      ruta_nav = A_Estrella_Tecnico(inicio, objetivo, mapaResultado, mapaCotas, mapa_agentes);
   }
   
   if (!ruta_nav.empty()) {
      Action a = ruta_nav.front();
      bool colision = false;

      if (a == WALK) {
         if (sensores.agentes[2] != '_') colision = true;
         if (abs(sensores.cota[2] - sensores.cota[0]) > 1){
            ruta_nav.clear(); 
            return IDLE;
         }
         
      }

      if (colision) {
         intentos_bloqueo++;
         if (intentos_bloqueo > 3){
            ruta_nav.clear(); 
            intentos_bloqueo = 0;
         }

         return IDLE;
      }
      
      intentos_bloqueo = 0;
      ruta_nav.pop_front();
      return a;
   }

   ruta_nav.clear();
   intentos_bloqueo++;
   return TURN_SR;
}

/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_5(Sensores sensores)
{
   ActualizarMapa(sensores);
   if (sensores.superficie[0] == 'D') zapatillas = true;

   if (fase_actual == 0) {
      int eco_disponible = sensores.max_ecologico - sensores.ecologico;

      list<Paso> ruta_tuberias = Algoritmo_Tuberias_Tecnico(sensores.BelPosF, sensores.BelPosC, mapaResultado, mapaCotas, sensores.energia, eco_disponible);
      
      if (ruta_tuberias.size() < 2) return IDLE;
      
      for (auto tramo : ruta_tuberias) 
         plan_construccion.push_back(tramo);

      fase_actual = 1;
      indice_tramo = 0;
   }

   if (fase_actual == 1) {
      if (sensores.venpaca)
         fase_actual = 2; 
      else {
         bool ingenieroCerca = false,
              salida = false;
         for (int i = 1; i <= 8 && !salida; i++) {
            if (sensores.agentes[i] == 'i'){
               ingenieroCerca = true; 
               salida = true;
            }
         }
         if (ingenieroCerca) {
            int dif_altura = abs(sensores.cota[2] - sensores.cota[0]);
            if (sensores.superficie[2] != 'M' && sensores.superficie[2] != 'P' && 
               (sensores.superficie[2] != 'B' || zapatillas) && 
                sensores.agentes[2] == '_' && dif_altura <= 1) {
               return WALK;
            } else
               return TURN_SL;
         }
         return IDLE;
      }
   }

   if (fase_actual == 2) {
      if (indice_tramo >= plan_construccion.size() - 1) return IDLE; 

      Paso casilla_tecnico = plan_construccion[indice_tramo];
      Paso casilla_ingeniero = plan_construccion[indice_tramo + 1];

      if (sensores.posF == casilla_tecnico.fil && sensores.posC == casilla_tecnico.col) {
         Orientacion orientacion_necesaria = CalcularOrientacionHaciaObjetivoT(casilla_tecnico.fil, casilla_tecnico.col, casilla_ingeniero.fil, casilla_ingeniero.col);
         
         if (sensores.rumbo == orientacion_necesaria) {
            if (sensores.enfrente) {
               indice_tramo++;
               ruta_nav.clear();
               return INSTALL;
            } else
               return IDLE; 
         } else
            return TURN_SL;
      } else {
         ubicacion posicion_destino; 
         posicion_destino.f = casilla_tecnico.fil; 
         posicion_destino.c = casilla_tecnico.col;
         return NavegarHaciaDestinoT(sensores, posicion_destino, zapatillas, mapaResultado, mapaCotas, plan_construccion, indice_tramo + 1, ruta_nav, intentos_bloqueo);
      }
   }

   return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_6(Sensores sensores)
{
   ActualizarMapa(sensores);
   if (sensores.superficie[0] == 'D') zapatillas = true;

   if (fase_actual == 0) {
      bool conozco_U = false,
           salida = false;
      if(mapaResultado.size() > 0 && mapaResultado[0].size() > 0) {
         for(int i=0; i<mapaResultado.size() && !salida; i++){
            for(int j=0; j<mapaResultado[0].size() && !salida; j++){
               if(mapaResultado[i][j] == 'U') { 
                  conozco_U = true; 
                  salida = true; 
               }
            }
            if(conozco_U) salida = true;
         }
      }

      bool conozco_Belkanita = (sensores.BelPosF != -1 && sensores.BelPosC != -1);

      int ciclos_expl_t = 0;
      ciclos_expl_t++;

      if (conozco_U && conozco_Belkanita && (ciclos_expl_t % 15 == 0)) {
         int eco_disponible = sensores.max_ecologico - sensores.ecologico;
         list<Paso> ruta_tuberias = Algoritmo_Tuberias_Tecnico(sensores.BelPosF, sensores.BelPosC, mapaResultado, mapaCotas, sensores.energia, eco_disponible);
         
         if (ruta_tuberias.size() >= 2) {
            plan_construccion.clear();
            for (auto tramo : ruta_tuberias) plan_construccion.push_back(tramo);
            fase_actual = 1; 
            indice_tramo = 0;
            ruta_nav.clear();
            intentos_bloqueo = 0;
            return IDLE;
         }
      }
      return ComportamientoTecnicoNivel_1(sensores);
   }

   if (fase_actual == 1) {
      if (sensores.venpaca)
         fase_actual = 2; 
      else
         return ComportamientoTecnicoNivel_1(sensores); 
   }

   if (fase_actual == 2) {
      if (plan_construccion.empty() || indice_tramo >= (int)plan_construccion.size() - 1)
         fase_actual = 0; plan_construccion.clear(); return IDLE; 

      Paso casilla_tecnico = plan_construccion[indice_tramo];
      Paso casilla_ingeniero = plan_construccion[indice_tramo + 1];

      if (sensores.posF == casilla_tecnico.fil && sensores.posC == casilla_tecnico.col) {
         Orientacion orientacion_necesaria = CalcularOrientacionHaciaObjetivoT(casilla_tecnico.fil, casilla_tecnico.col, casilla_ingeniero.fil, casilla_ingeniero.col);
         
         if (sensores.rumbo == orientacion_necesaria) {
            if (sensores.enfrente) {
               indice_tramo++;
               ruta_nav.clear();
               return INSTALL;
            } else
               return IDLE; 
         } else
            return TURN_SL;
      } else {
         ubicacion posicion_destino; 
         posicion_destino.f = casilla_tecnico.fil; 
         posicion_destino.c = casilla_tecnico.col;
         Action act = NavegarHaciaDestinoT(sensores, posicion_destino, zapatillas, mapaResultado, mapaCotas, plan_construccion, indice_tramo + 1, ruta_nav, intentos_bloqueo);
         
         if (intentos_bloqueo > 3 || (act == TURN_SR && ruta_nav.empty() && intentos_bloqueo > 0))
            fase_actual = 0; plan_construccion.clear(); ruta_nav.clear(); intentos_bloqueo = 0;

         return act;
      }
   }

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


