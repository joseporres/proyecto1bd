# Proyecto 1 - BD II
## B+TREE Agrupado
Esta implementación cuenta con 3 bloques: el nodo, el registro, y el arbol. Los registros se encuentran en el dataFile.txt y en el indexFile.txt se encuentra el B+TREE de forma lógica. Además, el indexFile.txt tiene como posicion 0 a la raiz del árbol.

## Search
Se utilizaron 2 funciones: una de ellas es la que el usuario puede utilizar mientras que la otra es solo de utilidad. En la funcion visible por el usuario, toma el nombre del alumno y lo transforma a un char[20] para así poder hacer las comparaciones con el key deseado en el searchUtil. Luego de recibir ambos, el nombre en char[20] y el root verificamos si es que el nodo es hoja. Si lo es, simplemente buscamos la posicion dentro del nodo y nos movemos al hijo obtenido con la posición. Caso contrario, iteramos en el nodo hoja hasta encontrar el key. Si se encuentra lo traemos del datafile.txt y lo retornamos. Si no, verificamos el caso en el que se encuentre en como primer hijo del nodo.

## Add 
(Nos faltó implementar el split, pero entendimos y realizamos toda la lógica) Al igual que el search, en la insercion hay 2 funciones principales: el add y el addUtil. El usuario al llamar al add simplemente le entrega un registro. Primero verificamos si es que el archivo se encuentra vacio. Si es que lo está creamos un root, insertamos el registro en el dataFile.txt y obtenemos la posición insertada. Luego llenamos de informacion el root y lo insertamos en el indexFile.txt como una hoja. Caso contrario, si el archivo no está vacío, insertamos el registro y obtenemos la posición insertada. Después llamamos al root y llamamos a la función addUtil la cual retornará si es que hay overflow o no en el nodo modificado. Y si lo hay aplicamos splitRoot en el nodo.

La función addUtil comienza por recorrer el nodo en el que se encuentra para encontrar la posición deseada. Luego verificamos si es hoja el nodo en el que nos encontramos. Si lo es lo insertamos y retornamos si hay overflow. Caso contrario, nos movemos a uno de los hijos y realizamos el mismo procedimiento hasta llegar a la posicion correcta. Finalmente, si es que hay overflow aplicamos el split.

## Delete (somos un grupo de 2)
