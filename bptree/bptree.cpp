#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstring>
#include <vector>

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

    bool isOverflow() 
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
    
        bool isEmpty(string name)
        {
            ifstream pFile(name);   
            bool x = pFile.peek() == ifstream::traits_type::eof();
            pFile.close();
            return x;
        }

    public:
        bptree(string indexFile, string dataFile)
        {
            this->indexFile = indexFile;
            this->dataFile = dataFile;
        };

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


        bool insert(Registro reg)
        {
            // Si el index esta vacio insertamos el root
            if (isEmpty(indexFile))
            {
                Node<order>root;
                long pos = writeDataFile(reg);
                root.insertNode(0, reg.nombre, pos);
                root.isLeaf = true;
                root.nodePos = 0;
                writeIndexFile(root, 0);
                return true;
            }

            //En cualquier otro caso insertamos de forma recursiva
            

        };

        bool split()


};


int main ()
{
    Registro reg;
    strcpy(reg.nombre,"A");
    reg.ciclo = 1;
    strcpy(reg.codigo,"1234");
    strcpy(reg.carrera,"cienciadelacomp");

    bptree <3> b ("indexFile.txt","dataFile.txt");

    b.insert(reg);


    return 0;
}