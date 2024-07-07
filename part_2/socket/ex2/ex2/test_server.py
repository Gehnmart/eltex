import subprocess
import threading
import sys

def run_client():
    process = subprocess.Popen(["./client"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    
    try:
        # Отправляем команды в терминал
        commands = ["time", "time", "exit"]
        input_data = "\n".join(commands) + "\n"
        process.communicate(input=input_data)

        # Ждем завершения процесса и получаем его вывод
        stdout, stderr = process.communicate()
        print(f"Client output:\n{stdout}")

    except Exception as e:
        print(f"An error occurred in client thread: {e}")
    
    finally:
        process.stdout.close()
        process.stderr.close()

def create_client_threads(num_clients):
    threads = []
    for _ in range(num_clients):
        thread = threading.Thread(target=run_client)
        threads.append(thread)
        thread.start()

    for thread in threads:
        thread.join()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python load_test.py <num_clients>")
        sys.exit(1)

    num_clients = int(sys.argv[1])
    create_client_threads(num_clients)
