#include "tcpserver.hpp"
#include "../../utils/utils.hpp"
#include <unistd.h>
#include <thread>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unordered_map>
#include <string>

using BufferMap = std::unordered_map<int, std::string>;

static void set_nonblocking(int fd)
{
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static void register_event(int kq, int fd, int filter, uint16_t flags)
{
  struct kevent event{};
  EV_SET(&event, fd, filter, flags, 0, 0, nullptr);
  kevent(kq, &event, 1, nullptr, 0, nullptr);
}

static int create_server_socket()
{
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0)
  {
    perror("socket");
    return -1;
  }

  int opt = 1;
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  constexpr uint16_t server_port = 8080;
  addr.sin_port = htons(server_port);
  inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

  if (::bind(server_socket, (sockaddr *)&addr, sizeof(addr)) < 0)
  {
    perror("bind");
    close(server_socket);
    return -1;
  }

  if (listen(server_socket, 128) < 0)
  {
    perror("listen");
    close(server_socket);
    return -1;
  }

  set_nonblocking(server_socket);
  return server_socket;
}

static int create_kqueue(int server_socket)
{
  int kq = kqueue();
  register_event(kq, server_socket, EVFILT_READ, EV_ADD | EV_ENABLE);
  return kq;
}

static std::string handle_command(DatabaseEngine &db_engine, std::string &command)
{
  CommandResult result = db_engine.execute_command(command);
  return serealize_utils::response_buffer(result);
}

void handle_socket(int client_socket, DatabaseEngine &db_engine)
{
  std::cout << "Client connected on thread " << std::this_thread::get_id() << '\n';
  char buffer[1024];
  while (1)
  {
    ssize_t bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytes > 0)
    {
      buffer[bytes] = '\0';
      std::string message(buffer);
      std::string response_message = handle_command(db_engine, message);
      ::send(client_socket, response_message.c_str(), response_message.size(), 0);
    }
    else
    {
      std::cout << "Client disconnected\n";
      break;
    }
  }
  close(client_socket);
}

static void accept_clients(
    int server_socket,
    int kq,
    BufferMap &read_buf,
    BufferMap &write_buf)
{
  while (true)
  {
    int client = accept(server_socket, nullptr, nullptr);
    if (client < 0)
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      perror("accept");
      break;
    }

    set_nonblocking(client);
    read_buf[client] = "";
    write_buf[client] = "";
    register_event(kq, client, EVFILT_READ, EV_ADD | EV_ENABLE);
  }
}

static void handle_read_event(
    int fd,
    int kq,
    DatabaseEngine &db_engine,
    BufferMap &read_buf,
    BufferMap &write_buf)
{
  char chunk[4096];
  while (true)
  {
    ssize_t bytes_read = recv(fd, chunk, sizeof(chunk), 0);
    if (bytes_read > 0)
    {
      read_buf[fd].append(chunk, bytes_read);
    }
    else if (bytes_read == 0)
    {
      close(fd);
      read_buf.erase(fd);
      write_buf.erase(fd);
      return;
    }
    else
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      close(fd);
      read_buf.erase(fd);
      write_buf.erase(fd);
      return;
    }
  }

  if (!read_buf[fd].empty())
  {
    std::string command = read_buf[fd];
    read_buf[fd].clear();
    write_buf[fd] = handle_command(db_engine, command);
    register_event(kq, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE);
  }
}

static void handle_write_event(int fd, int kq, BufferMap &write_buf)
{
  std::string &output = write_buf[fd];
  while (!output.empty())
  {
    ssize_t bytes_sent = send(fd, output.data(), output.size(), 0);
    if (bytes_sent > 0)
    {
      output.erase(0, bytes_sent);
    }
    else
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      close(fd);
      write_buf.erase(fd);
      return;
    }
  }

  if (output.empty())
    register_event(kq, fd, EVFILT_WRITE, EV_DELETE);
}

static void handle_event(
    const struct kevent &event,
    int server_socket,
    int kq,
    DatabaseEngine &db_engine,
    BufferMap &read_buf,
    BufferMap &write_buf)
{
  int fd = (int)event.ident;
  if (fd == server_socket)
  {
    accept_clients(server_socket, kq, read_buf, write_buf);
  }
  else if (event.filter == EVFILT_READ)
  {
    handle_read_event(fd, kq, db_engine, read_buf, write_buf);
  }
  else if (event.filter == EVFILT_WRITE)
  {
    handle_write_event(fd, kq, write_buf);
  }
}

void TcpServer::start()
{
  int server_socket = create_server_socket();
  if (server_socket < 0)
    return;

  int kq = create_kqueue(server_socket);
  BufferMap read_buf;
  BufferMap write_buf;

  while (true)
  {
    struct kevent events[64];
    int event_count = kevent(kq, nullptr, 0, events, 64, nullptr);
    for (int i = 0; i < event_count; ++i)
    {
      handle_event(events[i], server_socket, kq, IServer::db, read_buf, write_buf);
    }
  }
}

void TcpServer::stop() {}