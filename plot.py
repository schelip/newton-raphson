import pandas as pd
import matplotlib.pyplot as plt
import sys

def plot(file_name):
    # leitura do arquivo CSV
    df = pd.read_csv(file_name)

    # criação dos dataframes para cada gráfico
    df_resultado = df.pivot(index='Valor', columns='Metodo', values='Resultado')
    df_tempo = df.pivot(index='Valor', columns='Metodo', values='Tempo(s)')
    df_erro = df.pivot(index='Valor', columns='Metodo', values='Erro')

    # criação dos gráficos
    _, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(15,5))

    df_resultado.plot(ax=ax1, lw=2)
    ax1.set_title('Resultado')
    ax1.set_xlabel('Valor')
    ax1.set_ylabel('Resultado')
    ax1.legend(loc='best')

    df_tempo.plot(ax=ax2, lw=2)
    ax2.set_title('Tempo')
    ax2.set_xlabel('Valor')
    ax2.set_ylabel('Tempo (s)')
    ax2.legend(loc='best')

    df_erro.plot(ax=ax3, lw=2)
    ax3.set_title('Erro')
    ax3.set_xlabel('Valor')
    ax3.set_ylabel('Erro')
    ax3.legend(loc='best')

    plt.show()

if __name__ == '__main__':
    if len(sys.argv) == 2:
        plot(sys.argv[1])
    else:
        print("Usage: plot.py <file_name>")
    