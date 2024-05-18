// C program for Huffman Coding
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minheap.h"
#include "utilidades.h"
#include "huffmanops.h"
#define MAX_FILENAME 256

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


    // Calcular tabla de frecuencias (usamos arreglos de 128 porque es la cantidad maxima de caracteres representables por ascii)
    char c[128];
    int freq[128];
    int tam = calculateFrequencyTable(data,&c,&freq);


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
    }

    printf("Mensaje comprimido: %s\n",compressedMsj);


    huffmanData inorder[128];
    int tamano = 0;

    createInorderTree(huffmanTreeRoot,&inorder,&tamano);

    for(int i = 0; i < tamano; i++){
        printf("%c,%d\n",inorder[i].c,inorder[i].freq);
    }

    fwrite(&tamano, sizeof(int), 1, out);
    fwrite(inorder,sizeof(huffmanData),tamano,out); // escribe el array en un archivo
    fwrite(compressedMsj, sizeof(char), strlen(compressedMsj), out);


    // Liberar memoria y cerrar archivos
    freeHuffmanTree(huffmanTreeRoot);
    free(data);
    free(compressedMsj);
    fclose(out);
    return 0;
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

                // Calcular lugar en donde poner el fin de string, para omitir la extension al guardarlo asds
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
