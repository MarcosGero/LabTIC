#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

#define MAX_FILENAME 260
#define MAX_CONTENT_SIZE 8192

// ANSI escape codes para colores
#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

// Función para mostrar contenido en formato hexadecimal
void print_hex(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%02x ", (unsigned char)data[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    printf("\n");
}

// Función para mostrar contenido en formato binario
void print_binary(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        for (int bit = 7; bit >= 0; --bit) {
            printf("%c", (data[i] & (1 << bit)) ? '1' : '0');
        }
        printf(" ");
        if ((i + 1) % 6 == 0)
            printf("\n");
    }
    printf("\n");
}

// Función para mostrar contenido en formato de caracteres
void print_chars(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%c", data[i]);
    }
    printf("\n");
}

// Función para mostrar contenido de dos archivos en la consola en hexadecimal
void show_files_in_console_hex(const char* file1Content, size_t file1Size, const char* file2Content, size_t file2Size) {
    printf(RED "Archivo Original (Hexadecimal):\n" RESET);
    print_hex(file1Content, file1Size);

    printf(GREEN "Archivo Comprimido (Hexadecimal):\n" RESET);
    print_hex(file2Content, file2Size);
}

// Función para mostrar contenido de dos archivos en la consola en binario
void show_files_in_console_binary(const char* file1Content, size_t file1Size, const char* file2Content, size_t file2Size) {
    printf(RED "Archivo Original (Binario):\n" RESET);
    print_binary(file1Content, file1Size);

    printf(GREEN "Archivo Comprimido (Binario):\n" RESET);
    print_binary(file2Content, file2Size);
}

// Función para mostrar contenido de dos archivos en la consola en caracteres
void show_files_in_console_chars(const char* file1Content, size_t file1Size, const char* file2Content, size_t file2Size) {
    printf(RED "Archivo Original (Caracteres):\n" RESET);
    print_chars(file1Content, file1Size);

    printf(GREEN "Archivo Comprimido (Caracteres):\n" RESET);
    print_chars(file2Content, file2Size);
}

// Función principal
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
        printf("4. Comparar tamaños (Hexadecimal)\n");
        printf("5. Comparar tamaños (Binario)\n");
        printf("6. Comparar tamaños (Caracteres)\n");
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
                    free(content);
                } else {
                    printf("Error al cargar el archivo.\n");
                }
                break;
            case 2:
                printf("Ingrese el nombre del archivo a comprimir: \n");
                scanf("%s", input_filename);

                strcpy(input_filename_noExtension, input_filename);
                input_filename_noExtension[strlen(input_filename) - 4] = '\0';

                sprintf(output_filename, "%s.huf", input_filename_noExtension);

                if (comprimir_huffman(input_filename, output_filename) == 0)
                    printf("Archivo comprimido creado como: %s\n", output_filename);
                break;
            case 3:
                printf("Ingrese el nombre del archivo a descomprimir: \n");
                scanf("%s", input_filename);

                strcpy(input_filename_noExtension, input_filename);
                input_filename_noExtension[strlen(input_filename) - 4] = '\0';

                sprintf(output_filename, "%s-decompressed.txt", input_filename_noExtension);

                if (descomprimir_huffman(input_filename, output_filename) == 0)
                    printf("Archivo descomprimido creado como: %s\n", output_filename);
                break;
            case 4:
                printf("Ingrese el nombre del archivo original: ");
                scanf("%s", input_filename);
                size_t original_size;
                char *original_content = load_file(input_filename, &original_size);
                if (!original_content) {
                    printf("Error al cargar el archivo original.\n");
                    break;
                }

                printf("Ingrese el nombre del archivo comprimido: ");
                scanf("%s", output_filename);
                size_t compressed_size;
                char *compressed_content = load_file(output_filename, &compressed_size);
                if (!compressed_content) {
                    printf("Error al cargar el archivo comprimido.\n");
                    free(original_content);
                    break;
                }

                show_files_in_console_hex(original_content, original_size, compressed_content, compressed_size);

                free(original_content);
                free(compressed_content);
                break;
            case 5:
                printf("Ingrese el nombre del archivo original: ");
                scanf("%s", input_filename);
                size_t original_size_bin;
                char *original_content_bin = load_file(input_filename, &original_size_bin);
                if (!original_content_bin) {
                    printf("Error al cargar el archivo original.\n");
                    break;
                }

                printf("Ingrese el nombre del archivo comprimido: ");
                scanf("%s", output_filename);
                size_t compressed_size_bin;
                char *compressed_content_bin = load_file(output_filename, &compressed_size_bin);
                if (!compressed_content_bin) {
                    printf("Error al cargar el archivo comprimido.\n");
                    free(original_content_bin);
                    break;
                }

                show_files_in_console_binary(original_content_bin, original_size_bin, compressed_content_bin, compressed_size_bin);

                free(original_content_bin);
                free(compressed_content_bin);
                break;
            case 6:
                printf("Ingrese el nombre del archivo original: ");
                scanf("%s", input_filename);
                size_t original_size_char;
                char *original_content_char = load_file(input_filename, &original_size_char);
                if (!original_content_char) {
                    printf("Error al cargar el archivo original.\n");
                    break;
                }

                printf("Ingrese el nombre del archivo comprimido: ");
                scanf("%s", output_filename);
                size_t compressed_size_char;
                char *compressed_content_char = load_file(output_filename, &compressed_size_char);
                if (!compressed_content_char) {
                    printf("Error al cargar el archivo comprimido.\n");
                    free(original_content_char);
                    break;
                }

                show_files_in_console_chars(original_content_char, original_size_char, compressed_content_char, compressed_size_char);

                free(original_content_char);
                free(compressed_content_char);
                break;
            case 0:
                return 0;
            default:
                printf("Opción inválida.\n");
        }
    }
}
