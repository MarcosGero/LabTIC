/// Funciones varias para leer archivos e imprimir bits
// Carga un archivo
char *load_file(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(*size + 1);
    if (!buffer) {
        perror("Error al reservar memoria");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, *size, file);
    buffer[*size] = '\0'; // Para manejo como cadena de caracteres
    fclose(file);
    return buffer;
}

void printChar(const char a){
    printf(" %c: ",a);
    for (int bit = 7; bit > -1; bit--) {
        if (a & (1 << bit)) {
            printf("1");
        }else{
            printf("0");
        }
    }
    printf("\n");
}

int printBlock(char *a, int size){
    for(int i =0;i<size;i++){
        printChar(a[i]);
    }
}

void printFileInBlocks(const char *input_filename, int blockSize) {
    FILE *fp = fopen(input_filename, "rb");
    if (fp == NULL) {
        printf("Error: File pointer is NULL\n");
        return;
    }

    char *buffer = (char *)malloc(blockSize * sizeof(char));
    if (buffer == NULL) {
        printf("Error: Memory allocation failed\n");
        return;
    }

    size_t bytesRead;
    while ((bytesRead = fread(buffer, sizeof(char), blockSize, fp)) > 0) {
        printBlock(buffer, bytesRead);
    }

    free(buffer);
}

// Establece el bit en la posici?n 'pos' (0-indexado)
void set_bit(char *array, int pos, int value) {
    int byte_pos = pos / 8;
    int bit_pos = pos % 8;
    if (value == 1)
        array[byte_pos] |= (1 << 7-bit_pos);
    else
        array[byte_pos] &= ~(1 << 7-bit_pos);
}

// Obt?n el bit en la posici?n 'pos' (0-indexado)
int get_bit(char *array, int pos) {
    int byte_pos = pos / 8;
    int bit_pos = pos % 8;
    return (array[byte_pos] >> 7-bit_pos) & 1;
}

// A utility function to print an array of size n in a buffer
void sprintArr(int arr[], int n,int* buffer)
{
    int i;
    for (i = 0; i < n; ++i)
        sprintf(buffer,"%d", arr[i]);

    sprintf(buffer,"\n");


//    sprintf(output_filename, "%s.huf", input_filename_noExtension);
}
