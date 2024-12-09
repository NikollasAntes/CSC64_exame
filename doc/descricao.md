# Descrição

## Contexto

O programa descrito abaixo processa dois registros de conjuntos, A e B, e combina seus IDs de forma a gerar novos identificadores e calcular valores com base nos registros combinados. O objetivo desse projeto é acelerar a execução, utilizando, para tal fim, OpenMP para paralelizar o processamento. Assim, é possível lidar de forma eficiente com grandes volumes de dados.

## Estrutura geral do programa

1. Carregamento de dados

Os registros e IDs são carregados a partir de arquivos de texto gerados pelo arquivo db.c.

Cada registro contém um identificador e um valor associado a ele.

2. Filtragem de dados

Em `recordsA`, são  ignorados registros com valores menores do que ou iguais a 0.25.

Em `recordsB`, são ignorados valores com registros maiores do que ou iguais a 0.75.

3. Combinação de registros

A função `combine_ids` gera novos identificadores, com base em uma combinação dos Ids originais.

4. Processamento paralelo

Combinações de registros são processados em paralelo, com uso do OpenMP.

Os valores gerados são salvos em um arquivo csv.

5. Ordenação dos resultados

O registro têm os valores ordenados de forma ascendente com base nos valores da coluna f.

## Paralelização

A paralelização é realizada no laço principal, que processa as combinações dos registros de `recordA` e de `recordB`.

O uso do OpenMP se dá pelo uso da diretiva `#pragma omp parallel for collapse(2) schedule(dynamic) firstprivate(combined_id)`.
- `collapse(2)` combina os dois laços de iteração em um espaço de iteração para melhor distribuição entre as threads.
- `schedule(dynamic)` divide as tarefas dinamicamente entre as threads para balancear a carga de trabalho.
- `firstprivate(combined_id)` garante que cada thread tenha sua própria cópia da variável `combined_id`.

O acesso ao arquivo de saída é protegido por uma região crítica para evitar escrita concorrente, pelo uso da diretiva `#pragma omp critical`

## Estrutura adicional: tabela hash

A solução apresentada utiliza a tabela hash para armazenar e buscar combinações de IDs de forma eficiente.

Cada entrada da tabela contém uma chave e um valor associado.

É usada uma lista encadeada para lidar com colisões.

A tabela possui as operações:
- `insert`: novos pares chave-valor são adicionados ao início da lista encadeada do bucket correspondente.
- `delete`: remove pares chave-valor quando necessário.
- `search`: permite verificar se uma combinação de IDs já foi processada.
- `freeHashMap`: libera a memória alocada ao final.

## Conclusão

A solução apresentada utiliza OpenMP para paralelização, fazendo o uso dos recursos computacionais de forma mais eficiente, a fim de reduzir o tempo de execução. O uso de uma estrutura de dados eficiente, por meio da tabela hash permite buscas rápidas e evita processamento redundante. Dessa forma, é possível lidar com grandes conjuntos de dados.