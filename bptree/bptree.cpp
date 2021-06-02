#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstring>
#include <cmath>
#include<vector>

using namespace std;


template<int order>
struct Node
{
	char keys[order+1][20]; 
    long posLib{0};
    long next {-1};
	long children[order+2] = {0};
    long nodePos{-1};
    
	bool isLeaf = false;
    
    
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
            fstream outFile;
            outFile.open(dataFile,ios::in| ios::out| ios::binary | std::ofstream::app);
            long _pos;
            outFile.seekg(0,ios::end);
            outFile.write((char* )&reg, sizeof(Registro));
            _pos=outFile.tellg()- sizeof(Registro);
            outFile.close();
            return _pos;
        };

        void writeIndexFile(Node<order> node, int pos)
        {
            fstream outFile;
            outFile.open(indexFile,ios::in| ios::out| ios::binary);
            outFile.seekg(pos, ios::beg);         
            outFile.write((char* )&node, sizeof(Node<order>));
            outFile.close();
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

        bool addUtil(Node<order>& node, Registro reg, long dataPos)
        {
            // buscamos la posicion en el nodo
            int pos = 0;
            while (pos < node.posLib && node.keys[pos] < reg.nombre) pos++;
            if (!node.isLeaf) 
            {
                long address = node.children[pos];
                Node<order> child = getNextNode(address);
                if (addUtil(child, reg,dataPos)) 
                {
                    split(node, pos);
                }
            } 
            else 
            {
                node.insertNode(pos, reg.codigo,dataPos);
                writeIndexFile(node, node.nodePos);
            }
            
            return node.overflow();
        };
        
        bool searchUtil(Node<order>node, char nombre[20], Registro& buscado)
        {
            if (!node.isLeaf)
            {
                int pos = 0;
                for (int i = 0; i < node.posLib; i++)
                {
                    if (strcmp(nombre, node.keys[i]) < 0) break;
                    ++pos;
                }
                long nodePos = node.children[pos];
                node = getNextNode(nodePos);
                return searchUtil(node, nombre, buscado);
            }
            else
            {
                
                int pos = -1;//para llegar al children cero 
                for (int i = 0; i < node.posLib; ++i)
                {
                    if (strcmp(nombre, node.keys[i]) == 0)
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
                    file.seekg(dataPos, ios::beg);
                    file.read((char*) &reg, sizeof(Registro));
                    file.close(); 

                    if (strcmp(reg.nombre, nombre) != 0) return false;

                    buscado = reg;
                    return true;
                }

                Registro reg;
                fstream file;
                file.open(dataFile,ios::in | ios::out | ios::binary);
                file.seekg(dataPos, ios::beg);
                file.read((char*) &reg, sizeof(Registro));
                file.close();

                buscado = reg;
                return true; 
        
            }
        }

        bool searchUtilRango(Node<order>node, char begin[20], char end[20], vector<Registro>& buscado)
        {
            if (!node.isLeaf)
            {
                int pos = 0;
                for (int i = 0; i < node.posLib; i++)
                {
                    if (strcmp(begin, node.keys[i]) < 0) break;
                    ++pos;
                }
                long nodePos = node.children[pos];
                node = getNextNode(nodePos);
                return searchUtilRango(node, begin, end, buscado);
            }
            else
            {
                
                int pos = -1;//para llegar al children cero 
                for (int i = 0; i < node.posLib; ++i)
                {
                    if (strcmp(begin, node.keys[i]) == 0)
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
                    file.seekg(dataPos, ios::beg);
                    file.read((char*) &reg, sizeof(Registro));
                    file.close(); 

                    if (strcmp(reg.nombre, begin) != 0) return false;

                    buscado.push_back(reg);
                    // vas iterando por sobre las hojas
                    while (node.next != -1)
                    {
                        ++pos;
                        dataPos = node.children[pos+1];
                        Registro reg;
                        fstream file;
                        file.open(dataFile,ios::in | ios::out | ios::binary);
                        file.seekg(dataPos, ios::beg);
                        file.read((char*) &reg, sizeof(Registro));
                        file.close(); 
                        // cuando el nombre ya supera al end retorna true
                        if (strcmp(reg.nombre, end) > 0) return true;  
                        buscado.push_back(reg);                      
                    }

                    return true;
                }

                Registro reg;
                fstream file;
                file.open(dataFile,ios::in | ios::out | ios::binary);
                file.seekg(dataPos, ios::beg);
                file.read((char*) &reg, sizeof(Registro));
                file.close();

                buscado.push_back(reg);
                // vas iterando por sobre las hojas
                while (node.next != -1)
                {
                    ++pos;
                    dataPos = node.children[pos+1];
                    Registro reg;
                    fstream file;
                    file.open(dataFile,ios::in | ios::out | ios::binary);
                    file.seekg(dataPos, ios::beg);
                    file.read((char*) &reg, sizeof(Registro));
                    file.close(); 
                    // cuando el nombre ya supera al end retorna true
                    if (strcmp(reg.nombre, end) > 0) return true;  
                    buscado.push_back(reg);                      
                }
                return true; 

            }
        }

        void split(Node<order>node, int posNode)
        {
            Node<order> nodeSplit = getNextNode(node.children[posNode]); 
            Node<order> left = nodeSplit;
            left.posLib = 0;
            Node<order> right;
            fstream file;
            long posNew;
            file.open(indexFile, ios::in |ios::out| ios::binary);
            file.seekg(0, ios::end);
            posNew = file.tellg();
            file.close();
            right.nodePos = posNew;
        
            int i = 0, j = 0;

            for (i = 0; j < aux; ++i, ++j) 
            {
                left.children[i] = nodeSplit.children[j];
                strcpy(left.keys[i], nodeSplit.keys[j]);
                left.posLib++;
            }

            left.children[i] = nodeSplit.children[j];
            node.insertNode(posNode,nodeSplit.keys[j],left.nodePos);

            if (nodeSplit.children[0] != 0) ++j;
            else 
            {
                right.next = left.next;
                left.next = right.nodePos;
                node.children[posNode + 1] = right.nodePos;
            }

            for (i = 0; j < order + 1; ++i, ++j) 
            {
                right.children[i] = nodeSplit.children[j];
                strcpy(right.keys[i], nodeSplit.keys[j]);
                right.posLib++;
            }

            right.children[i] = nodeSplit.children[j];

            node.children[posNode] = left.nodePos;
            node.children[posNode + 1] = right.nodePos;


            left.isLeaf = nodeSplit.isLeaf;
            right.isLeaf = nodeSplit.isLeaf;

            writeIndexFile(left, left.nodePos);
            writeIndexFile(right, right.nodePos);
            writeIndexFile(node, node.nodePos);
            
        };

        void splitRoot(Node<order> &root)
        {
            Node<order> left;
            Node<order> right;
        
            int pos = 0, j = 0, i = 0;
            for (i = 0; j < aux; ++i, ++j) 
            {
                left.children[i] = root.children[j];
                strcpy(left.keys[i],root.keys[j]);
                left.posLib++;
            }  

            left.children[i] = root.children[j];
            strcpy(root.keys[0], root.keys[j]);
            
            long posNew;
            fstream file;
            file.open(indexFile, ios::in |ios::out| ios::binary);
            file.seekg(0, ios::end);
            posNew = file.tellg();
            file.close();

            left.nodePos = posNew;
            
            right.nodePos = left.nodePos + sizeof(Node<order>);
            left.next = right.nodePos;
            if (root.isLeaf) left.isLeaf = true;
            else left.isLeaf = false;
            right.isLeaf = left.isLeaf;

            if (root.children[0] != 0) j++;

            for (i = 0; j < order + 1; ++i, ++j) {
                right.children[i] = root.children[j];
                strcpy(right.keys[i], root.keys[j]);
                right.posLib++;
            }

            right.children[i] = root.children[j];

            root.children[0] = left.nodePos;

            root.children[1] = right.nodePos;

            root.posLib = 1;
            root.isLeaf=false;

            writeIndexFile(right, right.nodePos);

            writeIndexFile(root, root.nodePos);

            writeIndexFile(left, left.nodePos);
        }


    public:
        bptree(string indexFile, string dataFile)
        {
            this->indexFile = indexFile;
            this->dataFile = dataFile;
            aux = ceil(order/2.0);
        };
         
        void add(Registro reg)
        {
            // Si el index no está vacío insertamos de forma recursiva
            if (!isEmpty(indexFile))
            {
                //En cualquier otro caso insertamos de forma recursiva
                long dataPos = writeDataFile(reg); //lo insertamos y guardamos la posicion en el datafile
                Node<order> root = getNextNode(0);
                if (addUtil(root, reg, dataPos))
                {
                    splitRoot(root);
                }
            }
            else
            {
                // CC: Si el index esta vacio insertamos el root
                Node<order> root;
                long dataPos = writeDataFile(reg);
                root.insertNode(0, reg.nombre, dataPos);
                root.isLeaf = true;
                root.nodePos = 0;
                writeIndexFile(root, root.nodePos);

            }
        };

      
        bool search(string nombre, Registro &reg)
        {   
            Node<order> root = getNextNode(0);
            char nombreUtil[20];
            strcpy(nombreUtil, nombre.c_str());
            return searchUtil(root, nombreUtil, reg);
        };

        void search(string begin, string end, vector<Registro> &reg)
        {
            Node<order> root = getNextNode(0);
            char beginUtil[20];
            strcpy(beginUtil, begin.c_str());
            char endUtil[20];
            strcpy(endUtil, end.c_str());            
            return searchUtilRango(root, beginUtil, endUtil, reg);
        }


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
    b.add(reg2);
    b.add(reg3);


    Registro regRE;
    Registro regRf;
    Registro regRg;

    b.search("A",regRE);
    cout<<regRE.nombre<<endl;
    b.search("B",regRf);
    cout<<regRf.nombre<<endl;
    b.search("C",regRg);
    cout<<regRg.nombre<<endl;
    

    
    return 0;
}