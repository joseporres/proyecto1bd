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
    cout << reg.nombre << endl;
    cout << reg.codigo << endl;
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
                        return;
                    }
                }
                cout << "ADD EXISTENTE AL FINAL\n";
                modifyRegisters(next, registro);
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
                            return;
                        }
                    }
                    else
                    {
                        if (strcmp(registro.nombre, next.nombre) >= 0)
                        {
                            cout << "ADD NO EXISTENTE AL FINAL\n";
                            modifyRegisters(prev, registro);
                            return;
                        }
                    }

                }
            }
        };

        // si deleteAll es false se borra el primero que encuentre con esa key, caso contrario se borran todos.
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
                registros[pos+1].prev = -1;
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
                registros[pos].prev = -1;
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
                registros[pos-1].next = -1;
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
            cout << "HACIENDO DELETE AL MEDIO\n";
            

            // modifyOnDeleteRegisters(registros, pos, header);
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
    // TESTS DE INSERTS CUANDO NO HAY DELETE
    // Tests cuando no existe y hace insert al inicio.
    seq.add(regB);
    seq.add(regA);
    // seq.loadAll();
    // Tests cuando no existe y hacer insert al final.
    seq.add(regF);
    // Tests cuando sí existe y hacer insert al final.
    seq.add(regF);
    seq.add(regF);
    // Tests cuando no existe y hacer insert al medio
    seq.add(regE);
    // Tests cuando sí existe y hacer insert al medio
    seq.add(regE);
    // Tests cuando no existe y hacer insert al final.
    seq.add(regG);
    // Tests de delete al inicio
    seq.delete_("A");
    seq.delete_("B");
    // Tests de delete al final
    seq.delete_("G");
    // Tests de delete al medio
    // seq.delete_("E");
    // seq.loadAll();
    seq.printHeader();
    // Tests de delete al medio
    // Tests de delete al final

    return 0;
};