# Proyecto 1 - BD II
## Estructura Registro
En la siguiente estructura se puede apreciar que hemos considerado una lista doblemente enlazada para los registros no eliminados, con la finalidad de poder retroceder. Esto es de vital importancia para algunas funciones, como por ejemplo el \textit{search} por rangos. Por otra parte, en el caso de la lista de los eliminados es solo una \textit{linked list}. Sin embargo, ¿cómo sabemos dónde comienza cada una de estas listas?, debido a que puede darse el caso donde la lista de los no eliminados no necesariamente comience por la primera línea del registro \textit{main} (contraejemplo: se hace \textit{insertAll} de B y C, luego se añade A y este sería el nuevo inicio de la lista, a pesar de estar en el archivo auxiliar). Por estos motivos, decidimos emplear una cabecera donde no solo se guarde el inicio de la lista de eliminados, sino también de la lista de no eliminados en los atributos \textit{nextDel}, \textit{toDel}, \textit{next} y \textit{toNext} respectivamente. En los atributos que son igual a m es porque se refiere al archivo principal; mientras que a, al auxiliar y hay que tomar en cuenta que en el \textit{main} se cuenta un desfase de una línea por la cabecera. Finalmente, el atributo por sobre el cual se ordenarán los registros es nombre tal y como se trabajó en clase.

## Sequential file
###
###
###

## B+TREE Agrupado
Esta implementación cuenta con 3 bloques: el nodo, el registro, y el árbol. Los registros se encuentran en el dataFile.txt y en el indexFile.txt se encuentra el B+TREE de forma lógica. Además, el indexFile.txt tiene como posición 0 a la raíz del árbol.

### Search
Se utilizaron 2 funciones: una de ellas es la que el usuario puede utilizar mientras que la otra es solo de utilidad. En la función visible por el usuario, toma el nombre del alumno y lo transforma a un char[20] para así poder hacer las comparaciones con el key deseado en el searchUtil. Luego de recibir ambos, el nombre en char[20] y el root verificamos si es que el nodo es hoja. Si lo es, simplemente buscamos la posición dentro del nodo y nos movemos al hijo obtenido con la posición. Caso contrario, iteramos en el nodo hoja hasta encontrar el key. Si se encuentra lo traemos del datafile.txt y lo retornamos. Si no, verificamos el caso en el que se encuentre en como primer hijo del nodo.

### Add
(Nos faltó implementar el split, pero entendimos y realizamos toda la lógica) Al igual que el search, en la inserción hay 2 funciones principales: el add y el addUtil. El usuario al llamar al add simplemente le entrega un registro. Primero verificamos si es que el archivo se encuentra vacío. Si es que lo está creamos un root, insertamos el registro en el dataFile.txt y obtenemos la posición insertada. Luego llenamos de información el root y lo insertamos en el indexFile.txt como una hoja. Caso contrario, si el archivo no está vacío, insertamos el registro y obtenemos la posición insertada. Después llamamos al root y llamamos a la función addUtil la cual retornará si es que hay overflow o no en el nodo modificado. Y si lo hay aplicamos splitRoot en el nodo.

La función addUtil comienza por recorrer el nodo en el que se encuentra para encontrar la posición deseada. Luego verificamos si es hoja el nodo en el que nos encontramos. Si lo es lo insertamos y retornamos si hay overflow. Caso contrario, nos movemos a uno de los hijos y realizamos el mismo procedimiento hasta llegar a la posición correcta. Finalmente, si es que hay overflow aplicamos el split.

### Delete (somos un grupo de 2)
