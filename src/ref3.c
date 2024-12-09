#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"
#include <omp.h>

#define NUM_RECORDS 30000
#define THRESHOLD_CA_MIN 0.25f
#define THRESHOLD_CB_MAX 0.75f

typedef struct {
  char id[6];
  float value;
} Record;

void load_records(const char *filename, Record *records) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Erro ao abrir o arquivo %s.\n", filename);
    exit(1);
  }
  for (int i = 0; i < NUM_RECORDS; ++i) {
    fscanf(file, "%f", &records[i].value);
  }
  fclose(file);
}

void load_ids(const char *filename, char (*ids)[6]) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Erro ao abrir o arquivo %s.\n", filename);
    exit(1);
  }
  for (int i = 0; i < NUM_RECORDS; ++i) {
    fscanf(file, "%s", ids[i]);
  }
  fclose(file);
}

void combine_ids(const char *id1, const char *id2, char* result) {
  result[0] = id1[0];
  result[1] = id2[1];
  result[2] = id1[2];
  result[3] = id2[3];
  result[4] = id1[4];
  result[5] = '\0';
  //return result;
}

int compare_records(const void *a, const void *b) {
  float f1 = *((float *)a);
  float f2 = *((float *)b);
  if (f1 < f2)
    return -1;
  if (f1 > f2)
    return 1;
  return 0;
}

int main() {
  // Aloca memória dinamicamente para os registros e IDs
  double start = omp_get_wtime();
  Record *recordsA = (Record *)malloc(NUM_RECORDS * sizeof(Record));
  Record *recordsB = (Record *)malloc(NUM_RECORDS * sizeof(Record));
  //Record **recpA=(Record **)malloc(NUM_RECORDS * sizeof(Record*));
  //Record **recpB=(Record **)malloc(NUM_RECORDS * sizeof(Record*));
  
  char(*ids)[6] = (char(*)[6])malloc(NUM_RECORDS * sizeof(*ids));

  if (!recordsA || !recordsB) {
    fprintf(stderr, "Erro ao alocar memória.\n");
    exit(1);
  }
  load_records("A.txt", recordsA);
  load_records("B.txt", recordsB);
  load_ids("ids.txt", ids); // Assuming ids are the same for both A and B
  //load_ids("ids.txt",idsB);
  FILE *output = fopen("output_ref3.csv", "w");
  if (!output) {
    fprintf(stderr, "Erro ao abrir o arquivo de saída.\n");
    free(recordsA);
    free(recordsB);
    free(ids);
    return 1;
  }
  fprintf(output, "ID_a_m,ID_b_M,ID',a_m,b_M,f\n");

  //use representa se o registro é usado em alguma combinação ou não
  int *useA=(int *)malloc(NUM_RECORDS * sizeof(int));
  int *useB=(int *)malloc(NUM_RECORDS * sizeof(int));
  struct hashMap* mp
        = (struct hashMap*)malloc(sizeof(struct hashMap));
  initializeHashMap(mp);

  //determinando os registros a serem usados
  //em A removemos o elemento máximo (nunca ganha na comparação) e aqueles abaixo do threshold
    int id=0;
    for(int i=0;i<NUM_RECORDS;i++){
      insert(mp,ids[i],i);
      if(recordsA[i].value>THRESHOLD_CA_MIN){
        useA[i]=1;
        if(recordsA[i].value>recordsA[id].value){
          id=i;
        }        
      }
      else useA[i]=0;
    }
    useA[id]=0;

    //em B removemos o elemento mínimo (nunca ganha na comparação) e aqueles acima do threshold
    id=0;
    for(int i=0;i<NUM_RECORDS;i++){
      if(recordsB[i].value<THRESHOLD_CB_MAX){
        useB[i]=1;
        if(recordsB[i].value<recordsB[id].value){
          id=i;
        }        
      }
      else useB[i]=0;
    }
    useB[id]=0;
    char combined_id[6];

    //definição do loop principal. Cada par de registros A e B só será combinado uma única vez para evitar computação adicional
    #pragma omp parallel for collapse(2) schedule(dynamic) firstprivate(combined_id)
    for (int Ca = 0; Ca < NUM_RECORDS; ++Ca) {
      for (int Cb = 0; Cb < NUM_RECORDS; ++Cb) {
          if (useA[Ca] && useB[Cb]){

            combine_ids(ids[Ca], ids[Cb], combined_id);
            int idx = search(mp, combined_id);
            if (idx != -1){
              float product = (recordsA[idx].value * recordsB[idx].value) * 
                          (recordsA[Ca].value * recordsB[Cb].value);

              #pragma omp critical
              fprintf(output, "%s,%s,%s,%f,%f,%f\n", 
                      ids[Ca], ids[Cb], combined_id, 
                     recordsA[Ca].value, recordsB[Cb].value, product);
            }
          }
        }
    }
  fclose(output);

  // Ordena os registros com base no valor da coluna f
  // NOTE: This is a simplified approach; for large datasets, external sorting
  // would be more appropriate
  //system("sort -t, -k6 -n output_ref3.csv -o sorted_output_ref3.csv");
  system("(head -n 1 output_ref3.csv && tail -n +2 output_ref3.csv | sort -t, -k6 -n) > sorted_output_ref3.csv");
  // Descrição do comando:
  // sort: O comando para ordenar.
  // -t,: Define a vírgula (,) como delimitador de campo.
  // -k6: Especifica que a ordenação deve ser feita com base no sexto campo.
  // -n: Realiza uma ordenação numérica.
  // output.csv: O arquivo de entrada.
  // -o sorted_output.csv: Especifica o arquivo de saída.

  // Libera a memória alocada dinamicamente
  freeHashMap(mp);
  free(ids);
  free(recordsA);
  free(recordsB);
  free(useA);
  free(useB);
  printf("Processamento completo. Resultados salvos em sorted_output_ref3.csv.\n");

  double end = omp_get_wtime();
  printf("Elapsed time: %f seconds\n", end - start);

  return 0;
}