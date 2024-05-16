// C program for Huffman Coding
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minheap.h"
#include "utilidades.h"

#define MAX_FILENAME 256

// Calcula las frecuencias de los caracteres, y retorna dos arreglos: uno con los caracteres, y otro con sus respectivas frecuencias.
// Ademas, retorna el tamaño de esos arreglos
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
int comprimir_huffman(char* input_filename, char* output_filename) {
    printf("Iniciando compresión de Huffman\n");

    size_t size;
    char *data = load_file(input_filename, &size); // Carga los datos a codificar
    if (!data) {
        fprintf(stderr, "Error al cargar el archivo de entrada\n");
        return 1;
    }

    printf("Archivo de entrada cargado exitosamente\n");

    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        perror("Error al crear el archivo decodificado");
        free(data);
        return 2;
    }

    printf("Archivo de salida creado exitosamente\n");

    // Calcular tabla de frecuencias
    char c[128] = {0};
    int freq[128] = {0};
    int tam = calculateFrequencyTable(data, &c, &freq);

    printf("Tabla de frecuencias calculada\n");

    MinHeapNode* huffmanTreeRoot = HuffmanCodes(c, freq, tam);

    printf("Árbol de Huffman construido\n");

    char compressionTable[128][MAX_TREE_HT];
    memset(compressionTable, 0, sizeof(char) * 128 * MAX_TREE_HT);
    char arr[MAX_TREE_HT];
    int top = 0;

    createHuffmanTable(huffmanTreeRoot, arr, top, &compressionTable);

    printf("Tabla de compresión creada\n");

    // Reservar espacio de más para el mensaje comprimido
    size_t compressedSizeEstimate = size * MAX_TREE_HT;
    char* compressedMsj = malloc(compressedSizeEstimate);
    if (!compressedMsj) {
        perror("Error al reservar memoria para el mensaje comprimido");
        freeHuffmanTree(huffmanTreeRoot);
        free(data);
        fclose(out);
        return 3;
    }
    memset(compressedMsj, 0, compressedSizeEstimate);

    printf("Memoria reservada para el mensaje comprimido\n");

    // Comprimir el mensaje
    for (size_t i = 0; i < size; i++) {
        strcat(compressedMsj, compressionTable[(unsigned char)data[i]]);
    }

    printf("Mensaje comprimido: %s\n", compressedMsj);

    huffmanData inorder[128] = {0};
    int tamano = 0;
    createInorderTree(huffmanTreeRoot, &inorder, &tamano);

    printf("Árbol de Huffman convertido a inorder\n");

    // Verificar que tamano sea correcto
    if (tamano > 128) {
        fprintf(stderr, "Error: el tamaño del árbol de Huffman es mayor a 128\n");
        freeHuffmanTree(huffmanTreeRoot);
        free(data);
        free(compressedMsj);
        fclose(out);
        return 4;
    }

    // Escribir la tabla de frecuencias (array de structs huffmanData) en el archivo de salida
    size_t written = fwrite(inorder, sizeof(huffmanData), 128, out);
    if (written != 128) {
        perror("Error al escribir la tabla de frecuencias");
        freeHuffmanTree(huffmanTreeRoot);
        free(data);
        free(compressedMsj);
        fclose(out);
        return 5;
    }

    printf("Tabla de frecuencias escrita en el archivo\n");

    // Escribir el mensaje comprimido en el archivo de salida
    size_t compressedLength = strlen(compressedMsj);
    written = fwrite(compressedMsj, sizeof(char), compressedLength, out);
    if (written != compressedLength) {
        perror("Error al escribir el mensaje comprimido");
        freeHuffmanTree(huffmanTreeRoot);
        free(data);
        free(compressedMsj);
        fclose(out);
        return 6;
    }

    printf("Mensaje comprimido escrito en el archivo\n");

    // Liberar memoria y cerrar archivos
    freeHuffmanTree(huffmanTreeRoot);
    free(data);
    free(compressedMsj);
    fclose(out);
    printf("Compresión completada exitosamente\n");
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

        // Si tras movernos en el arbol damos con un nodo que es hoja, entonces este contendrá el caracter equivalente a convertir
        if(isLeaf(search)){
            msj[bitIndex] = search->data.c;
            search = root;
            bitIndex++;
        }
    }

    return msj;
}


int descomprimir_huffman(char* input_filename, char* output_filename) {
    FILE *in = fopen(input_filename, "rb");
    if (!in) {
        perror("Error al abrir el archivo comprimido");
        return 1;
    }

    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        perror("Error al crear el archivo descomprimido");
        fclose(in);
        return 2;
    }

    // Leer los structs huffmanData desde el archivo
    huffmanData inorder[128];
    size_t read_size = fread(inorder, sizeof(huffmanData), 128, in);
    if (read_size != 128) {
        perror("Error al leer la tabla de frecuencias");
        fclose(in);
        fclose(out);
        return 3;
    }

    // Construir el árbol de Huffman a partir de los structs leídos
    MinHeapNode* huffmanTreeRoot = buildTree(inorder, 0, 128);

    // Obtener el tamaño del mensaje comprimido
    fseek(in, 0, SEEK_END);
    long compressedSize = ftell(in) - (128 * sizeof(huffmanData));
    fseek(in, 128 * sizeof(huffmanData), SEEK_SET);

    // Leer el mensaje comprimido
    char* compressedMsj = malloc(compressedSize);
    fread(compressedMsj, sizeof(char), compressedSize, in);

    // Descompresión del mensaje
    int bitIndex = 0;
    MinHeapNode* currentNode = huffmanTreeRoot;

    for (long i = 0; i < compressedSize; i++) {
        char byte = compressedMsj[i];
        for (int j = 0; j < 8; j++) {
            int bit = (byte >> (7 - j)) & 1; // Extraer cada bit del byte
            if (bit) {
                currentNode = currentNode->right;
            } else {
                currentNode = currentNode->left;
            }
            if (isLeaf(currentNode)) {
                fputc(currentNode->data.c, out); // Escribir el carácter descomprimido en el archivo de salida
                currentNode = huffmanTreeRoot; // Volver a la raíz para el siguiente carácter
            }
        }
    }

    // Liberar memoria y cerrar archivos
    freeHuffmanTree(huffmanTreeRoot);
    free(compressedMsj);
    fclose(in);
    fclose(out);
    return 0;
}


int main() {
    char input_filename[MAX_FILENAME];
    char input_filename_noExtension[MAX_FILENAME];
    char output_filename[MAX_FILENAME];
    int choice;

    printf("Programa para comprimir archivos usando Huffman\n");

    while (1) {
        printf("\n1. Cargar y mostrar archivo\n");
        printf("2. Comprimir archivo\n");
        printf("3. Descomprimir archivo\n");
        printf("4. Comparar tamaños\n");
        printf("0. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Ingrese el nombre del archivo: ");
                scanf("%s", input_filename);
                size_t size;
                char *content = load_file(input_filename, &size);
                if (content) {
                    printf("Contenido del archivo:\n%s\n", content);
                    printBlock(content, size);
                    free(content);
                }
                break;
            case 2:
                printf("Ingrese el nombre del archivo a comprimir: ");
                scanf("%s", input_filename);

                // Calcular lugar en donde poner el fin de string, para omitir la extensión al guardarlo
                strcpy(input_filename_noExtension, input_filename);
                input_filename_noExtension[strlen(input_filename) - 4] = '\0';

                sprintf(output_filename, "%s.huf", input_filename_noExtension);

                if (comprimir_huffman(input_filename, output_filename) == 0)
                    printf("Archivo comprimido creado como: %s\n", output_filename);
                else
                    printf("Error al comprimir el archivo.\n");
                break;
            case 3:
                printf("Ingrese el nombre del archivo a descomprimir: ");
                scanf("%s", input_filename);

                // Calcular lugar en donde poner el fin de string, para omitir la extensión al guardarlo
                strcpy(input_filename_noExtension, input_filename);
                input_filename_noExtension[strlen(input_filename) - 4] = '\0';

                sprintf(output_filename, "%s_decoded.txt", input_filename_noExtension);

                if (descomprimir_huffman(input_filename, output_filename) == 0)
                    printf("Archivo descomprimido creado como: %s\n", output_filename);
                else
                    printf("Error al descomprimir el archivo.\n");
                break;
            case 4:

                break;
            case 0:
                return 0;
            default:
                printf("Opción inválida.\n");
        }
    }
}
