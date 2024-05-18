// C program for Huffman Coding
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minheap.h"
#include "utilidades.h"
#include "huffmanops.h"
#define MAX_FILENAME 256
char* decompress_binary(char* data, size_t size, MinHeapNode* root) {
    MinHeapNode* search = root;
    printf("La data es:\n");
    printf(data);
    printf("\n");
    int charIndex = 0;
    char* msj = (char*)malloc(size * 8); // por cada bit puede haber a lo sumo un caracter
    memset(msj, 0, size * 8);
    printf("Recorriendo el arbol:\n");
    for (int i = 0; i < size; i++) {
        char bit = data[i];

        // Recorrer el árbol
        if (bit == '1') {
            search = search->right;
        } else if (bit == '0') {
            search = search->left;
        } else {
            fprintf(stderr, "Error: bit no válido encontrado en la entrada\n");
            free(msj);
            return NULL;
        }
        // Si tras movernos en el árbol damos con un nodo que es hoja, entonces este contiene el caracter equivalente a convertir
        if (isLeaf(search)) {
            msj[charIndex] = search->data.c;
            search = root;
            charIndex++;
        }

        // Si hemos decodificado tantos caracteres como tamaño de entrada, romper el bucle
        if (charIndex >= size) {
            break;
        }
    }

    // Asegurarse de que el mensaje decodificado sea nulo terminado
    msj[charIndex] = '\0';
    return msj;
}

int comprimir_huffman(char* input_filename,char* output_filename){
    size_t size;
    char *data = load_file(input_filename, &size); // Carga los datos a codificar
    if (!data) return 1;

    FILE *out = fopen(output_filename, "wb");
    if (!out) {
        perror("Error al crear el archivo decodificado\n");
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

    //Ver tabla de compresion
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
    printf("Se armo el arbol de huffman\n");
    printHuffmanTree(huffmanTreeRoot);

    printf("Mensaje comprimido: %s\n",compressedMsj);


    huffmanData inorder[128];
    int tamano = 0;

    createInorderTree(huffmanTreeRoot,&inorder,&tamano);

    printf("Inorder creado \n");

    printHuffmanData(inorder,tamano);

    fwrite(&tamano, sizeof(int), 1, out);
    fwrite(inorder,sizeof(huffmanData),tamano,out); // escribe el array en un archivo
    char* decompressedMessage = decompress_binary(compressedMsj, strlen(compressedMsj), huffmanTreeRoot);
    printf("Mensaje: \n");
    printf(decompressedMessage);
    fwrite(compressedMsj, sizeof(char), strlen(compressedMsj), out);


    // Liberar memoria y cerrar archivos
    freeHuffmanTree(huffmanTreeRoot);
    free(data);
    free(compressedMsj);
    fclose(out);
    return 0;
}




int descomprimir_huffman(char* input_filename, char* output_filename) {
    FILE* in = fopen(input_filename, "rb");
    printf("Iniciando descompresion\n");
    if (!in) {
        perror("Error al abrir el archivo de entrada");
        return 1;
    }
    printf("Se abrio el archivo de entrada\n");
    int tamano;
    fread(&tamano, sizeof(int), 1, in); // Leer el tamaño del array inorder

    huffmanData* inorder = (huffmanData*)malloc(tamano * sizeof(huffmanData));
    if (!inorder) {
        perror("Error al asignar memoria");
        fclose(in);
        return 2;
    }
    printf("Se asigno la memoria: %d\n", tamano);


    fread(inorder, sizeof(huffmanData), tamano, in); // Leer el array inorder
    printf("Inorder leido\n");

    printHuffmanData(inorder,tamano);
    // Reconstruir el árbol de Huffman a partir del array inorder

    MinHeapNode* huffmanTreeRoot = buildTreeFromInorder(inorder, 0, tamano-1);

    printf("Se armo el arbol de huffman\n");
    printHuffmanTree(huffmanTreeRoot);
    
    // Leer el mensaje comprimido
    fseek(in, 0, SEEK_END);
    size_t fileSize = ftell(in) - sizeof(int) - (tamano * sizeof(huffmanData));
    fseek(in, sizeof(int) + (tamano * sizeof(huffmanData)), SEEK_SET);

    char* compressedData = (char*)malloc(fileSize + 1);
    if (!compressedData) {
        perror("Error al asignar memoria");
        free(inorder);
        freeHuffmanTree(huffmanTreeRoot);
        fclose(in);
        return 3;
    }

    fread(compressedData, sizeof(char), fileSize, in);
    compressedData[fileSize] = '\0';

    // Descomprimir el mensaje
    char* decompressedMessage = decompress_binary(compressedData, fileSize, huffmanTreeRoot);

    // Escribir el mensaje descomprimido en el archivo de salida
    FILE* out = fopen(output_filename, "wb");
    if (!out) {
        perror("Error al crear el archivo de salida");
        free(compressedData);
        free(decompressedMessage);
        free(inorder);
        freeHuffmanTree(huffmanTreeRoot);
        return 4;
    }
    printf(decompressedMessage);
    printf("\n");
    fwrite(decompressedMessage, sizeof(char), strlen(decompressedMessage), out);
    fclose(out);

    // Liberar memoria
    free(compressedData);
    free(decompressedMessage);
    free(inorder);
    freeHuffmanTree(huffmanTreeRoot);
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
        printf("3. Descomprimir archivo\n");
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
                scanf("%s", input_filename);

                // Calcular lugar en donde poner el fin de string, para omitir la extension al guardarlo asds
                strcpy(input_filename_noExtension,input_filename);
                input_filename_noExtension[strlen(input_filename)-4]='\0';

                sprintf(output_filename, "%s.huf", input_filename_noExtension);

                if(comprimir_huffman(input_filename, output_filename) == 0)
                    printf("Archivo protegido creado como: %s\n", output_filename);
                break;
            case 3:
                printf("Ingrese el nombre del archivo a comprimir: \n");
                scanf("%s", input_filename);

                // Calcular lugar en donde poner el fin de string, para omitir la extension al guardarlo
                strcpy(input_filename_noExtension,input_filename);
                input_filename_noExtension[strlen(input_filename)-4]='\0';

                sprintf(output_filename, "%s-decompressed.txt", input_filename_noExtension);

                if(descomprimir_huffman(input_filename, output_filename) == 0)
                    printf("Archivo desprotegido creado como: %s\n", output_filename);
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
