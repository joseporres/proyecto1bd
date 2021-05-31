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
    int ciclo = 0;
    int next = -1;
    char toNext = 'm';
    int prev = -1;
    char toPrev = 'm';
    int nextDel = -1; 
    char toDel = 'm';
};


void printRegistro(Registro reg)
{
    cout << reg.codigo << endl;
    cout << reg.nombre << endl;
    cout << reg.carrera << endl;
    cout << reg.ciclo << endl;
    cout << reg.next << endl;
    cout << reg.toNext << endl;
    cout << reg.prev << endl;
    cout << reg.toPrev << endl;
    cout << endl;
}

void printRegistros(vector<Registro> vec)
{
    for (int i = 0; i < vec.size(); ++i)
    {
        printRegistro(vec[i]);
    }
}

int binarySearch(vector<Registro> arr, string toFind)
{
    int left = 0, right = arr.size();
    char charFind[toFind.length()+1];
    strcpy(charFind, toFind.c_str());

    while (left <= right)
    {
        int m = left + (right - left) / 2;

        if (arr[m].nombre == toFind)
            return m;
        if (strcmp(arr[m].nombre, charFind) < 0)
            left = m + 1;
        else
            right = m - 1;
    }

    return -1;
}

class Sequential
{
    private:
        string nombre;
        string nombreAux = "auxAdd.txt";
        // Registro regAux;
        // const int lengthCod = sizeof(regAux.codigo)/sizeof(regAux.codigo[0]);
        // const int lengthNom = sizeof(regAux.nombre)/sizeof(regAux.nombre[0]);
        // const int lengthCar = sizeof(regAux.carrera)/sizeof(regAux.carrera[0]);
        // const int endline = sizeof(Registro);

        bool static compare(Registro a, Registro b)
        {
            return (strcmp(a.nombre, b.nombre) <= 0);
        }

        // solo actualiza el prev y el next
        void actualizeRegisters(Registro prev, Registro registro, Registro next)
        {
            // actualizamos el prev
            fstream filePrev;
            filePrev.open(registro.toPrev == 'm' ? nombre : nombreAux, ios::in | ios::out | ios::binary);
            filePrev.seekg(registro.prev * sizeof(Registro), ios::beg);
            filePrev.write((char*) &prev, sizeof(Registro));
            filePrev.close();
            // actualizamos el next
            fstream fileNext;
            fileNext.open(registro.toNext == 'm' ? nombre : nombreAux, ios::in | ios::out | ios::binary);
            fileNext.seekg(registro.next * sizeof(Registro), ios::beg);
            fileNext.write((char*) &next, sizeof(Registro));
            fileNext.close();
        }

        void modifyOnDeleteRegisters(vector<Registro> registros, int pos, Registro header)
        {
            // declaración de registros
            Registro prev = registros[pos-1];
            Registro reg = registros[pos];
            Registro next = registros[pos+1];
            pair<int, char> temp = make_pair(next.prev, next.toPrev);
            // Modificacion del registro a borrar
            reg.nextDel = header.nextDel;
            reg.toDel = header.toDel;
            // Modificacion de la cabecera
            header.nextDel = pos;
            header.toDel = next.toPrev;
            fstream fileHeader;
            fileHeader.open(nombre, ios::in | ios::out | ios::binary);
            fileHeader.seekg(0, ios::beg);
            fileHeader.write((char*) &header, sizeof(Registro));
            fileHeader.close();
            // Modificacion del prev
            prev.next = reg.next;
            prev.toNext = reg.toNext;
            // Modificacion del next
            next.prev = reg.prev;
            next.toPrev = reg.toPrev;
            // Actualizar en los archivos
            fstream fileCurrent;
            fileCurrent.open(temp.second == 'm' ? nombre : nombreAux, ios::in | ios::out | ios::binary);
            fileCurrent.seekg(temp.first * sizeof(Registro), ios::beg);
            fileCurrent.write((char*) &reg, sizeof(Registro));
            actualizeRegisters(prev, reg, next);
        }

        void modifyRegisters(Registro prev, Registro registro, Registro next)
        {
            pair<int, char> temp = make_pair(prev.next, prev.toNext);
            fstream fileAux;
            fileAux.open(nombreAux, ios::in | ios::out | ios::binary);
            fileAux.seekg(0, ios::end);
            // En el anterior
            prev.next   = fileAux.tellg();
            prev.toNext = 'a';
            // En el nuevo
            registro.prev   = next.prev;
            registro.toPrev = next.toPrev;
            registro.next   = temp.first;
            registro.toNext = temp.second;
            fileAux.write((char*) &registro, sizeof(Registro));                        
            // En el siguiente
            next.prev   = prev.next;
            next.toPrev = prev.toNext;                
            fileAux.close();
            // Actualizamos los registros modificados
            actualizeRegisters(prev, registro, next);
        }

    public:
        Sequential(string nombre)
        {
            this->nombre = nombre;
            fstream file;
            file.open(nombre, ios::in | ios::out | ios::binary);
            file.seekg(0, ios::end);
            Registro reg;
            strcpy(reg.codigo, "1111");
            strcpy(reg.nombre, "cabeceraaaaaaaaaaaa");
            strcpy(reg.carrera, "cienciadelacomp");
            if (file.tellg() == 0)
                file.write((char*) &reg, sizeof(reg));
            file.close();
        }
        ~Sequential(){};

        Registro readRecord(string nombre_, int pos, bool print = true)
        {
            fstream file;
            file.open(nombre_, ios::in | ios::out | ios::binary);
            Registro registro;
            file.seekg((pos) * sizeof(Registro), ios::beg);      
            file.read((char*) &registro, sizeof(Registro));
            if (print) printRegistro(registro);
            file.close();
            return registro;
        };

        vector<Registro> load(string nombre_, bool print = true)
        {
            vector<Registro> registros;
            fstream file;
            file.open(nombre_, ios::in | ios::out | ios::binary);
            file.seekg(0, ios::end);
            int lines = file.tellg()/sizeof(Registro);
            file.close();
            if (nombre_ == nombreAux) registros.emplace_back(readRecord(nombre_, 0, print));
            for (int i = 1; i < lines; ++i)
            {
                Registro reg = readRecord(nombre_, i, print);
                registros.emplace_back(reg);
            }
            return registros;
        }

        vector<Registro> loadAll(bool print = true)
        {
            vector<Registro> registros;
            // consigo la cabecera
            Registro reg = readRecord(nombre, 0, print);

            while (reg.next != -1)
            {
                if (reg.toNext == 'm')
                {
                    reg = readRecord(nombre, reg.next+1, print);
                }
                else
                {
                    reg = readRecord(nombreAux, reg.next, print);
                }
                registros.push_back(reg);
            }

            return registros;
        }

        void insertAll(vector<Registro> registros)
        {
            fstream fileAux;
            fileAux.open(nombreAux, ios::in | ios::out | ios::binary);
            fileAux.seekg(0, ios::end);
            int x = fileAux.tellg();
            fileAux.close();
            if (x == 0)
            {
                fstream file;
                file.open(nombre, ios::in | ios::out | ios::binary);
                sort(registros.begin(), registros.end(), compare);
                file.seekg(1 * sizeof(Registro), ios::beg);
                for (int i = 0; i < registros.size(); ++i)
                {
                    if (i < registros.size()-1) 
                    {
                        registros[i].next = i+1;
                    }
                    if (i > 0)
                    {
                        registros[i].prev = i-1;
                    }
                    file.write((char*) &registros[i], sizeof(Registro));
                }
                file.seekg(0, ios::beg);
                file.write((char*) &registros[0], sizeof(Registro));
                file.close();
            }

        };

        vector<Registro> search(string key)
        {
            vector<Registro> registros = load(nombre, false);
            vector<Registro> registrosAux = load(nombreAux, false);
            vector<Registro> registrosEncontrados;
            char charKey[key.length()+1];
            strcpy(charKey, key.c_str()); 

            int pos = binarySearch(registros, key);
            if (pos != -1)
            {
                // el actual
                printRegistro(registros[pos]);
                registrosEncontrados.push_back(registros[pos]);
                // ir hacia adelante
                for (int i = pos+1; i < registros.size(); ++i)
                {
                    if (strcmp(registros[i].nombre, charKey) != 0) break;
                    printRegistro(registros[i]);
                    registrosEncontrados.push_back(registros[i]);
                }
                // ir hacia atrás
                for (int i = pos-1; i >= 0; --i)
                {
                    if (strcmp(registros[i].nombre, charKey) != 0) break;
                    printRegistro(registros[i]);
                    registrosEncontrados.push_back(registros[i]);
                }
            }

            // linear en el archivo auxiliar
            for (int i = 0; i < registrosAux.size(); ++i)
            {
                // si lo encuentra
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
            char charBeg[begin.length()+1];
            strcpy(charBeg, begin.c_str()); 
            char charEnd[end.length()+1];
            strcpy(charEnd, end.c_str()); 

            int pos = binarySearch(registros, begin);
            if (pos != -1)
            {
                // el actual
                printRegistro(registros[pos]);
                registrosEncontrados.push_back(registros[pos]);
                // ir hacia adelante
                for (int i = pos+1; i < registros.size(); ++i)
                {
                    if (strcmp(registros[i].nombre, charEnd) > 0) break;
                    printRegistro(registros[i]);
                    registrosEncontrados.push_back(registros[i]);
                }
                // ir hacia atrás
                for (int i = pos-1; i >= 0; --i)
                {
                    if (strcmp(registros[i].nombre, charBeg) != 0) break;
                    printRegistro(registros[i]);
                    registrosEncontrados.push_back(registros[i]);
                }
            }

            // linear en el archivo auxiliar
            for (int i = 0; i < registrosAux.size(); ++i)
            {
                // si está en el rango
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
            // CASO 1: INSERTAR EN UN REGISTRO ELIMINADO USANDO LIFO
            fstream file;
            Registro reg;
            file.open(nombre, ios::in | ios::out | ios::binary);
            file.seekg(0, ios::beg);
            file.read((char*) &reg, sizeof(Registro));
            do
            {
                Registro prev;
                if (reg.toPrev == 'm')
                {
                    prev = readRecord(nombre, reg.prev+1, false);
                }
                else
                {
                    prev = readRecord(nombreAux, reg.prev, false);                    
                }
                Registro next;
                if (reg.toNext == 'm')
                {
                    next = readRecord(nombre, reg.next+1, false);
                }
                else
                {
                    next = readRecord(nombreAux, reg.next, false);                    
                }


                if (strcmp(registro.nombre, prev.nombre) >= 0 
                    && strcmp(registro.nombre, next.nombre) <= 0)
                {
                    // escribo
                    fstream fileToWrite;
                    fileToWrite.open(reg.toDel == 'm' ? nombre : nombreAux, ios::in | ios::out | ios::binary);
                    fileToWrite.seekg(reg.nextDel * sizeof(Registro), ios::beg);
                    fileToWrite.write((char*) &registro, sizeof(Registro));
                    fileToWrite.close();
                    file.close();
                    return;
                }
                reg = readRecord(reg.toDel == 'm' ? nombre : nombreAux, reg.nextDel, false);
            } while (reg.nextDel != -1); 
            file.close();

            // CASO 2: INSERTAR UN REGISTRO EN EL AUX
            // load a todos los registros de ambos archivos ordenados por sus "punteros".
            vector<Registro> registros = loadAll(false);
            string s(registro.nombre);
            int pos = binarySearch(registros, s);
            cout << s << pos << endl;
            // si lo encuentra
            if (pos != -1)
            {
                for (int i = pos+1; i < registros.size(); ++i)
                {
                    if (strcmp(registros[i].nombre, registro.nombre) != 0)
                    {
                        Registro prev = registros[i-1];
                        Registro next = registros[i];
                        modifyRegisters(prev, registro, next);
                        return;
                    }
                }
            }
            else
            {
                for (int i = 0; i < registros.size(); ++i)
                {
                    Registro prev = registros[i];
                    Registro next = registros[i+1];

                    if (i+1 < registros.size())
                    {
                        if (strcmp(registro.nombre, prev.nombre) >= 0 
                            && strcmp(registro.nombre, next.nombre) <= 0)
                        {
                            modifyRegisters(prev, registro, next);
                            return;
                        }
                    }
                    else
                    {
                        if (strcmp(registro.nombre, next.nombre) >= 0)
                        {
                            // consigo el prev de mi prev
                            Registro prevPrev;
                            fstream filePrevPrev;
                            if (prev.toPrev == 'm')
                            {
                                filePrevPrev.open(nombre, ios::in | ios::out | ios::binary);
                                filePrevPrev.seekg((prev.prev + 1) * sizeof(Registro), ios::beg);
                            }
                            else
                            {
                                filePrevPrev.open(nombreAux, ios::in | ios::out | ios::binary);
                                filePrevPrev.seekg((prev.prev) * sizeof(Registro), ios::beg);
                            }
                            filePrevPrev.read((char*) &prevPrev, sizeof(Registro));
                            filePrevPrev.close();

                            fstream fileAux;
                            fileAux.open(nombreAux, ios::in | ios::out | ios::binary);
                            fileAux.seekg(0, ios::end);
                            // En el siguiente
                            prev.next   = fileAux.tellg();
                            prev.toNext = 'a';
                            // En el nuevo
                            registro.prev   = prevPrev.next;
                            registro.toPrev = prevPrev.toNext;
                            fileAux.write((char*) &registro, sizeof(Registro));                                    
                            fileAux.close();

                            fstream filePrev;
                            if (registro.toPrev == 'm')
                            {
                                filePrev.open(nombre, ios::in | ios::out | ios::binary);
                                filePrev.seekg((registro.prev + 1) * sizeof(Registro), ios::beg);
                                filePrev.write((char*) &prev, sizeof(Registro));
                            }
                            else
                            {
                                filePrev.open(nombreAux, ios::in | ios::out | ios::binary);
                                filePrev.seekg((registro.prev) * sizeof(Registro), ios::beg);
                                filePrev.write((char*) &prev, sizeof(Registro));
                            }
                            filePrev.close();
                            // printRegistro(prev);
                            // printRegistro(registro);
                            return;
                        }
                    }

                }
            }
        };

        // si deleteAll es false se borra el primero que encuentre con esa key, caso contrario se borran todos.
        bool delete_(string key, bool deleteAll = false)
        {
            vector<Registro> registros = loadAll(false);
            int pos = binarySearch(registros, key);
            if (pos == -1) return false;
            
            fstream fileHeader;
            Registro header;
            fileHeader.open(nombre, ios::in | ios::out | ios::binary);
            fileHeader.seekg(0, ios::beg);
            fileHeader.read((char*) &header, sizeof(Registro));  
            fileHeader.close();

            if (!deleteAll)
            {
                modifyOnDeleteRegisters(registros, pos, header);
                return true;
            }
        
            modifyOnDeleteRegisters(registros, pos, header);
            char charKey[key.length()+1];
            strcpy(charKey, key.c_str());             
            // ir hacia adelante
            for (int i = pos+1; i < registros.size(); ++i)
            {
                if (strcmp(registros[i].nombre, charKey) != 0) break;
                modifyOnDeleteRegisters(registros, i, header);
            }
            // ir hacia atrás
            for (int i = pos-1; i > 0; --i)
            {
                if (strcmp(registros[i].nombre, charKey) != 0) break;
                modifyOnDeleteRegisters(registros, i, header);
            }
            return true;
        };

};


int main()
{
    Sequential seq("seqFile.txt");
    vector<Registro> registros;
    Registro reg0;
    strcpy(reg0.codigo, "7774");
    strcpy(reg0.nombre, "A");
    strcpy(reg0.carrera, "cienciadelacomp");
    Registro reg1;
    strcpy(reg1.codigo, "7774");
    strcpy(reg1.nombre, "A");
    strcpy(reg1.carrera, "cienciadelacomp");    
    Registro reg2;
    strcpy(reg2.codigo, "7774");
    strcpy(reg2.nombre, "A");
    strcpy(reg2.carrera, "cienciadelacomp");        
    Registro reg3;
    strcpy(reg3.codigo, "7774");
    strcpy(reg3.nombre, "A");
    strcpy(reg3.carrera, "cienciadelacomp");        
    Registro reg4;
    strcpy(reg4.codigo, "9999");
    strcpy(reg4.nombre, "B");
    strcpy(reg4.carrera, "cienciadelacomp");        
    Registro reg5;
    strcpy(reg5.codigo, "9999");
    strcpy(reg5.nombre, "B");
    strcpy(reg5.carrera, "cienciadelacomp");   
    Registro reg6;
    strcpy(reg6.codigo, "9999");
    strcpy(reg6.nombre, "C");
    strcpy(reg6.carrera, "cienciadelacomp");      
    Registro reg7;
    strcpy(reg7.codigo, "8888");
    strcpy(reg7.nombre, "D");
    strcpy(reg7.carrera, "cienciadelacomp");    
    registros.push_back(reg0);
    registros.push_back(reg1);
    registros.push_back(reg2);
    registros.push_back(reg3);
    registros.push_back(reg4);
    registros.push_back(reg5);
    registros.push_back(reg6);
    // Lee bien la cabecera creada en el constructor
    // seq.readRecord("seqFile.txt", -1);

    seq.insertAll(registros);
    seq.add(reg7);
    seq.add(reg7);
    // seq.load("seqFile.txt");
    // seq.load("auxAdd.txt");
    // seq.loadAll();
    // seq.search("A");
    // seq.search("A", "C");
    // seq.search("A", "B");
    // seq.load("seqFile.txt");


    return 0;
};