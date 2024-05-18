// C program for Huffman Coding
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minheap.h"
#include "utilidades.h"

#define MAX_FILENAME 256

// Calcula las frecuencias de los caracteres, y retorna dos arreglos: uno con los caracteres, y otro con sus respectivas frecuencias.
// Ademas, retorna el tama?o de esos arreglos
int calculateFrequencyTable(char* texto, char* letras, int* frecuencias){
    int size = strlen(texto);
    int freqTmp[128];

    // Prepara los arreglos
    memset(&freqTmp,0,128*sizeof(int));
    memset(frecuencias,0,128*sizeof(int));
    memset(letras,0,128);

    printf("\ntext: %s; size: %d\n",texto,size);

    // Escanea todo el archivo y va contando
    for(int i = 0; i < size; i++){
        freqTmp[texto[i]]++;
    }

    int nonZeroIndex = 0; // Indice para ir llenando los arreglos letras y frecuencias
    // Muestra la tabla de las cantidades
    for(int i = 0; i < 128; i++){
        if (freqTmp[i] != 0){
//            printf("%c: %d\n",i,freqTmp[i]);
            frecuencias[nonZeroIndex] = freqTmp[i];
            letras[nonZeroIndex] = i;
            nonZeroIndex++;
        }
    }
    return nonZeroIndex;

}

// The main function that builds Huffman tree
MinHeapNode* buildHuffmanTree(char data[], int freq[], int size){
    MinHeapNode *left, *right, *top;

    // Step 1: Create a min heap of capacity
    // equal to size. Initially, there are
    // modes equal to size.
    MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

    // Iterate while size of heap doesn't become 1
    while (!isSizeOne(minHeap)) {

        // Step 2: Extract the two minimum
        // freq items from min heap
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        // Step 3: Create a new internal
        // node with frequency equal to the
        // sum of the two nodes frequencies.
        // Make the two extracted node as
        // left and right children of this new node.
        // Add this node to the min heap
        // '$' is a special value for internal nodes, not
        // used
        top = newNode('$', left->data.freq + right->data.freq);

        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }

    // Step 4: The remaining node is the
    // root node and the tree is complete.
    return extractMin(minHeap);
}

// Prints huffman codes from the root of Huffman Tree.
// It uses arr[] to store codes
void printCodes(MinHeapNode* root, int arr[],int top){

    // Nota: el recorrido es post-order

    // Assign 0 to left edge and recur
    if (root->left) {

        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }

    // Assign 1 to right edge and recur
    if (root->right) {

        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }

    // If this is a leaf node, then
    // it contains one of the input
    // characters, print the character
    // and its code from arr[]
    if (isLeaf(root)) {
        printf("%c: ", root->data);
        printArr(arr, top); // Aqui esta almacenado el codigo
    }
}


// Funcion auxiliar para liberar el arbol creado por el algoritmo de huffman
void freeHuffmanTree(MinHeapNode* root){
    if (root == NULL) return;

    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    free(root);
}

// The main function that builds a
// Huffman Tree and print codes by traversing
// the built Huffman Tree
MinHeapNode* HuffmanCodes(char data[], int freq[], int size){
    // Construct Huffman Tree
    MinHeapNode* root = buildHuffmanTree(data, freq, size);

    // Print Huffman codes using
    // the Huffman tree built above
    int arr[MAX_TREE_HT], top = 0;

    printCodes(root, arr, top);

    return root;
}

/* UTILITY FUNCTIONS */
/* Function to find index of the maximum value in arr[start...end] */

int max (huffmanData arr[], int strt, int end)
{
    int i, max = arr[strt].freq, maxind = strt;

    for(i = strt+1; i <= end; i++)
    {
        if(arr[i].freq > max)
        {
            max = arr[i].freq;
            maxind = i;
        }
    }

    return maxind;
}


/* Recursive function to construct binary of size len from
   Inorder traversal inorder[]. Initial values of start and end
   should be 0 and len -1.  */
MinHeapNode* buildTree (huffmanData inorder[], int start, int end)
{
    if (start > end)
        return NULL;

    /* Find index of the maximum element from Binary Tree */
    int i = max(inorder, start, end);

    /* Pick the maximum value and make it root */
    MinHeapNode *root = newNode(inorder[i].c,inorder[i].freq);

    /* If this is the only element in inorder[start..end],

       then return it */
    if (start == end)
        return root;

    /* Using index in Inorder traversal, construct left and
       right subtress */

    root->left  = buildTree (inorder, start, i-1);
    root->right = buildTree (inorder, i+1, end);
    return root;
}


void createInorderTree(MinHeapNode* root, huffmanData* inorder,int* size){

    // Caso base
    if (root == NULL){
        return;
    }

    // Recorrido recursivo inorder
    createInorderTree(root->left,inorder,size);
    inorder[*size] = root->data;
    (*size)++;
    createInorderTree(root->right,inorder,size);
}


void createHuffmanTable(MinHeapNode* root,char arr[],int top,char* table){
    // Nota: el recorrido es post-order

    // Assign 0 to left edge and recur
    if (root->left) {
        arr[top] = '0';
        createHuffmanTable(root->left, arr, top + 1,table);
    }

    // Assign 1 to right edge and recur
    if (root->right) {
        arr[top] = '1';
        createHuffmanTable(root->right, arr, top + 1,table);
    }

    if (isLeaf(root)) {
//        printf("%c: %s\n",root->data.c,arr);
        arr[top+1] = '\0'; // Para convertirlo en string
        strncpy(table+root->data.c*MAX_TREE_HT,arr,top);
    }
}

int comprimir_huffman(char* input_filename,char* output_filename){
    size_t size;
    char *data = load_file(input_filename, &size); // Carga los datos a codificar
    if (!data) return 1;

    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        perror("Error al crear el archivo decodificado");
        free(data);
        return 2;
    }

    // Mostrar dato sin codificar
//    printBlock(data,size);

    // Calcular tabla de frecuencias (usamos arreglos de 128 porque es la cantidad maxima de caracteres representables por ascii)
    char c[128];
    int freq[128];
    int tam = calculateFrequencyTable(data,&c,&freq);

//    for(int i = 0; i < tam; i++){
//        printf("%c,%d\n",c[i],freq[i]);
//    }

    MinHeapNode* huffmanTreeRoot = HuffmanCodes(c,freq,tam);

    char compressionTable[128][MAX_TREE_HT];
    memset(&compressionTable,0,sizeof(char)*128*MAX_TREE_HT);
    char arr[MAX_TREE_HT]; // Armar peque?o acumulador para los codigos obtenidos en el barrido del arbol de huffman
    int top = 0;

    createHuffmanTable(huffmanTreeRoot,arr,top,&compressionTable);

    // Ver tabla de compresion
    printf("\nTABLA COMPRESION: \n");
    for(int i = 0; i < 128; i++){
        printf("%c: %s\n",i,compressionTable[i]);
    }

    // Reservar espacio de mas para el mensaje comprimido
    char* compressedMsj = malloc(sizeof(char)*size*MAX_TREE_HT);
    memset(compressedMsj,0,size*MAX_TREE_HT);

    for(int i = 0; i < size; i++){

        strcat(compressedMsj,compressionTable[data[i]]);
//        printf("%c: ",data[i]);
//        printf("%s\n",compressionTable[data[i]]);
    }

    printf("Mensaje comprimido: %s\n",compressedMsj);


    huffmanData inorder[128];
    int tamano = 0;

    createInorderTree(huffmanTreeRoot,&inorder,&tamano);

    for(int i = 0; i < tamano; i++){
        printf("%c,%d\n",inorder[i].c,inorder[i].freq);
    }

    // test
    MinHeapNode* newRoot = buildTree(inorder,0,tamano);

    fwrite(inorder,sizeof(huffmanData),tamano,out); // escribe el array en un archivo


    // Liberar memoria y cerrar archivos
    freeHuffmanTree(huffmanTreeRoot);
    free(data);
    free(compressedMsj);
    fclose(out);
    return 0;
}

char* decompress_binary(char* input_filename,size_t size, MinHeapNode* root){
    MinHeapNode* search = root;
    int bitIndex = 0;
    char* msj = malloc(size*8); // por cada bit puede haber a lo sumo un caracter
    memset(msj,0,size*8);

    for(int i = 0; i < size*8;i++){
        int bit = get_bit(input_filename,i);
        //printf("%d\n",bit);
        // Recorre el arbol
        if (bit){
            search = search->right;
        }
        else{
            search = search->left;
        }

        // Si tras movernos en el arbol damos con un nodo que es hoja, entonces este contendr? el caracter equivalente a convertir
        if(isLeaf(search)){
            msj[bitIndex] = search->data.c;
            search = root;
            bitIndex++;
        }
    }

    return msj;
}


int descomprimir_huffman(char* input_filename,char* output_filename){
    size_t size;
    char *data = load_file(input_filename, &size); // Carga los datos a codificar
    if (!data) return 1;

    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        perror("Error al crear el archivo decodificado");
        free(data);
        return 2;
    }


    // Mostrar dato sin codificar
    printBlock(data,size);

    // En esta parte se debe leer el arbol del archivo
    char c[128];
    int freq[128];
    int tam = calculateFrequencyTable(data,&c,&freq);

    for(int i = 0; i < tam; i++){
        printf("%c,%d\n",c[i],freq[i]);
    }




    char table[128][MAX_TREE_HT];


    MinHeapNode* huffmanTreeRoot = HuffmanCodes(c,freq,tam); // Retorna la raiz del arbol de huffman, para poder guardarlo en el archivo


    // Descompresion

    // Escribir en el archivo de salida: el arbol de huffman (como array inorder de nodos, y mensaje comprimido)
    int textLength = strlen(data);
    int bitIndex = 0; // Va a recorrer todo el string a codificar, bit por bit
    char batchedBits = 0; // Solo se puede escribir de a bytes en un archivo
    int batchedBitsIndex = 0;

    // Iterar sobre el mensaje a decodificar. Por cada caracter, recorreremos el arbol de huffman para obtener su codigo, y asi escribirlo en el archivo
    for(int i = 0; i < textLength; i++){
        MinHeapNode* root = huffmanTreeRoot; // Armar copia local de la raiz del arbol
        while(!isLeaf(root)){ // Iterar hasta encontrarse con una hoja (o sea, un nodo con un caracter valido)

            // Codificar en huffman usando el arbol de decision
            int bit = get_bit(data,bitIndex);
            printf("%d\n",bit);
            int codedBit = 0;
            if (bit){
                root = root->right;
            }
            else{
                root = root->left;
            }
            bitIndex++;

            set_bit(&batchedBits,batchedBitsIndex,bit);


            // Armar grupo de bits para escribir
            batchedBitsIndex = (batchedBitsIndex+1)%8;
//            printf("batch: %d\n",batchedBitsIndex);


            if (batchedBitsIndex%8==0){
                printf("LOTE LISTO PARA ESCRIBIR: %c\n",batchedBits);
//                fwrite(batchedBits,1,1,out);
                fputc(batchedBits,out);
            }
        }
    }


    // Liberar memoria y cerrar archivos
    freeHuffmanTree(huffmanTreeRoot);
    free(data);
    fclose(out);
    return 0;
}


int main() {
    char input_filename[MAX_FILENAME];
    char input_filename_noExtension[MAX_FILENAME];
    char output_filename[MAX_FILENAME];
    int choice;
    int block_size;

    printf("Programa para comprimir archivos usando Huffman\n");

    while (1) {
        printf("\n1. Cargar y mostrar archivo\n");
        printf("2. Comprimir archivo\n");
        printf("3. Descomprimir errores\n");
        printf("4. Comparar tama?os\n");
        printf("0. Salir\n");
        printf("Seleccione una opci?n: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Ingrese el nombre del archivo: ");
                scanf("%s", input_filename);
                size_t size;
                char *content = load_file(input_filename, &size);
                if (content) {
                    printf("Contenido del archivo:\n%s\n", content);
                    printBlock(content,size);
                    free(content);
                }
                break;
            case 2:
                printf("Ingrese el nombre del archivo a comprimir: \n");
//                scanf("%s", input_filename);
                strcpy(input_filename,"xd.txt");

                // Calcular lugar en donde poner el fin de string, para oasdmitir la extension al guardarlo
                strcpy(input_filename_noExtension,input_filename);
                input_filename_noExtension[strlen(input_filename)-4]='\0';

                sprintf(output_filename, "%s.huf", input_filename_noExtension);

                if(comprimir_huffman(input_filename, output_filename) == 0)
                    printf("Archivo protegido creado como: %s\n", output_filename);
                break;
            case 3:

                break;
            case 4:

                break;
            case 0:
                return 0;
            default:
                printf("Opci?n inv?lida.\n");
        }
    }
}
