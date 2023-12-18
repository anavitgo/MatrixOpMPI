
# Otimização Paralela de Algoritmos de Operações em Matrizes com MPI
**Ana Vitoria Gouvea de Oliveira Freitas - 11370196**  
**Alexandre Brito Gomes - 11857323**  

A área da computação utiliza de diversos algoritmos, com diferentes custos computacionais para realizá-los. Uma máxima ideal é que os códigos possam sem executados utilizando-se da menor quantidade de memória, no menor tempo possível e com menor processamento. Às vezes não é possível fazer um balanço perfeito entre essas atividades em uma máquina só, mas ao utilizar de máquinas adicionais e paralelizar essas atividades, essa operação pode se tornar mais eficiente. Um exemplo será mostrado aqui ao realizar algumas operações com matrizes, realizando-as paralelamente com  e compará-las com a versão sequencial. 

# Paralelização

Para paralelizar, utilizamos a biblioteca OpenMP juntamente de MPI para distribuição dos processos em outras máquinas. Por padrão, utilizamos de 8 threads para a paralelização. 

Um detalhe importante do código é que geramos matrizes diferentes para a versão sequencial e para a paralela. Dessa forma, os valores dos elementos são diferentes, gerando resultados totais de soma diferentes. A ideia era verificar o desempenho de cada código dado a memsa quantidade de dados.

Os códigos estão divididos originalmente em:

1.  10 Arquivos.c dividos com as funcionalidades. Eles estão divididos em 5 arquivos paralelos, com prefixo 'par' + sufixo e 5 arquivos sequenciais, com prefixo 'seq' + sufixo, com os seguintes sufixos:
	1. SmallestElement.c
	2. BiggestElement.c
	3. TotalSum.c
	4. SumColumns.c
	5. SumRows.c
	  
2.  Arquivo Makefile para compilação, execução e definição de flags necessárias para paralelização do código



Vamos analisar um trecho do código parSumColumns.c para exemplificar a paralelização geral que utilizamos nas outras funcionalidades.

```
// Sum each column locally using OpenMP

unsigned  long  long  int  *local_sums  = (unsigned  long  long  int  *)calloc(n, sizeof(unsigned  long  long  int));


#pragma  omp  parallel  for  collapse(2)

for (int  j  =  0; j  <  n; j++) {

for (int  i  =  0; i  <  local_rows; i++) {

local_sums[j] +=  local_matrix[i  *  n  +  j];

}

}


// Reduce the local sums across all processes

unsigned  long  long  int  *global_sums  = (unsigned  long  long  int  *)malloc(n  *  sizeof(unsigned  long  long  int));

MPI_Reduce(local_sums, global_sums, n, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

MPI_Barrier(MPI_COMM_WORLD);

```

Utilizamos o  `#pragma omp parallel for collapse(2)
`  para paralelizar o loop de soma, fazemos um  `MPI_Reduce` para unificar as somas e utilizamos de uma ` MPI_Barrier` para garantir que os processos sincronizaram e acabaram.  

## [](https://github.com/anavitgo/ImageProcessingInC#execu%C3%A7%C3%A3o)Execução

Para execução do código, é necessário rodar cada funcionalidade individualmente. Para executá-la, é necessário seguir os seguintes passos: 

1.  `make mpi SEQ_SOURCE=<nome-do-arquivo-sequencial> PAR_SOURCE=<nome-do-arquivo-paralelo>`  -
	 Compila o código. Um exemplo para rodar a funcionalidade de soma de colunas, por exemplo, seria:
	 `make mpi SEQ_SOURCE=seqSumColumns.c PAR_SOURCE=parSumColumns`
		 
2.  `make run MATRIX_DIM=<tamanho-da-matriz> NUM_PROC=<numero-de-processos>`  - Executa o código, podendo passar diferentes parâmetros de tamanho e de número de processos. Um exemplo com uma matriz 100x100 e 4 processos seria:
	`make run MATRIX_DIM=100 NUM_PROC=4`

## Experimentação

Quanto à experimentação, utilizamos um cluster do ICMC, e executamos no host hal2 com o seguinte poder computacional: 
- Architecture:            x86_64
 - CPU op-mode(s):        32-bit, 64-bit
 - Address sizes:         39 bits physical, 48 bits virtual
  - Byte Order:            Little Endian
- CPU(s):                  8
 - On-line CPU(s) list:   0-7
- Vendor ID:               GenuineIntel
 -Model name:            Intel(R) Core(TM) i7-4790 CPU @ 3.60GHz.
 Para realização dos testes, optamos por usar os 8 cores disponíveis nas máquinas para designar as threads com o auxílio do openmp. Para valores de testes, realizaremos usufruindo de 4 6 8 processos com matrizes de tamanho 100, 1000, 10000 e reparar o desempenho. 

### Soma de colunas
| Tamanho / Processos | Sequencial  | Paralelo  |
| ---------------------|-----------------|--------------|
| 100/4                | 0.000410s       | 0.010171s    |
| 1000/4               | 0.021306s       | 0.015266s    |
| 10000/4              | 2.610121s       | 0.287220s    |
| 100/6                | 0.000414s       | 0.018909s    |
| 1000/6               | 0.020773s       | 0.020088s    |
| 10000/6              | 2.609229s       | 0.192948s    |
| 100/8                | 0.000427s       | 0.025398s    |
| 1000/8               | 0.020891s       | 0.021009s    |
| 10000/8              | 2.603226s       | 0.134446s    |

### Soma de linhas
| Tamanho / Processos | Sequencial  | Paralelo  |
| ---------------------|-----------------|--------------|
| 100/4                | 0.000486s       | 0.062767s    |
| 1000/4               | 0.019946s       | 0.061301s    |
| 10000/4              | 1.826744s       |  0.140213s    |
| 100/6                | 0.000496s       | 0.060586s    |
| 1000/6               | 0.020064s       | 0.063575s    |
| 10000/6              | 1.828594s       | 0.115272s    |
| 100/8                | 0.000347s       | 0.061933s    |
| 1000/8               | 0.019812s       | 0.068637s   |
| 10000/8              | 1.827738s       | 0.110755s    |

### Soma total
| Tamanho / Processos | Sequencial  | Paralelo  |
| ---------------------|-----------------|--------------|
| 100/4                | 0.000208s       |  0.021803s    |
| 1000/4               |  0.019678s       |  0.013473s    |
| 10000/4              |  1.770123s       |  0.067667s    |
| 100/6                | 0.000186s       | 0.017975s    |
| 1000/6               | 0.017916s       | 0.019685s    |
| 10000/6              | 1.769945s       | 0.054697s    |
| 100/8                |  0.000257s      |  0.022499s    |
| 1000/8               | 0.017827s       | 0.021640s   |
| 10000/8              |  1.771409s      | 0.046477s    |

### Menor Elemento
| Tamanho / Processos | Sequencial  | Paralelo  |
| ---------------------|-----------------|--------------|
| 100/4                | 0.000205s      |  0.020712s    |
| 1000/4               |  0.021555s       |  0.016016s    |
| 10000/4              |  1.689119s       |  0.062541s    |
| 100/6                | 0.000188s       | 0.046379s    |
| 1000/6               | 0.017021s       | 0.018495s    |
| 10000/6              | 1.691091s       | 0.048851s    |
| 100/8                |  0.000201s     |  0.024153s    |
| 1000/8               | 0.016954s       | 0.024854s  |
| 10000/8              |  1.691016s      | 0.044486s    |

### Maior Elemento
| Tamanho / Processos | Sequencial  | Paralelo  |
| ---------------------|-----------------|--------------|
| 100/4                | 0.000196s      |  0.017480s    |
| 1000/4               |  0.017098s      |  0.015057s    |
| 10000/4              |  1.689428s       |  0.064378s    |
| 100/6                | 0.000202s       | 0.019552s   |
| 1000/6               | 0.017045s       | 0.020489s    |
| 10000/6              | 1.690174s       |  0.047687s    |
| 100/8                |  0.000213s     |  0.024482s    |
| 1000/8               | 0.017054s       | 0.022665s  |
| 10000/8              |  1.688198s      | 0.017121s    |


Podemos observar mais visualmente também por meio dos seguintes gráficos a diferença do sequencial em relação ao paralelo.


### Soma de Colunas

![Alt text](https://github.com/anavitgo/MatrixOpMPI/blob/main/results/sumColumn.png?raw=true)

### Soma de Linhas

![Alt text](https://github.com/anavitgo/MatrixOpMPI/blob/main/results/sumRow.png?raw=true)

### Soma Total 

![Alt text](https://github.com/anavitgo/MatrixOpMPI/blob/main/results/totalSum.png?raw=true)

### Menor Elemento

![Alt text](https://github.com/anavitgo/MatrixOpMPI/blob/main/results/smallestElement.png?raw=true)

### Maior Elemento

![Alt text](https://github.com/anavitgo/MatrixOpMPI/blob/main/results/biggestElement.png?raw=true)

## [](https://github.com/anavitgo/ImageProcessingInC#an%C3%A1lise)Análise

Com a visualização dos resultados, é possível ver que o desempenho sequencial é melhor que  o paralelo em casos onde a matriz é pequena, até 1000x1000, e em alguns casos o paralelo consegue ser melhor até nesse tamanho. Mas a partir de números altos, como 10000x10000, o desempenho do paralelo se torna muito melhor que o do sequencial.  É importante também notar que o melhor desempenho, no geral, se dá com o uso de 8 processos, além com o usufruto da API OpenMP, que aproveita do ambiente compartilhado entre as threads para otimizar ainda mais as tarefas.