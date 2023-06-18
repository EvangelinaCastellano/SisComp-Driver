import time
import threading
import subprocess
import matplotlib.pyplot as plt

# Listas para almacenar los valores leídos
x_values = []
y_values = []

# Evento de reinicio
reset_event = threading.Event()

def write_to_proc(value):
    global x_values, y_values
    command = f'echo "{value}" > /proc/signal_file'
    subprocess.run(command, shell=True)
    reset_event.set()  # Configurar el evento de reinicio

def read_to_proc():
    command = f'cat /proc/signal_file'
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    return result.stdout

def thread_func():
    while True:
        result = read_to_proc()
        # Obtener el tiempo actual
        current_time = time.time()

        if reset_event.is_set():
            # Reiniciar el gráfico
            reset_event.clear()
            x_values.clear()
            y_values.clear()
            plt.clf()
        
        # Agregar el valor leído a las listas
        #x_values.append(current_time)
        x_values.append(len(x_values) + 1)
        y_values.append(float(result))
        
        # Actualizar la gráfica
        plt.plot(x_values, y_values, 'b-')
        plt.xlabel('Tiempo')
        plt.ylabel('Valor')
        plt.title('Gráfico en tiempo real')
        plt.grid(True)
        plt.pause(1)  # Pausar por 1 segundo antes de la siguiente actualización

if __name__ == '__main__':
    plt.ion()  # Modo interactivo para la gráfica en tiempo real
    
    while True:
        user_input = input("Ingrese la señal a graficar (1 o 2): ")
        
        if user_input in ('1', '2'):
            write_to_proc(user_input)
            
            # Crear un hilo y pasar la función thread_func como objetivo
            thread = threading.Thread(target=thread_func)

            # Iniciar la ejecución del hilo
            thread.start()
        else:
            print("Valor no válido. Intente nuevamente.")
