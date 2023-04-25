# Inverso da Raiz com Newton Raphson
Este programa tem como objetivo comparar a eficiência e precisão de diferentes métodos para calcular a raiz quadrada e a raiz quadrada inversa de um número real em ponto flutuante.

Desenvolvido para o Trabalho 3 de Matemática Computacional no curso de Ciência da Computação na UEM.

Os métodos implementados neste programa são:

- Inverso Método de Newton-Raphson para calcular a raiz quadrada
- Método de Newton-Raphson para calcular a raiz quadrada inversa
- Método de Fast sqrt de Tarolli

## Como executar
```powershell
> clang -g sqrt.c -o sqrt.exe
> ./sqrt.exe
```

O programa possui dois modos de execução: o primeiro realiza o calculo para todos os valores entre 0 e N_VALUES e salvo o resultado, tempo de execução e erro em um arquivo csv. É possível renderizar os dados do arquivo com o programa em python:

```ps
> py -m pip install matplotlib pandas
> py plot.py <nome_arquivo>
```

O outro modo de execução é o de valor único. Será requisitada uma entrada e os resultados para essa única entrada serão exibidos no console.