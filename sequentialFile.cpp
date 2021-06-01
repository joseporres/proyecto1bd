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


void printRegistro(Registro reg, bool elimData = false)
{
    cout << reg.nombre << endl;
    cout << reg.codigo << endl;
    cout << reg.carrera << endl;
    cout << reg.ciclo << endl;
    cout << reg.next << endl;
    cout << reg.toNext << endl;
    cout << reg.prev << endl;
    cout << reg.toPrev << endl;
    if (elimData)
    {
        cout << reg.nextDel << endl;
        cout << reg.toDel << endl;    
    }
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
        int reconstructFactor = 5;

        void printInfoHeader(Registro reg)
        {
            cout << reg.nombre << endl;
            // inicio de la lista doblemente enlazada (adelante y atrás) de no eliminados
            cout << reg.next << endl;
            cout << reg.toNext << endl;
            // inicio de la lista enlazada (solo adelante) de eliminados LIFO
            cout << reg.nextDel << endl;
            cout << reg.toDel << endl;
            cout << endl;
        }

        bool static compare(Registro a, Registro b)
        {
            return (strcmp(a.nombre, b.nombre) <= 0);
        }

        // solo actualiza el prev y el next
        void actualizeRegisters(Registro prev, Registro registro, Registro next)
        {
            // actualizamos el prev
            fstream filePrev;
            if (registro.toPrev == 'm')
            {
                filePrev.open(nombre, ios::in | ios::out | ios::binary);
                filePrev.seekg((registro.prev+1) * sizeof(Registro), ios::beg);
            }
            else
            {
                filePrev.open(nombreAux, ios::in | ios::out | ios::binary);
                filePrev.seekg(registro.prev * sizeof(Registro), ios::beg);
            }
            filePrev.write((char*) &prev, sizeof(Registro));
            filePrev.close();
            // actualizamos el next
            fstream fileNext;
            if (registro.toNext == 'm')
            {
                fileNext.open(nombre, ios::in | ios::out | ios::binary);
                fileNext.seekg((registro.next+1) * sizeof(Registro), ios::beg);
            }
            else
            {
                fileNext.open(nombreAux, ios::in | ios::out | ios::binary);
                fileNext.seekg(registro.next * sizeof(Registro), ios::beg);
            }
            fileNext.write((char*) &next, sizeof(Registro));
            fileNext.close();
        }

        void modifyRegisters(Registro prev, Registro registro, Registro next)
        {
            pair<int, char> temp = make_pair(prev.next, prev.toNext);
            fstream fileAux;
            fileAux.open(nombreAux, ios::in | ios::out | ios::binary);
            fileAux.seekg(0, ios::end);
            // En el anterior
            prev.next   = fileAux.tellg()/sizeof(Registro);
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

        void modifyRegisters(Registro prev, Registro registro)
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
            prev.next   = fileAux.tellg()/sizeof(Registro);
            prev.toNext = 'a';
            // En el nuevo
            registro.prev   = prevPrev.next;
            registro.toPrev = prevPrev.toNext;

            // printRegistro(prevPrev);
            // printRegistro(prev);
            // printRegistro(registro);
            // cout << "-----------------------------\n";
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
        }

        void actualizeElimPointers(pair<int, char> goToprevElim, Registro prevElim, Registro elim)
        {
            // cout << goToprevElim.first << goToprevElim.second << endl;
            // printRegistro(prevElim);
            // printRegistro(elim);
            // cout << endl;
            // se ha actualizado el head de la lista de eliminados
            if (goToprevElim.first == -1)
            {
                fstream fileHeader;
                Registro header;
                // se cambiar el header por el siguiente
                fileHeader.open(nombre, ios::in | ios::out | ios::binary);
                fileHeader.seekg(0, ios::beg);
                fileHeader.read((char*) &header, sizeof(Registro));
                pair<int, char> temp = make_pair(header.nextDel, header.toDel);
                header.nextDel = elim.nextDel;
                header.toDel = elim.toDel;
                fileHeader.seekg(0, ios::beg);
                fileHeader.write((char*) &header, sizeof(Registro));

                if (elim.prev == -1)
                {
                    header.next   = temp.first;
                    header.toNext = temp.second;
                    fileHeader.seekg(0, ios::beg);
                    fileHeader.write((char*) &header, sizeof(Registro));
                }
                fileHeader.close();
            }
            else
            {
                // se cambiar el prev elim por lo que antes apuntaba el elim, ya que en elim ahora hay un nuevo registro
                prevElim.nextDel = elim.nextDel;
                prevElim.toDel = elim.toDel;
                fstream fileActualize;
                if (goToprevElim.second == 'm')
                {
                    fileActualize.open(nombre, ios::in | ios::out | ios::binary);
                    fileActualize.seekg((goToprevElim.first+1)*sizeof(Registro), ios::beg);
                }
                else
                {
                    fileActualize.open(nombreAux, ios::in | ios::out | ios::binary);
                    fileActualize.seekg((goToprevElim.first)*sizeof(Registro), ios::beg);
                }
                fileActualize.write((char*) &prevElim, sizeof(Registro));
                fileActualize.close();

                if (elim.prev == -1)
                {
                    fstream fileHeader;
                    Registro header;
                    // se actualiza el inicio de la lista de los no eliminados
                    fileHeader.open(nombre, ios::in | ios::out | ios::binary);
                    fileHeader.seekg(0, ios::beg);
                    fileHeader.read((char*) &header, sizeof(Registro));
                    fileHeader.close();
                    fstream fileStart;
                    if (header.toNext == 'm')
                    {
                        fileStart.open(nombre, ios::in | ios::out | ios::binary);
                        fileStart.seekg((header.next+1)*sizeof(Registro), ios::beg);
                    }
                    else
                    {
                        fileStart.open(nombreAux, ios::in | ios::out | ios::binary);
                        fileStart.seekg((header.next)*sizeof(Registro), ios::beg);
                    }
                    Registro start;
                    fileStart.read((char*) &start, sizeof(Registro));
                    fileStart.close();
                    header.next   = start.prev;
                    header.toNext = start.toPrev;
                    fileHeader.open(nombre, ios::in | ios::out | ios::binary);
                    fileHeader.seekg(0, ios::beg);
                    fileHeader.write((char*) &header, sizeof(Registro));
                    // printInfoHeader(header);
                    fileHeader.close();                
                }
            }
        }

        void tryToReconstruct()
        {
            fstream fileAux;
            fileAux.open(nombreAux, ios::in | ios::out | ios::binary);
            fileAux.seekg(0, ios::end);
            int lines = fileAux.tellg()/sizeof(Registro);
            if (lines >= reconstructFactor)
            {
                cout << "RECONSTRUYENDO...\n";
                fileAux.close();
                vector<Registro> registros = loadAll(false);
                fstream fileMain;
                Registro header;
                fileMain.open(nombre, ios::in | ios::out | ios::binary);
                fileMain.seekg(0, ios::beg);
                fileMain.read((char*) &header, sizeof(Registro));
                fileMain.close();
                // borrar todo fileAux
                fileAux.open(nombreAux, ios::out | ios::trunc);
                fileAux.close();
                // borrar todo fileMain
                fileMain.open(nombre, ios::out | ios::trunc);      
                // reescribir el header
                header.next    = 0;
                header.toNext  = 'm';
                header.nextDel = -1;
                header.toDel   = 'm';
                fileMain.write((char*) &header, sizeof(Registro));
                // reescribir en el main los resultados del loadAll
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
                    registros[i].toNext = 'm';
                    registros[i].toPrev = 'm';
                    fileMain.write((char*) &registros[i], sizeof(Registro));
                }
                fileMain.close();
            }
            else
            {
                fileAux.close();
            }
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
            {
                cout << "ESCRIBIENDO CABECERA\n";
                file.write((char*) &reg, sizeof(reg));
            }
            file.close();
        }
        Sequential(string nombre, int reconstructFactor)
        {
            this->nombre = nombre;
            this->reconstructFactor = reconstructFactor;
            fstream file;
            file.open(nombre, ios::in | ios::out | ios::binary);
            file.seekg(0, ios::end);
            Registro reg;
            strcpy(reg.codigo, "1111");
            strcpy(reg.nombre, "cabeceraaaaaaaaaaaa");
            strcpy(reg.carrera, "cienciadelacomp");
            if (file.tellg() == 0)
            {
                cout << "ESCRIBIENDO CABECERA\n";
                file.write((char*) &reg, sizeof(reg));
            }
            file.close();
        }        
        ~Sequential(){};

        void printHeader()
        {
            Registro header;
            fstream file;
            file.open(nombre, ios::in | ios::out | ios::binary);
            file.seekg(0, ios::beg);
            file.read((char*) &header, sizeof(Registro));
            printInfoHeader(header);
        }

        void printAllDeleted()
        {
            Registro header;
            fstream file;
            file.open(nombre, ios::in | ios::out | ios::binary);
            file.seekg(0, ios::beg);
            file.read((char*) &header, sizeof(Registro));
            printInfoHeader(header);
            Registro current;
            if (header.nextDel != -1)
            {
                if (header.toDel == 'm')
                {
                    current = readRecord(nombre, header.nextDel+1, false);
                }
                else
                {
                    current = readRecord(nombreAux, header.nextDel, false);
                }
                printRegistro(current, true);
            }
            while (current.nextDel != -1)
            {
                if (current.toDel == 'm')
                {
                    current = readRecord(nombre, current.nextDel+1, false);
                }
                else
                {
                    current = readRecord(nombreAux, current.nextDel, false);
                }                
                printRegistro(current, true);
            }
            
        }

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
            Registro reg = readRecord(nombre, 0, false);
            if (reg.toNext == 'm')
            {
                reg = readRecord(nombre, reg.next+1, print);
            }
            else
            {
                reg = readRecord(nombreAux, reg.next, print);
            }
            // printRegistro(reg);
            registros.push_back(reg);

            // while (reg.next != -1)
            while (reg.next >= 0)
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
                Registro header;
                file.seekg(0, ios::beg);
                file.read((char*) &header, sizeof(Registro));
                header.next = 0;
                header.toNext = 'm';
                file.seekg(0, ios::beg);
                file.write((char*) &header, sizeof(Registro));
                file.close();
            }

        };

        vector<Registro> search(string key)
        {
            vector<Registro> registros = loadAll(false);
            vector<Registro> registrosEncontrados;
            char charKey[key.length()+1];
            strcpy(charKey, key.c_str());
            int pos = binarySearch(registros, key);
            if (pos == -1) return registrosEncontrados;
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

            // vector<Registro> registros = load(nombre, false);
            // vector<Registro> registrosAux = load(nombreAux, false);


            // if (pos != -1)
            // {
            //     // el actual
            //     printRegistro(registros[pos]);
            //     registrosEncontrados.push_back(registros[pos]);
            //     // ir hacia adelante
            //     for (int i = pos+1; i < registros.size(); ++i)
            //     {
            //         if (strcmp(registros[i].nombre, charKey) != 0) break;
            //         printRegistro(registros[i]);
            //         registrosEncontrados.push_back(registros[i]);
            //     }
            //     // ir hacia atrás
            //     for (int i = pos-1; i >= 0; --i)
            //     {
            //         if (strcmp(registros[i].nombre, charKey) != 0) break;
            //         printRegistro(registros[i]);
            //         registrosEncontrados.push_back(registros[i]);
            //     }
            // }

            // // linear en el archivo auxiliar
            // for (int i = 0; i < registrosAux.size(); ++i)
            // {
            //     // si lo encuentra
            //     if (strcmp(registrosAux[i].nombre, charKey) == 0)
            //     {
            //         printRegistro(registrosAux[i]);
            //         registrosEncontrados.push_back(registrosAux[i]);
            //     }
            // }
            return registrosEncontrados;
        };

        vector<Registro> search(string begin, string end)
        {
            vector<Registro> registros = loadAll(false);
            vector<Registro> registrosEncontrados;
            char charBeg[begin.length()+1];
            strcpy(charBeg, begin.c_str()); 
            char charEnd[end.length()+1];
            strcpy(charEnd, end.c_str()); 
            int pos = binarySearch(registros, begin);
            if (pos == -1) return registrosEncontrados;
            // cout << "POS: " << pos << " SIZE: " << registros.size() << endl;
            // el actual
            printRegistro(registros[pos]);
            registrosEncontrados.push_back(registros[pos]);
            // ir hacia atrás repetidos
            for (int i = pos-1; i >= 0; --i)
            {
                if (strcmp(registros[i].nombre, charBeg) != 0) break;
                printRegistro(registros[i]);
                registrosEncontrados.push_back(registros[i]);
            }
            // ir hacia adelante hasta el charEnd
            for (int i = pos+1; i < registros.size(); ++i)
            {
                if (strcmp(registros[i].nombre, charEnd) > 0) break;
                printRegistro(registros[i]);
                registrosEncontrados.push_back(registros[i]);
            }   

            // vector<Registro> registros = load(nombre, false);
            // vector<Registro> registrosAux = load(nombreAux, false);

            // int pos = binarySearch(registros, begin);
            // if (pos != -1)
            // {
            //     // el actual
            //     printRegistro(registros[pos]);
            //     registrosEncontrados.push_back(registros[pos]);
            //     // ir hacia adelante
            //     for (int i = pos+1; i < registros.size(); ++i)
            //     {
            //         if (strcmp(registros[i].nombre, charEnd) > 0) break;
            //         printRegistro(registros[i]);
            //         registrosEncontrados.push_back(registros[i]);
            //     }
            //     // ir hacia atrás
            //     for (int i = pos-1; i >= 0; --i)
            //     {
            //         if (strcmp(registros[i].nombre, charBeg) != 0) break;
            //         printRegistro(registros[i]);
            //         registrosEncontrados.push_back(registros[i]);
            //     }
            // }

            // // linear en el archivo auxiliar
            // for (int i = 0; i < registrosAux.size(); ++i)
            // {
            //     // si está en el rango
            //     if ((strcmp(registrosAux[i].nombre, charBeg) >= 0)  
            //         && (strcmp(registrosAux[i].nombre, charEnd) <= 0))
            //     {
            //         printRegistro(registrosAux[i]);
            //         registrosEncontrados.push_back(registrosAux[i]);
            //     }
            // }
            return registrosEncontrados;
        };

        void add(Registro registro)
        {
            // CASO 1: INSERTAR EN UN REGISTRO ELIMINADO USANDO LIFO
            fstream file;
            Registro head;
            file.open(nombre, ios::in | ios::out | ios::binary);
            file.seekg(0, ios::beg);
            file.read((char*) &head, sizeof(Registro));
            file.close();
            Registro elim;
            if (head.nextDel != -1)
            {
                if (head.toDel == 'm')
                {
                    elim = readRecord(nombre, head.nextDel+1, false);
                }
                else
                {
                    elim = readRecord(nombreAux, head.nextDel, false);
                }
            }
            Registro prevElim = elim;
            pair<int, char> goToPrevElim = make_pair(-1, 'm');
            if (elim.nextDel != -1)
            {
                while (true)
                {
                    // debug utility
                    // printRegistro(elim, true);
                    // está al inicio
                    if (elim.prev == -1)
                    {
                        Registro next;
                        if (elim.toNext == 'm')
                        {
                            next = readRecord(nombre, elim.next+1, false);
                        }
                        else
                        {
                            next = readRecord(nombreAux, elim.next, false);                    
                        }

                        if (strcmp(registro.nombre, next.nombre) <= 0)
                        {
                            // sobreescribo en ese espacio eliminado que está al inicio
                            cout << "ADD EN ESPACIO ELIMINADO QUE ESTÁ AL INICIO\n";
                            fstream fileToWrite;
                            if (next.toPrev == 'm')
                            {
                                fileToWrite.open(nombre, ios::in | ios::out | ios::binary);
                                fileToWrite.seekg((next.prev+1)*sizeof(Registro), ios::beg);
                            }
                            else
                            {
                                fileToWrite.open(nombreAux, ios::in | ios::out | ios::binary);
                                fileToWrite.seekg((next.prev)*sizeof(Registro), ios::beg);
                            }
                            registro.next   = elim.next;
                            registro.toNext = elim.toNext;
                            fileToWrite.write((char*) &registro, sizeof(Registro));
                            fileToWrite.close();
                            actualizeElimPointers(goToPrevElim, prevElim, elim);
                            return;
                        }
                    }
                    // está al final
                    if (elim.next == -1)
                    {
                        Registro prev;
                        if (elim.toPrev == 'm')
                        {
                            prev = readRecord(nombre, elim.prev+1, false);
                        }
                        else
                        {
                            prev = readRecord(nombreAux, elim.prev, false);                    
                        }

                        if (strcmp(prev.nombre, registro.nombre) <= 0)
                        {
                            // sobreescribo en ese espacio eliminado que está al final
                            cout << "ADD EN ESPACIO ELIMINADO QUE ESTÁ AL FINAL\n";
                            fstream fileToWrite;
                            if (prev.toNext == 'm')
                            {
                                fileToWrite.open(nombre, ios::in | ios::out | ios::binary);
                                fileToWrite.seekg((prev.next+1)*sizeof(Registro), ios::beg);
                            }
                            else
                            {
                                fileToWrite.open(nombreAux, ios::in | ios::out | ios::binary);
                                fileToWrite.seekg((prev.next)*sizeof(Registro), ios::beg);
                            }
                            // cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n";
                            // cout << goToPrevElim.first << goToPrevElim.second << endl;
                            // printRegistro(prevElim);
                            // printRegistro(elim);
                            registro.prev   = elim.prev;
                            registro.toPrev = elim.toPrev;
                            fileToWrite.write((char*) &registro, sizeof(Registro));
                            fileToWrite.close();
                            actualizeElimPointers(goToPrevElim, prevElim, elim);
                            prev.next *= -1;
                            fstream fileToReActive;
                            if (registro.toPrev == 'm')
                            {
                                fileToReActive.open(nombre, ios::in | ios::out | ios::binary);
                                fileToReActive.seekg((registro.prev+1)*sizeof(Registro), ios::beg);
                            }
                            else
                            {
                                fileToReActive.open(nombre, ios::in | ios::out | ios::binary);
                                fileToReActive.seekg((registro.prev)*sizeof(Registro), ios::beg);
                            }
                            fileToReActive.write((char*) &prev, sizeof(Registro));
                            fileToReActive.close();
                            return;
                        }
                    }
                    // está al medio
                    Registro prev;
                    if (elim.toPrev == 'm')
                    {
                        prev = readRecord(nombre, elim.prev+1, false);
                    }
                    else
                    {
                        prev = readRecord(nombreAux, elim.prev, false);                    
                    }
                    Registro next;
                    if (elim.toNext == 'm')
                    {
                        next = readRecord(nombre, elim.next+1, false);
                    }
                    else
                    {
                        next = readRecord(nombreAux, elim.next, false);                    
                    }

                    if (strcmp(prev.nombre, registro.nombre) <= 0 
                        && strcmp(registro.nombre, next.nombre) <= 0)
                    {
                        // sobreescribo en ese espacio eliminado que está al medio
                        cout << "ADD EN ESPACIO ELIMINADO QUE ESTÁ AL MEDIO\n";
                        fstream fileToWrite;
                        if (prev.toNext == 'm')
                        {
                            fileToWrite.open(nombre, ios::in | ios::out | ios::binary);
                            fileToWrite.seekg((prev.next+1)*sizeof(Registro), ios::beg);
                        }
                        else
                        {
                            fileToWrite.open(nombreAux, ios::in | ios::out | ios::binary);
                            fileToWrite.seekg((prev.next)*sizeof(Registro), ios::beg);
                        }
                        registro.next   = elim.next;
                        registro.toNext = elim.toNext;
                        registro.prev   = elim.prev;
                        registro.toPrev = elim.toPrev;
                        fileToWrite.write((char*) &registro, sizeof(Registro));
                        fileToWrite.close();
                        actualizeElimPointers(goToPrevElim, prevElim, elim);
                        return;
                    }
                    // final de la lista de eliminados
                    if (elim.nextDel == -1) break;
                    if (goToPrevElim.first == -1)
                    {
                        goToPrevElim = make_pair(head.nextDel, head.toDel);
                    }
                    else
                    {
                        goToPrevElim = make_pair(prevElim.nextDel, prevElim.toDel);
                    }
                    prevElim = elim;

                    if (elim.toDel == 'm')
                    {
                        elim = readRecord(nombre, elim.nextDel+1, false);
                    }
                    else
                    {
                        elim = readRecord(nombreAux, elim.nextDel, false);
                    }
                } 
            }

            // CASO 2: INSERTAR UN REGISTRO EN EL AUX
            // load a todos los registros de ambos archivos ordenados por sus "punteros".
            vector<Registro> registros = loadAll(false);
            // cout << registros.size() << endl;
            string s(registro.nombre);
            int pos = binarySearch(registros, s);
            // si lo encuentra
            if (pos != -1)
            {
                Registro next = registros[pos];
                for (int i = pos+1; i < registros.size(); ++i)
                {
                    next = registros[i];
                    if (strcmp(registros[i].nombre, registro.nombre) != 0)
                    {
                        cout << "ADD EXISTENTE AL MEDIO\n";
                        Registro prev = registros[i-1];
                        modifyRegisters(prev, registro, next);
                        tryToReconstruct();
                        return;
                    }
                }
                cout << "ADD EXISTENTE AL FINAL\n";
                modifyRegisters(next, registro);
                tryToReconstruct();
            }
            else
            {
                if (strcmp(registro.nombre, registros[0].nombre) < 0)
                {
                    // printRegistro(registros[0]);
                    cout << "ADD NO EXISTENTE AL INICIO\n";
                    // se cuenta la cantidad de líneas en el aux
                    fstream fileAux;
                    fileAux.open(nombreAux, ios::in | ios::out | ios::binary);
                    fileAux.seekg(0, ios::end);
                    // se modifica el anterior registro inicial
                    registros[0].prev   = fileAux.tellg()/sizeof(Registro);
                    registros[0].toPrev = 'a';
                    fileAux.close();
                    // se consigue el nextNext
                    fstream fileNextNext;
                    Registro nextNext;
                    if (registros[0].toNext == 'm')
                    {
                        fileNextNext.open(nombre, ios::in | ios::out | ios::binary);
                        fileNextNext.seekg((registros[0].next + 1) * sizeof(Registro), ios::beg);
                        fileNextNext.read((char*) &nextNext, sizeof(Registro));
                    }
                    else
                    {
                        fileNextNext.open(nombreAux, ios::in | ios::out | ios::binary);   
                        fileNextNext.seekg((registros[0].next) * sizeof(Registro), ios::beg);  
                        fileNextNext.read((char*) &nextNext, sizeof(Registro));                   
                    }
                    fileNextNext.close();

                    // se modifica el registro a insertar haciendo que apunte como next a lo que apunta el nextNext como prev.
                    fileAux.open(nombreAux, ios::in | ios::out | ios::binary);
                    fileAux.seekg(0, ios::end);
                    registro.next   = nextNext.prev;
                    registro.toNext = nextNext.toPrev;
                    fileAux.write((char*) &registro, sizeof(Registro));
                    fileAux.close();
                    // se abre un file del seqFile donde está la cabecera
                    fstream fileHeader;
                    Registro regHeader;
                    fileHeader.open(nombre, ios::in | ios::out | ios::binary);
                    fileHeader.seekg(0, ios::beg);
                    // se lee la cabecera
                    fileHeader.read((char*) &regHeader, sizeof(Registro));
                    // printRegistro(registros[0]);
                    // printRegistro(regHeader);
                    pair<int, char> temp = make_pair(regHeader.next, regHeader.toNext);
                    // se reescribe la cabecera en base al prev del anterior registro inicial
                    // porque este será el nuevo inicio de nuestros registros cuando iteremos en el loadAll.
                    regHeader.next   = registros[0].prev;
                    regHeader.toNext = registros[0].toPrev;
                    fileHeader.seekg(0, ios::beg);
                    fileHeader.write((char*) &regHeader, sizeof(Registro));
                    fileHeader.close();

                    fstream fileFirst;
                    if (temp.second == 'm')
                    {
                        fileFirst.open(nombre, ios::in | ios::out | ios::binary);
                        fileFirst.seekg((temp.first + 1) * sizeof(Registro), ios::beg);
                        // cout << temp.first << endl;
                        fileFirst.write((char*) &registros[0], sizeof(Registro));
                    }
                    else
                    {
                        fileFirst.open(nombreAux, ios::in | ios::out | ios::binary);   
                        fileFirst.seekg((temp.first) * sizeof(Registro), ios::beg);  
                        fileFirst.write((char*) &registros[0], sizeof(Registro));                   
                    }
                    fileFirst.close();
                    tryToReconstruct();
                    return;
                }

                for (int i = 0; i < registros.size(); ++i)
                {
                    Registro prev = registros[i];
                    Registro next = registros[i+1];

                    if (i+1 < registros.size())
                    {
                        // Por ejemplo, D < E < F verdadero, se considera < y no <= 
                        // porque acá ya directamente es el caso en el que el binary search no lo encontró
                        if (strcmp(prev.nombre, registro.nombre) < 0 
                            && strcmp(registro.nombre, next.nombre) < 0)
                        {
                            cout << "ADD NO EXISTENTE AL MEDIO\n";
                            modifyRegisters(prev, registro, next);
                            tryToReconstruct();
                            return;
                        }
                    }
                    else
                    {
                        if (strcmp(registro.nombre, next.nombre) >= 0)
                        {
                            cout << "ADD NO EXISTENTE AL FINAL\n";
                            modifyRegisters(prev, registro);
                            tryToReconstruct();
                            return;
                        }
                    }

                }
            }
        };

        bool delete_(string key)
        {
            vector<Registro> registros = loadAll(false);
            int pos = binarySearch(registros, key);
            if (pos == -1) return false;

            // Al inicio (pos == 0)
            if (pos == 0)
            {
                cout << "HACIENDO DELETE AL INICIO\n";
                fstream fileHeader;
                Registro header;
                fileHeader.open(nombre, ios::in | ios::out | ios::binary);
                fileHeader.seekg(0, ios::beg);
                fileHeader.read((char*) &header, sizeof(Registro));  
                fileHeader.close();
                // el nextDel del registro eliminado es igual a la cabecera de la lista de eliminados
                registros[pos].nextDel = header.nextDel;
                registros[pos].toDel = header.toDel;
                // se actualiza el inicio de la lista de NO eliminados
                header.next   = registros[pos].next;
                header.toNext = registros[pos].toNext;
                // se actualiza el inicio de la lista de eliminados
                header.nextDel = registros[pos+1].prev;
                header.toDel = registros[pos+1].toPrev;
                int temp = registros[pos+1].prev;
                // registros[pos+1].prev = -1;
                // sobreescribir header
                fileHeader.open(nombre, ios::in | ios::out | ios::binary);
                fileHeader.seekg(0, ios::beg);
                fileHeader.write((char*) &header, sizeof(Registro));
                fileHeader.close();
                // sobreescribir el registro eliminado
                fstream fileDeleted;
                if (registros[pos+1].toPrev == 'm')
                {
                    fileDeleted.open(nombre, ios::in | ios::out | ios::binary);
                    fileDeleted.seekg((temp + 1) * sizeof(Registro), ios::beg);
                    fileDeleted.write((char*) &registros[pos], sizeof(Registro));
                }
                else
                {
                    fileDeleted.open(nombreAux, ios::in | ios::out | ios::binary);
                    fileDeleted.seekg((temp) * sizeof(Registro), ios::beg);
                    fileDeleted.write((char*) &registros[pos], sizeof(Registro));
                }
                fileDeleted.close();
                // sobreescribir el registro siguiente al eliminado
                fstream fileNextDeleted;
                if (registros[pos].toNext == 'm')
                {
                    fileNextDeleted.open(nombre, ios::in | ios::out | ios::binary);
                    fileNextDeleted.seekg((registros[pos].next + 1) * sizeof(Registro), ios::beg);
                    fileNextDeleted.write((char*) &registros[pos+1], sizeof(Registro));
                }
                else
                {
                    fileNextDeleted.open(nombreAux, ios::in | ios::out | ios::binary);
                    fileNextDeleted.seekg((registros[pos].next) * sizeof(Registro), ios::beg);
                    fileNextDeleted.write((char*) &registros[pos+1], sizeof(Registro));
                }
                fileNextDeleted.close();
                return true;
            }
            // Al final (pos == registros.size() - 1)
            if (pos == registros.size() - 1)
            {
                cout << "HACIENDO DELETE AL FINAL\n";
                fstream fileHeader;
                Registro header;
                fileHeader.open(nombre, ios::in | ios::out | ios::binary);
                fileHeader.seekg(0, ios::beg);
                fileHeader.read((char*) &header, sizeof(Registro));  
                fileHeader.close();
                // el nextDel del registro eliminado es igual a la cabecera de la lista de eliminados
                registros[pos].nextDel = header.nextDel;
                registros[pos].toDel = header.toDel;
                // se actualiza el inicio de la lista de eliminados
                header.nextDel = registros[pos-1].next;
                header.toDel = registros[pos-1].toNext;
                int temp = registros[pos].prev;
                // registros[pos].prev = -1;
                // sobreescribir header
                fileHeader.open(nombre, ios::in | ios::out | ios::binary);
                fileHeader.seekg(0, ios::beg);
                fileHeader.write((char*) &header, sizeof(Registro));
                fileHeader.close();
                // sobreescribir el registro eliminado
                fstream fileDeleted;
                if (registros[pos-1].toNext == 'm')
                {
                    fileDeleted.open(nombre, ios::in | ios::out | ios::binary);
                    fileDeleted.seekg((registros[pos-1].next + 1) * sizeof(Registro), ios::beg);
                    fileDeleted.write((char*) &registros[pos], sizeof(Registro));
                }
                else
                {
                    fileDeleted.open(nombreAux, ios::in | ios::out | ios::binary);
                    fileDeleted.seekg((registros[pos-1].next) * sizeof(Registro), ios::beg);
                    fileDeleted.write((char*) &registros[pos], sizeof(Registro));
                }
                fileDeleted.close();
                // para guardar el next anterior solo que en negativo para que ahí termine de iterar sobre la lista de NO eliminados.
                registros[pos-1].next *= -1;
                // sobreescribir el registro previo al eliminado
                fstream filePrevDeleted;
                if (registros[pos].toPrev == 'm')
                {
                    filePrevDeleted.open(nombre, ios::in | ios::out | ios::binary);
                    filePrevDeleted.seekg((temp + 1) * sizeof(Registro), ios::beg);
                    filePrevDeleted.write((char*) &registros[pos-1], sizeof(Registro));
                }
                else
                {
                    filePrevDeleted.open(nombreAux, ios::in | ios::out | ios::binary);
                    filePrevDeleted.seekg((temp) * sizeof(Registro), ios::beg);
                    filePrevDeleted.write((char*) &registros[pos-1], sizeof(Registro));
                }
                filePrevDeleted.close();
                return true;
            }
            // Al medio es cuando no es al inicio ni al final
            cout << "HACIENDO DELETE AL MEDIO EN POS " << pos << endl;
            // printRegistro(registros[pos]);
            fstream fileHeader;
            Registro header;
            fileHeader.open(nombre, ios::in | ios::out | ios::binary);
            fileHeader.seekg(0, ios::beg);
            fileHeader.read((char*) &header, sizeof(Registro));  
            fileHeader.close();
            // se actualiza el registro eliminado
            // el nextDel del registro eliminado es igual a la cabecera de la lista de eliminados
            registros[pos].nextDel = header.nextDel;
            registros[pos].toDel = header.toDel;
            int tempPrev = registros[pos].prev;
            int tempNext = registros[pos].next;
            // registros[pos].prev = -1;
            // registros[pos].next = -1;            
            // se actualiza el inicio de la lista de eliminados
            header.nextDel = registros[pos-1].next;
            header.toDel = registros[pos-1].toNext;
            // se actualiza el registro previo al eliminado
            registros[pos-1].next = tempNext;
            registros[pos-1].toNext = registros[pos].toNext;
            // se actualiza el registro siguiente al eliminado
            int tempNextPrev = registros[pos+1].prev;
            registros[pos+1].prev = tempPrev;
            registros[pos+1].toPrev = registros[pos+1].toPrev;
            // sobreescribir header
            fileHeader.open(nombre, ios::in | ios::out | ios::binary);
            fileHeader.seekg(0, ios::beg);
            fileHeader.write((char*) &header, sizeof(Registro));
            fileHeader.close();
            // sobreescribir el registro previo al eliminado
            fstream filePrevDeleted;
            if (registros[pos].toPrev == 'm')
            {
                filePrevDeleted.open(nombre, ios::in | ios::out | ios::binary);
                filePrevDeleted.seekg((tempPrev + 1) * sizeof(Registro), ios::beg);
                filePrevDeleted.write((char*) &registros[pos-1], sizeof(Registro));
            }
            else
            {
                filePrevDeleted.open(nombreAux, ios::in | ios::out | ios::binary);
                filePrevDeleted.seekg((tempPrev) * sizeof(Registro), ios::beg);
                filePrevDeleted.write((char*) &registros[pos-1], sizeof(Registro));
            }
            filePrevDeleted.close();
            // sobreescribir el registro eliminado
            fstream fileDeleted;
            if (registros[pos+1].toPrev == 'm')
            {
                fileDeleted.open(nombre, ios::in | ios::out | ios::binary);
                fileDeleted.seekg((tempNextPrev + 1) * sizeof(Registro), ios::beg);
                fileDeleted.write((char*) &registros[pos], sizeof(Registro));
            }
            else
            {
                fileDeleted.open(nombreAux, ios::in | ios::out | ios::binary);
                fileDeleted.seekg((tempNextPrev) * sizeof(Registro), ios::beg);
                fileDeleted.write((char*) &registros[pos], sizeof(Registro));
            }
            fileDeleted.close();
            // sobreescribir el registro siguiente al eliminado
            fstream fileNextDeleted;
            if (registros[pos].toNext == 'm')
            {
                fileNextDeleted.open(nombre, ios::in | ios::out | ios::binary);
                fileNextDeleted.seekg((tempNext + 1) * sizeof(Registro), ios::beg);
                fileNextDeleted.write((char*) &registros[pos+1], sizeof(Registro));
            }
            else
            {
                fileNextDeleted.open(nombreAux, ios::in | ios::out | ios::binary);
                fileNextDeleted.seekg((tempNext) * sizeof(Registro), ios::beg);
                fileNextDeleted.write((char*) &registros[pos+1], sizeof(Registro));
            }
            fileNextDeleted.close();
            // está bien
            // printRegistro(registros[pos-1]);
            // printRegistro(registros[pos]);
            // printRegistro(registros[pos+1]);
            return true;
        };

};


int main()
{
    Sequential seq("seqFile.txt");
    vector<Registro> registros;
    Registro regA;
    strcpy(regA.codigo, "2222");
    strcpy(regA.nombre, "A");
    strcpy(regA.carrera, "cienciadelacomp");
    Registro regB;
    strcpy(regB.codigo, "9999");
    strcpy(regB.nombre, "B");
    strcpy(regB.carrera, "cienciadelacomp");
    Registro regC;
    strcpy(regC.codigo, "6666");
    strcpy(regC.nombre, "C");
    strcpy(regC.carrera, "cienciadelacomp");      
    Registro regD;
    strcpy(regD.codigo, "8888");
    strcpy(regD.nombre, "D");
    strcpy(regD.carrera, "cienciadelacomp");   
    Registro regE;
    strcpy(regE.codigo, "3333");
    strcpy(regE.nombre, "E");
    strcpy(regE.carrera, "cienciadelacomp");   
    Registro regF;
    strcpy(regF.codigo, "7777");
    strcpy(regF.nombre, "F");
    strcpy(regF.carrera, "cienciadelacomp");   
    Registro regG;
    strcpy(regG.codigo, "4444");
    strcpy(regG.nombre, "G");
    strcpy(regG.carrera, "cienciadelacomp");       

    registros.push_back(regC);
    registros.push_back(regD);
    seq.insertAll(registros);
    // seq.printHeader();
    // TESTS DE INSERTS CUANDO NO HAY DELETE
    // Tests cuando no existe y hace insert al inicio.
    seq.add(regB);
    seq.add(regA);
    // seq.loadAll();
    // Tests cuando no existe y hacer insert al final.
    seq.add(regF);
    // Tests cuando sí existe y hacer insert al final.
    seq.add(regF);
    // Tests de search
    cout << "TEST SEARCH EXACTO\n";
    seq.search("F");    
    seq.add(regF);
    // Reconstrucción
    seq.loadAll();
    cout << "POST RECONSTRUCCIÓN\n";
    // Tests cuando no existe y hacer insert al medio
    seq.add(regE);
    // Tests cuando sí existe y hacer insert al medio
    seq.add(regE);
    // Tests cuando no existe y hacer insert al final.
    seq.add(regG);
    cout << "TEST SEARCH RANGO\n";
    seq.search("E", "G");
    cout << "PRE DELETES\n";
    seq.loadAll();
    cout << "POST DELETES\n";
    // Tests de delete al inicio
    seq.delete_("A");
    // // Tests de delete al final
    seq.delete_("G");
    // // Tests de delete al medio
    // seq.printHeader();
    seq.delete_("E");
    // seq.load("seqFile.txt");
    // seq.load("auxAdd.txt");
    seq.loadAll();
    cout << "TEST SEARCH RANGO DEPUÉS DE DELETES\n";
    seq.search("E", "G");        
    cout << "TEST LINKED LIST DE ELIMINADOS\n";
    seq.printAllDeleted();
    cout << "TEST ADDS EN REGISTROS ELIMINADOS\n";
    // Tests de add en un registro eliminado
    // add en un registro eliminado al inicio
    seq.add(regA); 
    // add en un registro eliminado al medio
    // int xd;
    // cin >> xd;
    // seq.add(regE); // entra en bucle
    // seq.load("auxAdd.txt");
    // add en un registro eliminado al final
    seq.add(regG);
    seq.loadAll();

    return 0;
};