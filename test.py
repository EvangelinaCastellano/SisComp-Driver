import subprocess

def write_to_clipboard(value):
    command = f'echo "{value}" > /proc/clipboard'
    subprocess.run(command, shell=True)

if __name__ == '__main__':
    while True:
        user_input = input("Ingrese el valor para escribir en /proc/clipboard (1 o 2): ")
        
        if user_input in ('1', '2'):
            write_to_clipboard(user_input)
            print("Valor ejecutado en /proc/clipboard.")
        else:
            print("Valor no válido. Intente nuevamente.")
