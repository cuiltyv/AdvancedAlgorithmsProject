#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <climits>
#include <cmath>
#include <iomanip>
#include <queue>
#include <cstring>
#include <fstream>
#define MAX 100
using namespace std;


// Actividad Integradora 2
// Carlos Cuilty A01721636
// Eduardo Tello A01721735

typedef pair<int,int> iPair;


ofstream outFile("checking.txt");

//Estructura para guardar colonia
struct Colonia {
    string nombre;
    int x;
    int y;
    int central;
    int indice;

    // Constructor
    Colonia(string nombre, int x, int y, int central, int indice){
        this->nombre = nombre;
        this->x = x;
        this->y = y;
        this->central = central;
        this->indice = indice;
    }
};

struct Edge{
    string origen;
    string destino;
    int costo;

};

//Estructura para guardar grafo
struct Graph{
    int V, E, costMSTKruskal, costoTSP = 0; // number of vertex
    vector<Colonia> colonias;
    vector<pair<int,pair<int,int>>> edges; //First = costo, second conexión
    vector<pair<int,pair<int,int>>> edgesTSP; //First = costo, second conexión para TSP
    vector<string> caminoTSP; //Camino para TSP
    vector<string> tspFinal; //Camino para TSP


    vector<pair<int,int>> selectedEdgesK;

    Graph(int V, int E){
        this->V = V;
        this->E = E;
        costMSTKruskal = 0;

    }

    // Función para agregar aristas
    void addEdge(int u, int v, int w){
        edges.push_back({w, {u, v}}); //First = costo, second conexión
        edgesTSP.push_back({w, {u, v}}); //First = costo, second conexión

    }


    //Complejidad O(1)
    void addColonia(string nombre, int x, int y, int esCentral){
        //cout << "colonias: " << colonias.size() << "     colTSP: " << coloniasTSP.size() << endl;
        Colonia colonia(nombre,x,y,esCentral,colonias.size());
        colonias.push_back(colonia);
    }

    void crearMatrizTSPNoCentrales(int matAdj[MAX][MAX]);
    void load(int mat[MAX][MAX], int matFloyd[MAX][MAX]);
    void kruskalMST();
    void printEdgesK();
    void conexionNice(string origen, string destino);
    string minDist(string colonia, int posx, int posy);
    void floyd(int mat[MAX][MAX], int p[MAX][MAX]);
    int tsp(int mat[MAX][MAX]);
    void printMat(int matAdj[MAX][MAX]);
    void validarTSP(int mat[MAX][MAX], int p[MAX][MAX]);
    void printTSP();
    void consultasFloyd();
    int encontrarColonia(string origen);
    void checaTrayectoria(int p[MAX][MAX], int x, int y);
    void checaTrayectoriaFloyd(int p[MAX][MAX], int x, int y);
    void consultas(int mat[MAX][MAX], int p[MAX][MAX]);
    int getCostoEdge(int a, int b);




};

struct DisjointSets{
    int *parent, *rnk;
    int n;
    DisjointSets(int n){
        this->n = n;
        parent = new int[n+1];
        rnk = new int[n+1];
        for (int i=0; i<=n; i++){
            rnk[i] = 0;
            parent[i] = i;
        }
    }
    // Para encontrar el parent de 'u'
    int find(int u){
        if (u != parent[u]){
            parent[u] = find(parent[u]);
        }
        return parent[u];
    }

    // Union por rank
    void merge(int x, int y){
        x = find(x);
        y = find(y);
        if (rnk[x] > rnk[y]){
            parent[y] = x;
        }
        else{
            parent[x] = y;
        }
        if (rnk[x] == rnk[y]){
            rnk[y]++;
        }
    }
};



// Complejidad O(n)
int Graph::encontrarColonia(string origen) {

    for(int i = 0; i < colonias.size();i++){
        if(colonias[i].nombre == origen){
            return colonias[i].indice;
        }
    }

    return -1;
}



// Complejidad O(e)
void Graph::conexionNice(string origen, string destino){

    int a = encontrarColonia(origen);
    int b = encontrarColonia(destino);


    for(int i = 0; i < edges.size(); i++){
        if((edges[i].second.first == a && edges[i].second.second == b) || (edges[i].second.first == b && edges[i].second.second == a)){
            edges[i].first = 0;
        }
    }



}

// Complejidad O(n)
string Graph::minDist(string colonia, int posx, int posy){
    int valorMin = INT_MAX;
    string conexion = "";

    for(int i = 0; i < colonias.size(); i++){
        if(valorMin > sqrt((posx-colonias[i].x)*(posx-colonias[i].x) + (posy-colonias[i].y)*(posy-colonias[i].y))){
            valorMin = sqrt((posx-colonias[i].x)*(posx-colonias[i].x) + (posy-colonias[i].y)*(posy-colonias[i].y));
            conexion = colonias[i].nombre;


        }
    }

    //addColonia(colonia, posx, posy, 0);

    return conexion;

}




// Complejidad O(e)
void Graph::load(int mat[MAX][MAX], int matFloyd[MAX][MAX]){
    string origen, destino;
    int peso;
    int a, b, c, d;


    for (int i=0; i<E; i++){
        cin >> origen >> destino >> peso;

        a = encontrarColonia(origen);
        b = encontrarColonia(destino);


        //tsp
        mat[a][b] = peso;
        mat[b][a] = peso;





        matFloyd[a][b] = matFloyd[b][a] = peso;

        if(a != -1 && b != -1){
            addEdge(a, b, peso);
            addEdge(b, a, peso);
        }

    }




}




// Complejidad O(e log e)
void Graph::kruskalMST(){
    sort(edges.begin(), edges.end());
    DisjointSets ds(V);
    costMSTKruskal = 0;
    for(auto it:edges){
        // cada casilla del find es (w, (u, v))
        int p1 = ds.find(it.second.first);
        int p2 = ds.find(it.second.second);
        if(p1 != p2){
            costMSTKruskal += it.first;
            selectedEdgesK.push_back(it.second);
            ds.merge(it.second.first, it.second.second);
        }
    }
}

// Complejidad O(n) n = tamaño de selectedEdgesK
void Graph::printEdgesK(){
    //cout << "Selected edges kruskal: ";
    int costo = 0;
    for(auto it:selectedEdgesK){
        costo = getCostoEdge(it.first, it.second);
        if(costo != 0){
            outFile << colonias[it.first].nombre << " - " << colonias[it.second].nombre << " " << costo << endl;
            cout << colonias[it.first].nombre << " - " << colonias[it.second].nombre << " " << costo << endl;
        }
        //cout << "{" << colonias[it.first].nombre << ": " << it.second << ", " << colonias[it.second].nombre << ": " << it.second <<  "} ";
    }
    outFile << endl;
    cout << endl;
}

// Complejidad O(n)
int Graph::getCostoEdge(int a, int b){


    for(int i = 0; i < edges.size(); i++){
        if((edges[i].second.first == a && edges[i].second.second == b) || (edges[i].second.first == b && edges[i].second.second == a)){
            return edges[i].first;
        }
    }
    return 0;
}

// Complejidad O(n^3)
void Graph::floyd(int mat[MAX][MAX], int p[MAX][MAX]){

    for(int k=0; k<V; k++){
        for(int i=0; i<V; i++){
            for(int j = 0; j<V; j++){
                if(mat[i][k] != INT_MAX && mat[k][j] != INT_MAX  && (mat[i][k] + mat[k][j]) < mat[i][j]) {
                    mat[i][j] = mat[i][k] + mat[k][j];
                    p[i][j] = k;
                }
            }
        }
    }
}

// Complejidad O(n^2)
void iniciaMat(int mat[MAX][MAX], int matFloyd[MAX][MAX], int p[MAX][MAX]){
    for(int i = 0; i < MAX;i++){
        mat[i][i] = 0;
        for(int j = i+1; j < MAX;j++){
            mat[i][j] = INT_MAX;
            mat[j][i] = INT_MAX;
        }
    }

    for(int i=0; i<MAX; i++){
        matFloyd[i][i] = 0;
        p[i][i] = -1; //-1 es conexion directa
        for(int j=i+1; j<MAX; j++){
            matFloyd[i][j] = matFloyd[j][i] = INT_MAX;
            p[i][j] = p[j][i] = -1;
        }
    }
}

struct nodo{
    int niv; // Nivel del nodo dentro del arbol
    int costoAcum; // Costo acumulado hasta ese punto
    int costoPos; // Costo posible de esa trayectoria
    int verticeAnterior; // Vertice anterior
    int verticeActual; // Vertice actual
    bool visitados[MAX] = {false}; // Arreglo de visitados
    bool operator<(const nodo &otro) const{ //Para fila priorizada
        return costoPos >= otro.costoPos; //Costo Posible menor
    }
    vector<int> camino;
};



// Complejidad O(n^2)
void calculaCostoPosible(nodo &nodoActual, int mat[MAX][MAX], int n){
    nodoActual.costoPos = nodoActual.costoAcum;
    int costoObtenido = INT_MAX;


    for(int i = 0; i < n; i++){
        // Si no he visitado al nodo "i" o es el ultimo que visité
        if (!nodoActual.visitados[i] || i == nodoActual.verticeActual){
            if(!nodoActual.visitados[i]){
                for(int j = 0; j < n; j++){
                    if(i != j && (!nodoActual.visitados[j] || j == 1)){
                        // if(isCentral[i] && isCentral[j]){
                        //cout << "entra" << endl;

                        costoObtenido = min(costoObtenido, mat[i][j]);
                    }
                }
            }
            else{  // El nodo i es el ultimo visitado
                for(int j = 0; j < n; j++){
                    if(!nodoActual.visitados[j]){
                        // cout << mat[i][j] << endl;
                        costoObtenido = min(costoObtenido, mat[i][j]);
                    }
                }

            }

        }

        if(costoObtenido == INT_MAX){
            nodoActual.costoPos = INT_MAX;
        }
        else{
            nodoActual.costoPos += costoObtenido;
        }

    }
}






// Complejidad O(n)
void Graph::validarTSP(int mat[MAX][MAX], int p[MAX][MAX]){



    int primerNoCentral = -1;
    vector<string> temp;

    for(int i = 0; i < caminoTSP.size()-1; i++){
        if(caminoTSP[i] != caminoTSP[i+1]){
            temp.push_back(caminoTSP[i]);
        }
    }


    caminoTSP = temp;




    int a, b, izq = 0, der = 1;




    //DOUBLE POINTER IZQ Y DERECHA
    while(der < caminoTSP.size()) {
        a = encontrarColonia(caminoTSP[izq]) + 1;
        b = encontrarColonia(caminoTSP[der]) + 1;

        //cout << colonias[a-1].nombre << " " << colonias[b-1].nombre << endl;

        if(primerNoCentral == -1 && colonias[a-1].central == 0){
            primerNoCentral = a-1;
        }

        if(colonias[a-1].central == 0 && colonias[b-1].central == 0){


            costoTSP += mat[a - 1][b - 1];
            //cout << " Trayectoria: " << colonias[a-1].nombre << "-";
            tspFinal.push_back(colonias[a - 1].nombre);
            checaTrayectoria(p, a -1, b - 1);

            izq = der;
            der ++;

        }
        else if(colonias[a-1].central == 0 && colonias[b-1].central == 1){
            der++;
        }
        else if(colonias[a-1].central == 1 && colonias[b-1].central == 0){
            izq = der;
            der ++;
        }
        else{
            izq++;
            der++;
        }



    }

    a = encontrarColonia(caminoTSP[izq]) + 1;

    costoTSP += mat[a - 1][primerNoCentral];
    //cout << " Trayectoria: " << colonias[a-1].nombre << "-";
    tspFinal.push_back(colonias[a - 1].nombre);
    checaTrayectoria(p, a -1, primerNoCentral);



    tspFinal.push_back(colonias[primerNoCentral].nombre);



}

// Complejidad O(n)
void Graph::printTSP(){


    outFile << "2 - La ruta mas optima." << endl << endl;
    cout << "2 - La ruta mas optima." << endl << endl;

    for(int i = 0; i < tspFinal.size()-1; i++){
        outFile << tspFinal[i] << " - " ;
        cout << tspFinal[i] << " - " ;
    }
    outFile << tspFinal[tspFinal.size()-1];
    cout << tspFinal[tspFinal.size()-1];

    outFile << endl << endl;
    cout << endl << endl;

    outFile << "La Ruta Optima tiene un costo total de: " << costoTSP << endl << endl;
    cout << "La Ruta Optima tiene un costo total de: " << costoTSP << endl << endl;
}


// Complejidad O(n^2*2^n)
int Graph::tsp(int mat[MAX][MAX]){
    int primero = 0;

    for(int i = 0; i < colonias.size(); i++){
        if(colonias[i].central == 0){
            //cout << colonias[i].nombre << endl;
            primero = i;
            break;
        }
    }

    int n = V;
    int costoOptimo = INT_MAX;
    nodo raiz;
    raiz.niv = 0;
    raiz.costoAcum = 0;
    raiz.verticeActual = primero;
    raiz.camino.push_back(primero);
    raiz.verticeAnterior = 0;
    raiz.visitados[primero] = true;
    calculaCostoPosible(raiz, mat, n);
    //cout << raiz.costoPos << endl;
    priority_queue<nodo> pq;
    pq.push(raiz);
    vector<int> ruta;



    while(!pq.empty()){
        // sacar de pq
        // ver si el costopos < costOptimo
        //Si si, generar todos los posibles hijos de este nodo ( que no hayan sido visitados)
        // para cada hijo generar un nuevo nodo, acutalizar los datos
        // y cuando el nivel sea == n-2 calcular el costo real, y si este mejora el costo optimo
        // entonces actualziar el costoOptimo con el costo real
        //si el nivel < n-2 .. checar el costo posible y si es mejora que el costo optimo
        //entonces meterloa la pq.
        nodo nodoAct = pq.top();
        pq.pop();
        if(nodoAct.costoPos < costoOptimo){
            for(int i = 0; i < n; i++){
                if(!nodoAct.visitados[i]){
                    nodo hijo;
                    hijo.niv = nodoAct.niv+1;
                    hijo.costoAcum = nodoAct.costoAcum + mat[nodoAct.verticeActual][i];
                    hijo.verticeActual = i;
                    memcpy(hijo.visitados, nodoAct.visitados, (n+1) * sizeof(bool));

                    hijo.verticeAnterior = nodoAct.verticeActual;


                    hijo.camino = nodoAct.camino;
                    hijo.camino.push_back(i);
                    hijo.visitados[i] = true;

                    if(nodoAct.niv == n-2){


                        if(hijo.costoAcum + mat[raiz.verticeActual][i] <= costoOptimo && hijo.costoAcum + mat[raiz.verticeActual][i] > 0){
                            //cout << "entra" << endl;

                            //cout << "Costo: " << costoOptimo << endl;
                            hijo.camino.push_back(raiz.verticeActual);
                            ruta = hijo.camino;

                            costoOptimo = hijo.costoAcum + mat[raiz.verticeActual][i];




                        }


                    }
                    else if(nodoAct.niv < n-2){
                        calculaCostoPosible(hijo, mat, n);
                        //cout << hijo.costoPos << endl;
                        if(costoOptimo >= hijo.costoPos){
                            pq.push(hijo);
                        }


                    }







                }
            }
        }

    }

    //for(int vec: ruta){
    //    cout << vec << endl;
    //}

    //aqui
    //int valIni = ruta[0];



    for(int vertice: ruta){
        //cout << vertice << endl;
        //cout << colonias[vertice-1].nombre << " " << endl;;
        caminoTSP.push_back(colonias[vertice].nombre);
    }

    //cout << colonias[valIni-1].nombre << endl;
    //cout << "ates " << endl;
    //caminoTSP.push_back(colonias[valIni-1].nombre);
    //cout << "llega" << endl;


    return costoOptimo;
}


// Complejidad O(n^2)
void Graph::printMat(int matAdj[MAX][MAX]){
    for(int i = 0; i < V;i++){
        for(int j = 0; j < V;j++){
            cout << matAdj[i][j] << ", ";
        }
        cout << endl;
    }
}


// Complejidad O(n^2)
void printFloyd(int mat[MAX][MAX], int p[MAX][MAX], int n){
    cout << "Matriz de Costos:" << endl;
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            if(mat[i][j] == INT_MAX){
                cout << "INF\t";
            }
            else{
                cout << mat[i][j] << "\t";
            }
        }
        cout << endl;
    }
    cout << "Matriz de Trayectorias: " << endl;
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            cout << p[i][j] << "\t";
        }
        cout << endl;
    }
}

// Complejidad O(2^n)
void Graph::checaTrayectoria(int p[MAX][MAX], int x, int y){

    if(p[x][y] != -1){
        checaTrayectoria(p, x, p[x][y]);
        //cout << (p[x][y]+1) << "-";
        tspFinal.push_back(colonias[p[x][y]].nombre);
        checaTrayectoria(p, p[x][y], y);
    }

}


// Complejidad O(n^2)
void Graph::consultas(int mat[MAX][MAX], int p[MAX][MAX]){
    int a, b;
    outFile  << "-------------------" << endl;
    cout << "-------------------" << endl;

    outFile << "3 - Caminos mas cortos entre centrales." << endl << endl;
    cout << "3 - Caminos mas cortos entre centrales." << endl << endl;



    for(int i = 0; i < colonias.size(); i++){
        for(int j = 0; j < colonias.size(); j++){


            if((colonias[i].central == 1 && colonias[j].central == 1) && (colonias[i].indice != colonias[j].indice) && (colonias[i].indice < colonias[j].indice)){
                a = colonias[i].indice+1;
                b = colonias[j].indice+1;

                //cout << a << "   -    " << b << endl;

                if(mat[a-1][b-1] == INT_MAX){
                    outFile << "no path" << endl;
                    cout << "no path" << endl;
                }
                else{
                    //cout << "costo: " << mat[a-1][b-1]
                    outFile << colonias[a-1].nombre << " - ";
                    cout <<  colonias[a-1].nombre << " - ";

                    checaTrayectoriaFloyd(p, a-1, b-1);
                    cout << colonias[b-1].nombre << " (" << mat[a-1][b-1] << ")" << endl << endl;
                    outFile << colonias[b-1].nombre << " (" << mat[a-1][b-1] << ")" << endl << endl;
                }

            }

        }
    }







}

// Complejidad O(1)
void Graph::checaTrayectoriaFloyd(int p[MAX][MAX], int x, int y){

    if(p[x][y] != -1){
        checaTrayectoriaFloyd(p, x, p[x][y]);
        outFile << colonias[(p[x][y])].nombre << " - ";
        cout << colonias[(p[x][y])].nombre << " - ";
        checaTrayectoriaFloyd(p, p[x][y], y);
    }
}



// Complejidad O(n^2*2^n) por llamada a TSP
int main(){

    int n, m, k, q;


    cin >> n >> m >> k >> q;

    Graph g(n,m);
    string nombre;
    int posx, posy;
    int esCentral;

    int matAdj[MAX][MAX];  //tsp
    int matFloyd[MAX][MAX]; //floyd
    int p[MAX][MAX]; //floyd

    for(int i = 0; i < n; i++){
        cin >> nombre >> posx >> posy >> esCentral;
        g.addColonia(nombre, posx, posy, esCentral);
    }
    iniciaMat(matAdj, matFloyd, p);

    g.load(matAdj, matFloyd);
    string origen, destino;
    for(int i = 0; i < k; i++){
        cin >> origen >> destino;
        g.conexionNice(origen, destino);
    }

    outFile << "-------------------" << endl;
    cout << "-------------------" << endl;
    g.kruskalMST();
    outFile << "1 - Cableado optimo de nueva conexion." << endl << endl;
    cout << "1 - Cableado optimo de nueva conexion." << endl << endl;
    g.printEdgesK();
    outFile << "Costo Total: " << g.costMSTKruskal << endl << endl;
    cout << "Costo Total: " << g.costMSTKruskal << endl << endl;
    outFile << "-------------------" << endl;
    cout << "-------------------" << endl;



    //g.printMat(matAdj);

    int ans = g.tsp(matAdj);

    g.floyd(matFloyd, p);



    //printFloyd(matFloyd, p, n);




    g.validarTSP(matFloyd, p);

    //cout << endl << endl;
    g.printTSP();

    //consultas(matFloyd, p, 3);

    g.consultas(matFloyd, p);

    //Distancias
    string res;

    outFile  << "-------------------" << endl;
    cout << "-------------------" << endl;

    outFile << "4 - Conexion de nuevas colonias" << endl << endl;
    cout << "4 - Conexion de nuevas colonias" << endl << endl;
    for(int i = 0; i < q; i++){
        cin >> nombre >> posx >> posy;
        res = g.minDist(nombre, posx, posy);
        outFile << nombre << " debe conectarse con " << res << endl;
        cout << nombre << " debe conectarse con " << res << endl;
    }
    outFile << endl;
    cout << endl;

    outFile  << "-------------------" << endl;
    cout << "-------------------" << endl;
    outFile.close();

    return 0;
}



/*
 *
 * LindaVista = 1
 * Purisima = 2   1
 * Tecnologico = 3
 * Roma = 4    2
 * AltaVista = 5    3
 *
5 8 1 2
LindaVista 200 120 1
Purisima 150 75 0
Tecnologico -50 20 1
Roma -75 50 0
AltaVista -50 40 0
LindaVista Purisima 48
LindaVista Roma 17
Purisima Tecnologico 40
Purisima Roma 50
Purisima AltaVista 80
Tecnologico Roma 55
Tecnologico AltaVista 15
Roma AltaVista 18
Purisima Tecnologico
Independencia 180 -15
Roble 45 68
*/












