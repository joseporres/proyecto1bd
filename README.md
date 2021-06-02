# Proyecto 1 - BD II
## Estructura Registro
En la siguiente estructura se puede apreciar que hemos considerado una lista doblemente enlazada para los registros no eliminados, con la finalidad de poder retroceder. Esto es de vital importancia para algunas funciones, como por ejemplo el search por rangos. Por otra parte, en el caso de la lista de los eliminados es solo una linked list. Sin embargo, ¿cómo sabemos dónde comienza cada una de estas listas?, debido a que puede darse el caso donde la lista de los no eliminados no necesariamente comience por la primera línea del registro main (contraejemplo: se hace insertAll de B y C, luego se añade A y este sería el nuevo inicio de la lista, a pesar de estar en el archivo auxiliar). Por estos motivos, decidimos emplear una cabecera donde no solo se guarde el inicio de la lista de eliminados, sino también de la lista de no eliminados en los atributos nextDel, toDel, next y toNext respectivamente. En los atributos que son igual a m es porque se refiere al archivo principal; mientras que a, al auxiliar y hay que tomar en cuenta que en el main se cuenta un desfase de una línea por la cabecera. Finalmente, el atributo por sobre el cual se ordenarán los registros es nombre tal y como se trabajó en clase.

## Sequential file
### Funciones de utilidad
#### BinarySearch
Como los registros están ordenados por nombre nos será de mucha ayuda para reducir considerablemente la complejidad al momento de querer buscar algún registro en específico.
#### ReadRecord
Recibe como parámetro el nombre del archivo donde se va a realizar la lectura (seqFile.txt o auxAdd.txt) y la posición que va a leer, además retorna el registro leído.
#### Load
Un load bastante rústico, ya que recibe el nombre de un archivo y lo que hace es ir leyéndolo línea por línea. Esta función fue nuestra primera versión de load y al final solo se empleó para hacer debugs manuales.
#### LoadAll
La función más importante de las de utilidad y es un load mejora que parte desde la cabecera y va iterando a través de los next, mientras que el next sea un entero positivo, es decir mayor o igual a 0.
### Funciones principales
#### InsertAll
Esta función solo se llama al inicio para llenar el archivo principal con registros y solo ejecuta su labor si el archivo auxiliar está vacío para que los punteros tengan sentido y no se pierdan.
#### Search exacto
Primero, se llama a LoadAll que nos retorna un vector de Registro. Luego, se aplica una búsqueda binaria que devuelve una posición. En caso la posición sea -1 es porque no lo encontró y directamente retorna un vector vacío. Caso contrario, como este search debe encontrar todos los registros con un nombre en particular y sabemos que los registros están ordenados por nombre, entonces vamos a ir iterando desde pos+1 hacia adelante mientras que el nombre sea el mismo y también que no pase los límites del vector y realizaremos un procedimiento bastante similar, pero en sentido inverso desde pos-1 hacia atrás. Finalmente, retorna el vector de registros encontrados con el nombre que se buscaba.
#### Search por rangos
El search por rangos es prácticamente idéntico al exacto, solo se diferencia en que cuando itera desde pos+1 hacia adelante compara que el nombre sea menor o igual al end.
#### Reconstrucción
Se verifica que en el archivo auxiliar haya una cantidad de líneas mayor o igual a un factor de reconstrucciones determinado por el usuario en el constructor y que por defecto es 5. En caso cumpla con esto, entonces va a llamar a la función LoadAll y a partir de ahí realizar la reconstrucción es bastante sencillo todos los toNext y toPrev cambian por m y su next y prev son secuenciales.
#### Delete
Se aplica un BinarySearch sobre el resultado de LoadAll.
Si no lo encuentra, directamente retorna falso porque no hay ningún registro para borrar con ese nombre.
Caso contrario, tenemos los siguientes casos (en todos los casos se actualiza la cabecera de la lista de eliminados):

    Eliminar al inicio, se actualiza el registro eliminado, el siguiente y en este caso también en la misma cabecera los atributos next y toNext, ya que son los que determinan el inicio de la lista de no eliminados.
    Eliminar al medio, se actualiza el registro eliminado, el previo y el siguiente.
    Eliminar al final, se guarda el next del registro anterior solo que en negativo para no perder del todo su valor, pero para que ya no se considere en el LoadAll. Asimismo, se actualiza el registro eliminado y el previo.

#### Add
Cuando se inserta por cualquier método, se intenta realizar una reconstrucción.
Dentro del Add, tenemos los siguientes casos:

    Insertar en un registro eliminado usando la estrategia LIFO.
    Se itera por sobre sobre los registros eliminados.
    
        Puede insertarlo al inicio, si el previo del registro eliminado es -1 y se actualiza la cabecera, el registro a insertar y el siguiente.
        Puede insertarlo al medio, si no es ni al inicio ni al final y el nombre está acotado por su previo y siguiente, se actualiza el registro a insertar, el previo y el siguiente.
        Puede insertarlo al final, si el siguiente del registro eliminado es -1 y se actualiza el registro a insertar y el previo. En este caso, se aplica una jugada de convertir el next del previo de negativo a positivo para volver a unir al registro nuevo a la lista de no eliminados.
    
    Insertar un registro en el auxiliar utilizando la estrategia Heap.
    Se aplica un BinarySearch sobre el resultado de LoadAll.
    
        Si es un registro con nombre ya existente.
        Se va iterando por sobre los nombres repetidos, en caso haya.
        
            Puede insertarlo al medio, se actualiza el registro a insertar, el previo y el siguiente.
            Puede insertarlo al final, se actualiza el registro a insertar y el previo.
        
        Caso contrario (si es un registro con nombre que no existe).
        Primero, se intenta insertar al inicio haciendo una comparación con el registro de la posición 0, pero si no cumple se tiene que iterar linealmente para insertarlo ya sea al medio de dos registros o al final.
        
            Puede insertarlo al inicio, se actualiza el ex registro inicial, la cabecera y el registro a insertar.
            Puede insertarlo al medio, se actualiza el registro a insertar, el previo y el siguiente.
            Puede insertarlo al final, se actualiza el ex registro final y el registro a insertar.
        
    


## B+TREE Agrupado
Esta implementación cuenta con 3 bloques: el nodo, el registro, y el árbol. Los registros se encuentran en el dataFile.txt y en el indexFile.txt se encuentra el B+TREE de forma lógica. Además, el indexFile.txt tiene como posición 0 a la raíz del árbol.

### Search
Se utilizaron 2 funciones: una de ellas es la que el usuario puede utilizar mientras que la otra es solo de utilidad. En la función visible por el usuario, toma el nombre del alumno y lo transforma a un char[20] para así poder hacer las comparaciones con el key deseado en el searchUtil. Luego de recibir ambos, el nombre en char[20] y el root verificamos si es que el nodo es hoja. Si lo es, simplemente buscamos la posición dentro del nodo y nos movemos al hijo obtenido con la posición. Caso contrario, iteramos en el nodo hoja hasta encontrar el key. Si se encuentra lo traemos del datafile.txt y lo retornamos. Si no, verificamos el caso en el que se encuentre en como primer hijo del nodo.

### Add
(Nos faltó implementar el split, pero entendimos y realizamos toda la lógica) Al igual que el search, en la inserción hay 2 funciones principales: el add y el addUtil. El usuario al llamar al add simplemente le entrega un registro. Primero verificamos si es que el archivo se encuentra vacío. Si es que lo está creamos un root, insertamos el registro en el dataFile.txt y obtenemos la posición insertada. Luego llenamos de información el root y lo insertamos en el indexFile.txt como una hoja. Caso contrario, si el archivo no está vacío, insertamos el registro y obtenemos la posición insertada. Después llamamos al root y llamamos a la función addUtil la cual retornará si es que hay overflow o no en el nodo modificado. Y si lo hay aplicamos splitRoot en el nodo.

La función addUtil comienza por recorrer el nodo en el que se encuentra para encontrar la posición deseada. Luego verificamos si es hoja el nodo en el que nos encontramos. Si lo es lo insertamos y retornamos si hay overflow. Caso contrario, nos movemos a uno de los hijos y realizamos el mismo procedimiento hasta llegar a la posición correcta. Finalmente, si es que hay overflow aplicamos el split.

### Delete (somos un grupo de 2)
