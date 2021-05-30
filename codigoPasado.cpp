#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

struct Registro
{
    char codigo [5];
    char nombre [20];
    char carrera [15];
    int ciclo = 1;
    int next = -1;
    char to = 'm';
};


void printRegistro(Registro reg)
{
    cout << reg.codigo << endl;
    cout << reg.nombre << endl;
    cout << reg.carrera << endl;
    cout << reg.ciclo << endl;
    cout << reg.next << endl;
    cout << reg.to << endl;
    cout << endl;
}

int binarySearchRight(vector<Registro> arr, string toFind)
{
    int left = 0, right = arr.size();
    char charFind[toFind.length()+1];
    strcpy(charFind, toFind.c_str());

    while (left < right)
    {
        int m = (left + right) / 2;

        if (strcmp(arr[m].nombre, charFind) >= 0)
            right = m;
        else
            left = m + 1;
    }

    return right;
}

class Sequential
{
    private:
        string nombre;
        string nombreAux = "auxAdd.txt";
        Registro regAux;
        int startLines;
        // const int lengthCod = sizeof(regAux.codigo)/sizeof(regAux.codigo[0]);
        // const int lengthNom = sizeof(regAux.nombre)/sizeof(regAux.nombre[0]);
        // const int lengthCar = sizeof(regAux.carrera)/sizeof(regAux.carrera[0]);
        const int endline = sizeof(Registro)+2;

        bool static compare(Registro a, Registro b)
        {
            return (strcmp(a.nombre, b.nombre) <= 0);
        }

        Registro readRecord(string nombre_, int pos, bool print = true)
        {
            ifstream file;
            file.open(nombre_);
            Registro registro;
            file.seekg(pos * endline, ios::beg);      
            file.read((char*) &registro, sizeof(Registro));
            if (print) printRegistro(registro);
            file.close();
            return registro;
        };


    public:
        Sequential(string nombre)
        {
            this->nombre = nombre;
            fstream file;
            file.open(nombre, fstream::in);
            file.seekg(0, file.end);
            startLines = file.tellg()/endline;
            file.close();
        }
        ~Sequential(){};

        vector<Registro> load(string nombre_, bool print = true)
        {
            vector<Registro> registros;
            ifstream file;
            file.open(nombre_);
            file.seekg(0, ios::end);
            int lines = file.tellg()/endline;
            for (int i = 0; i < lines; ++i)
            {
                Registro reg = readRecord(nombre_, i, print);
                if (reg.next != -2) registros.emplace_back(reg);
            }
            file.close();
            return registros;
        }

        void insertAll(vector<Registro> registros)
        {
            fstream file;
            file.open(nombre, ios::in | ios::out);
            sort(registros.begin(), registros.end(), compare);
            for (int i = 0; i < registros.size(); ++i)
            {
                if (i != registros.size()-1) 
                {
                    registros[i].next = i+1;
                }
                file.write((char*) &registros[i], sizeof(registros[i]));
                file.write("\n", sizeof("\n"));
            }
            file.close();
        };

        vector<Registro> search(string key)
        {
            vector<Registro> registros = load(nombre, false);
            vector<Registro> registrosAux = load(nombreAux, false);
            vector<Registro> registrosEncontrados;
            char charKey[key.length()+1];
            strcpy(charKey, key.c_str()); 
            int pos = binarySearchRight(registros, key);
            for (int i = pos; i < registros.size(); ++i)
            {
                if (strcmp(registros[i].nombre, charKey) != 0) break;
                printRegistro(registros[i]);
                registrosEncontrados.push_back(registros[i]);
            }
            
            for (int i = 0; i < registrosAux.size(); ++i)
            {
                if (strcmp(registrosAux[i].nombre, charKey) == 0)
                {
                    printRegistro(registrosAux[i]);
                    registrosEncontrados.push_back(registrosAux[i]);
                }
            }
            return registrosEncontrados;
        };

        vector<Registro> search(string begin, string end)
        {
            vector<Registro> registros = load(nombre, false);
            vector<Registro> registrosAux = load(nombreAux, false);
            vector<Registro> registrosEncontrados;
            char charEnd[end.length()+1];
            char charBeg[begin.length()+1];
            strcpy(charEnd, end.c_str()); 
            strcpy(charBeg, begin.c_str()); 
            int pos = binarySearchRight(registros, begin);
            for (int i = pos; i < registros.size(); ++i)
            {
                if (strcmp(registros[i].nombre, charEnd) > 0) break;
                printRegistro(registros[i]);
                registrosEncontrados.push_back(registros[i]);
            }
            for (int i = 0; i < registrosAux.size(); ++i)
            {
                if ((strcmp(registrosAux[i].nombre, charBeg) >= 0)  
                    && (strcmp(registrosAux[i].nombre, charEnd) <= 0))
                {
                    printRegistro(registrosAux[i]);
                    registrosEncontrados.push_back(registrosAux[i]);
                }
            }            
            return registrosEncontrados;
        };

        void add(Registro registro)
        {
            vector<Registro> registrosMain = load(nombre, false);
            vector<Registro> registrosAux = load(nombreAux, false);
            fstream fileAux;
            fileAux.open(nombreAux, ios::in | ios::app);
            fileAux.seekg(0, ios::end);
            fstream fileMain;
            fileMain.open(nombre, ios::in | ios::app);
            int pos = binarySearchRight(registrosMain, registro.nombre); 

            if (strcmp(registrosMain[pos].nombre, registro.nombre) <= 0)
            {
                if (registrosAux.size() == 0)
                {  
                    int i;
                    for (i = pos; i < registrosMain.size(); ++i) 
                    {
                        if (strcmp(registrosMain[i].nombre, registro.nombre) != 0) break;
                    }
                    --i;
                    auto temp = make_pair(registrosMain[i].next, registrosMain[i].to);
                    registrosMain[i].next = 0;
                    registrosMain[i].to = 'a';
                    registro.next = temp.first;
                    registro.to = temp.second;
                    // Añadir al aux
                    fileAux.write((char*) &registro, sizeof(registro));
                    fileAux.write("\n", sizeof("\n"));
                    // Sobreescribir el del main
                    fileMain.seekg(i * endline, ios::beg);
                    fileMain.write((char*) &registrosMain[i], sizeof(registrosMain[i]));
                }
                else
                {
                    int modifyPos = pos;
                    for (modifyPos = pos; modifyPos < registrosMain.size(); ++modifyPos) 
                    {
                        if (strcmp(registrosMain[modifyPos].nombre, registro.nombre) != 0) break;
                    }
                    Registro prev = registrosMain[pos];
                    string x = (registrosMain[pos].to == 'a' ? nombreAux : nombre);
                    Registro current = readRecord(x, registrosMain[pos].next);
                    while (true)
                    {
                        x = (current.to == 'a' ? nombreAux : nombre);
                        prev = current;
                        Registro current = readRecord(x, current.next);   

                        if (strcmp(current.nombre, registro.nombre) > 0)
                        {
                            auto temp = make_pair(prev.next, prev.to);
                            prev.next = registrosAux.size();
                            prev.to = 'a';
                            registro.next = temp.first;
                            registro.to = temp.second;
                            // Añadir al aux
                            fileAux.write((char*) &registro, sizeof(registro));
                            fileAux.write("\n", sizeof("\n"));
                            // Sobreescribir el del main
                            fileMain.seekg((modifyPos - 1) * endline, ios::beg);
                            fileMain.write((char*) &prev, sizeof(prev));
                            break;
                        }
                        
                        if (current.next == -1) 
                        {
                            current.next = registrosAux.size();
                            current.to = 'a';
                            // Añadir al aux
                            fileAux.write((char*) &registro, sizeof(registro));
                            fileAux.write("\n", sizeof("\n"));
                            // Sobreescribir el aux
                            fileAux.seekg((registrosAux.size() - 1) * endline, ios::beg);
                            fileAux.write((char*) &current, sizeof(current));
                            break;                   
                        }
                    }
                }
            }
            else
            {
                if (registrosAux.size() == 0)
                {  
                    registro.next = 0;
                    registro.to = 'm';
                    // Añadir al aux
                    fileAux.write((char*) &registro, sizeof(registro));
                    fileAux.write("\n", sizeof("\n"));
                }
                else
                {
                    int compareNext = 0;
                    char compareTo = 'm';
                    for (int i = 0; i < registrosAux.size(); ++i)
                    {
                        if (registrosAux[i].to == 'a') compareTo = 'a';
                        if (registrosAux[i].next > compareNext) compareNext = registrosAux[i].next;
                    }

                    for (int i = 0; i < registrosAux.size(); ++i)
                    {
                        if (registrosAux[i].next = compareNext && registrosAux[i].to == compareTo)
                        {
                            registro.next = i;
                            registro.to = 'a';
                            // Añadir al aux
                            fileAux.write((char*) &registro, sizeof(registro));
                            fileAux.write("\n", sizeof("\n"));
                            break;
                        }
                    }
                }
            }
            fileAux.close(); 
            fileMain.close(); 
        };

        bool delete_(string key)
        {
            // vector<Registro> registrosAux = load(nombreAux, false);            
        };

};


int main()
{
    Sequential seq("seqFile.txt");
    vector<Registro> registros;
    Registro reg;
    strcpy(reg.codigo, "77740");
    strcpy(reg.nombre, "corresbruguelautaro0");
    strcpy(reg.carrera, "cienciadelacom");
    reg.ciclo = 1;
    Registro reg2;
    strcpy(reg2.codigo, "99990");
    strcpy(reg2.nombre, "aorresbruguelautaro0");
    strcpy(reg2.carrera, "cienciadelacomp");
    reg2.ciclo = 5;
    Registro reg3;
    strcpy(reg3.codigo, "19990");
    strcpy(reg3.nombre, "borresbruguelautaro0");
    strcpy(reg3.carrera, "cienciadelacomp");
    reg3.ciclo = 3;
    Registro reg4;
    strcpy(reg4.codigo, "99990");
    strcpy(reg4.nombre, "aaaresbruguelautaro0");
    strcpy(reg4.carrera, "cienciadelacomp");
    reg4.ciclo = 5;
    Registro reg5;
    strcpy(reg5.codigo, "9999");
    strcpy(reg5.nombre, "aaresbruguelautaro0");
    strcpy(reg5.carrera, "cienciadelacomp");
    reg5.ciclo = 5;
    registros.push_back(reg);
    registros.push_back(reg2);
    registros.push_back(reg3);
    registros.push_back(reg3);
    registros.push_back(reg4);
    registros.push_back(reg5);
    for (int i = 0; i < registros.size(); ++i)
    {
        printRegistro(registros[i]);
    }

    // INSERTAR TODOS
    // cout<<"Insertar todos"<<endl;
    // seq.insertAll(registros);
    // cout<<"---------------------------"<<endl;

    // // BÚSQUEDA EXACTA (IMPRIME REPETIDOS)
    // cout<<"Búsqueda exacta"<<endl;
    // seq.search("borresbruguelautaro");
    // cout<<"---------------------------"<<endl;
    
    // // BÚSQUEDA POR RANGOS (IMPRIME REPETIDOS)
    // cout<<"Busqueda por rangos"<<endl;
    // seq.search("aaresbruguelautaro", "borresbruguelautaro");
    // cout<<"---------------------------"<<endl;

    // INSERTAR UN REGISTRO
    // Registro reg6;
    // strcpy(reg6.codigo, "1236");
    // strcpy(reg6.nombre, "aaaaesbruguelautaro");
    // strcpy(reg6.carrera, "cienciadelacom");
    // Registro reg7;
    // strcpy(reg7.codigo, "2345");
    // strcpy(reg7.nombre, "aaaaasbruguelautaro");
    // strcpy(reg7.carrera, "cienciadelacom");

    // seq.add(reg6);
    // seq.add(reg7);
    // cout<<"IMPRESION DEL add aux"<<endl;
    // seq.load("auxAdd.txt");
    // cout<<"---------------------------"<<endl;    
    // cout<<"IMPRESION DEL seqFile"<<endl;
    // seq.load("seqFile.txt");
    


    
    //-----------------------------------------------------

    // cout<<"Leido"<<endl<<"---------------------------"<<endl;
    // ifstream file("seqFile.txt");
    // Registro registro;
    // file.read((char *) &registro, sizeof(Registro));
    // printRegistro(registro);
    // file.close();

    // sort(registros.begin(), registros.end(), compare);
    // quickSort(registros,0,registros.size()-1);
    
    // for(auto it : registros){
    //     printRegistro(it);
    // }

    return 0;
};