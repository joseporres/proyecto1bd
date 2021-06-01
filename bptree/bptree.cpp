#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstring>
#include <cmath>

using namespace std;


template<int order>
struct Node
{
	char keys[order+1][20]; 
	long children[order+2] = {0};
	bool isLeaf = false;
    int nodePos = -1;
    int posLib = 0;
    int next = -1;
    
    void insertNode(int pos, char value[20], long childs)
    {
        int i;
        for (i = posLib; i > pos; i--)//ponemos al valor en la posicion que le corresponde manteniendo el orden
        {
            children[i+1] = children[i];
            strcpy(keys[i], keys[i-1]);
        }
        strcpy(keys[i], value);
        children[i+1] = childs;
        posLib++;
    }

    bool overflow() 
    { 
        return posLib > order; 
    }
};

struct Registro
{
    char codigo [5];
    char nombre [20];
    char carrera [15];
    int ciclo = 0;
};



template<int order>
class bptree
{
    private:
        string indexFile;
        string dataFile;
        float aux;

        bool isEmpty(string name)
        {
            ifstream pFile(name);   
            bool x = pFile.peek() == ifstream::traits_type::eof();
            pFile.close();
            return x;
        }

        long writeDataFile(Registro reg)
        {
            fstream file;
            file.open(dataFile, ios::in | ios::out | ios::binary);
            file.seekg(0, ios::beg);
            file.write((char*) &reg, sizeof(Registro));
            long final = file.tellg() - sizeof(Registro);
            file.close();
            return final;
        };

        void writeIndexFile(Node<order> node, int pos)
        {
            fstream file;
            file.open(indexFile, ios::in | ios::out | ios::binary);
            file.seekg(pos, ios::beg);
            file.write((char*) &node, sizeof(Node<order>));
            file.close();
        };

         Node<order> getRoot()
         {
            Node<order> root;
            fstream file;
            file.open(indexFile, ios::in | ios::out |ios::binary);
            file.read((char*) &root, sizeof(Node<order>));
            file.close();
            return root;
         };

         Node<order> getNextNode(long pos)
         {
            Node<order> next;
            fstream file;
            file.open(indexFile, ios::in | ios::out |ios::binary);
            file.seekg(pos,ios::beg);
            file.read((char*) &next, sizeof(Node<order>));
            file.close();
            return next;
         };

        bool addUtil(Node<order> node, Registro reg, long dataPos)
        {
            // buscamos la posicion en el nodo
            int posNodo; 
            while (posNodo < node.posLib && strcmp(node.keys[posNodo], reg.nombre) < 0) ++posNodo;
            if (node.isLeaf)
            {
                node.insertNode(posNodo, reg.nombre, dataPos);
                writeIndexFile(node, node.nodePos);
            }
            else
            {
                long next = node.children[posNodo];
                Node<order> nextNode = getNextNode(next);
                if (addUtil(nextNode, reg, dataPos)) 
                {
                    // Acá nos faltó la lógica de realizar un split
                }
            }
            bool overflow = node.overflow();
            if (!overflow) return false;
            return true;
        };
        
        Registro searchUtil(Node<order>node, char nombre [20])
        {
            if (!node.isLeaf)
            {
                int i = 0, pos = 0;
                while (i < node.posLib && strcmp(nombre, node.keys[i]) >= 0) ++i;
                node = getNextNode(node.children[pos]);
                return searchUtil(node,nombre);
            }
            else
            {
                
                int pos = -1;
                for(int i = 0; i < node.posLib; i++)
                {
                    if (nombre == node.keys[i])
                    {
                        pos = i;
                        break;
                    }
                }
                long dataPos = node.children[pos+1];
                if (pos == -1)
                { 
                    Registro reg;
                    fstream file;
                    file.open(dataFile,ios::in | ios::out | ios::binary);
                    file.seekg(node.children[0], ios::beg);
                    file.read((char*) &reg, sizeof(Registro));
                    file.close();
                    if (reg.nombre == nombre)
                    {
                        return reg;
                    }
                }
                else
                {
                    Registro reg;
                    fstream file;
                    file.open(dataFile,ios::in|ios::out|ios::binary);
                    file.seekg(node.children[dataPos],ios::beg);
                    file.read((char*)&reg, sizeof(Registro));
                    file.close();
                    return reg; 
                }

            }
        }


    public:
        bptree(string indexFile, string dataFile)
        {
            this->indexFile = indexFile;
            this->dataFile = dataFile;
            aux = order / 2.0f;
        };

         
        void add(Registro reg)
        {
            // Si el index no está vacío insertamos de forma recursiva
            if (!isEmpty(indexFile))
            {
                //En cualquier otro caso insertamos de forma recursiva
                long dataPos = writeDataFile(reg); //lo insertamos y guardamos la posicion en el datafile
                Node<order> root = getRoot();
                if (addUtil(root, reg, dataPos)) 
                {
                    // Acá nos faltó la lógica de realizar un split
                }
                return;
            }
            // CC: Si el index esta vacio insertamos el root
            Node<order> root;
            long dataPos = writeDataFile(reg);
            root.insertNode(0, reg.nombre, dataPos);
            root.isLeaf = true;
            root.nodePos = 0;
            writeIndexFile(root, 0);
        };

      
        Registro search(string nombre)
        {   
            Node<order> root = getRoot();
            char nombreUtil [20];
            strcpy(nombreUtil, nombre.c_str());
            Registro reg = searchUtil(root,nombreUtil);
            return reg;
        };


};


int main ()
{
    Registro reg;
    strcpy(reg.nombre, "A");
    reg.ciclo = 1;
    strcpy(reg.codigo, "1234");
    strcpy(reg.carrera, "cienciadelacomp");

    Registro reg2;
    strcpy(reg2.nombre, "B");
    reg2.ciclo = 1;
    strcpy(reg2.codigo, "5678");
    strcpy(reg2.carrera, "cienciadelacomp");

    Registro reg3;
    strcpy(reg3.nombre, "C");
    reg3.ciclo = 1;
    strcpy(reg3.codigo, "9123");
    strcpy(reg3.carrera, "cienciadelacomp");

    bptree<3> b("indexFile.txt", "dataFile.txt");

    b.add(reg);
   

    Registro regRE;

    regRE = b.search("A");

    cout<<reg.nombre;
    return 0;
}