import random
import socket
import threading
from queue import Queue

HOST = "127.0.0.1"
PORT = 8080
WORKER_COUNT = 20


class WorkerThread(threading.Thread):
  def __init__(self, worker_id):
    super().__init__(daemon=True)
    self.worker_id = worker_id
    self.task_queue = Queue()
    self.sock = None
    self._connect()

  def _connect(self):
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    self.sock.settimeout(5)
    self.sock.connect((HOST, PORT))

  def _execute(self, command):
    try:
      self.sock.sendall(command.encode("utf-8"))
      response = self.sock.recv(4096)
      return response.decode("utf-8")
    except (socket.timeout, OSError):
      try:
        self.sock.close()
      except OSError:
        pass
      self._connect()
      self.sock.sendall(command.encode("utf-8"))
      response = self.sock.recv(4096)
      return response.decode("utf-8")

  def run(self):
    while True:
      task = self.task_queue.get()
      if task is None:
        self.task_queue.task_done()
        break

      command, result_queue = task
      try:
        result = self._execute(command)
      except Exception as exc:
        result = f"ERROR: {exc}"
      result_queue.put(result)
      self.task_queue.task_done()


class ConnectionPool:
  def __init__(self, worker_count=WORKER_COUNT):
    self.workers = [WorkerThread(i) for i in range(worker_count)]
    for worker in self.workers:
      worker.start()

  def submit(self, command):
    worker = random.choice(self.workers)
    result_queue = Queue()
    worker.task_queue.put((command, result_queue))
    return result_queue.get()

  def shutdown(self):
    for worker in self.workers:
      worker.task_queue.put(None)

    for worker in self.workers:
      worker.task_queue.join()
      worker.join(timeout=1)


if __name__ == "__main__":
  pool = ConnectionPool()
  print("Connection pool is open. Type commands; enter 'quit' to exit.")

  while True:
      try:
          command = input("redis> ").strip()
      except EOFError:
          print()
          break

      if not command:
          continue

      if command.lower() in {"quit", "exit"}:
          break

      response = pool.submit(command)
      print(f"Response: {response}")

  pool.shutdown()
