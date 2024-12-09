# Tutorial

## Pré-requisitos

Assumindo um ambiente Linux, execute os seguintes comandos no terminal para instalar o compilador e as bibliotecas necessárias:
```
sudo apt update
sudo apt install build-essential libomp-dev -y
```

## Clonar o repositório com os arquivos do projeto

```
git clone xxxxxxxxxxxxxxx
cd xxxxxxxxxxxxxxxx
```

## Compilação e execução

Primeiramente, deve-se compilar o arquivo db.c, a fim de gerar os arquivos de texto. Na pasta src, rode:

```
gcc -o db db.c
./db
```

Para compilar os arquivos seq.c e ref3.c, execute, na pasta src:

```
gcc -o seq seq.c
gcc -o ref3 ref3.c -fopenmp
```

Para executar o seq.c, já compilado:

```
./seq
```

Para executar o ref3.c, já compilado:

```
./ref3
```

Para definir o número de threads, deve-se rodar o comando export OMP_NUM_THREADS=<NÚMERO>.

Por exemplo, para rodar com 8 threads:
```
export OMP_NUM_THREADS=8 && ./ref3
```