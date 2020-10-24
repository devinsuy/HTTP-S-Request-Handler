#include "RequestHandler.h"

// Initialize and send request, log response to console
// sets req_success = 0 if errors encountered or bad response code
RequestHandler::RequestHandler(const char* host, const char* path, const bool print, const char* verb)  
: verb(verb), path(path), host(host), BUFFER_SIZE(1024), print(print), status_code(-1), resp_size(-1){
    auto start = std::chrono::high_resolution_clock::now();
    if (init() && send_rcv()){
      // Calculate total runtime in milliseconds
      auto end = std::chrono::high_resolution_clock::now();
      total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
      
      req_success = (status_code < 400) ? 1 : 0;
      std::cout << "\nRESPONSE INFO\n-------------\nStatus Code: " << status_code
                << "\nResponse Size: " << resp_size << " bytes"
                << "\nTotal Time: " << total_time.count() << " ms\n\n";
    } 
    else{
        printf("\nHTTPS request to \"%s\" invalid!\n\n", host);
    }
}

// Using the URL, resolves host and sets inet_addr of our socket
int RequestHandler::resolve_host(struct sockaddr_in * saddr){
  struct hostent *h = gethostbyname2(host, AF_INET);
  if(h == NULL)
      return 0;
  // Assign the inet_addr
  memcpy((char *) &saddr->sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  return 1;
}

// Output errors encountered setting up the SSL
// connection with the server
void RequestHandler::print_errs(){
  int error_code;
  printf("FAILED to establish connection\n");
  while ((error_code = ERR_get_error())) {
      const char *str = ERR_error_string(error_code, 0);
      if (!str) return;
      puts(str);
      printf("\n");
      fflush(stdout);
  }
}

// Setup connection 
int RequestHandler::init(){
  // Create base socket object
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
      printf("FAILED to create socket.\n");
      return 0;
  }

  // Initialize and connect socket
  struct sockaddr_in sock_addr;
  memset (&sock_addr, 0, sizeof(sock_addr));
  if (!resolve_host(&sock_addr)){
      printf("FAILED to resolve host. \"%s\"\n", host);
      return 0;
  }
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_port = htons (443); 
  socklen_t socklen = sizeof(sock_addr);
  if (connect(s, (struct sockaddr *)&sock_addr, socklen)){
      printf("FAILED to create socket connection to \"%s\"", host);
      return 0;
  }

  // Prepare connection
  SSL_library_init();
  SSLeay_add_ssl_algorithms();
  SSL_load_error_strings();
  const SSL_METHOD *meth = TLS_client_method();
  SSL_CTX *ctx = SSL_CTX_new (meth);
  ssl = SSL_new (ctx);
  if (!ssl) {
      print_errs();
      return 0;
  }
  // Connect 
  SSL_set_tlsext_host_name(ssl, host);
  sock = SSL_get_fd(ssl);
  SSL_set_fd(ssl, s);
  int err = SSL_connect(ssl);
  if (err <= 0) {
      print_errs();
      fflush(stdout);
      return 0;
  }
  printf ("Established connection to \"%s\" with %s\n\n", host, SSL_get_cipher(ssl));
  return 1;
}

// Create the request header text
char* RequestHandler::generate_req(){
  sprintf(req_head, "%s %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", verb, path, host);
  if (print){
    printf("HTTPS REQUEST\n------------\n");
    puts(req_head);
  }
  return req_head;
}

// Send the HTTPS request to the server
int RequestHandler::send_request(const char *req){
  const int bytes_sent = SSL_write(ssl, req, strlen(req));

  if (bytes_sent < 0) {
      const int error_code = SSL_get_error(ssl, bytes_sent);
      switch (error_code) {
        case SSL_ERROR_WANT_WRITE: return 0;
        case SSL_ERROR_WANT_READ: return 0;
        case SSL_ERROR_ZERO_RETURN:
        case SSL_ERROR_SYSCALL:
        case SSL_ERROR_SSL:
        default: return 0;
      }
  }
  return 1;
}

// Returns first index a delimiter is found, -1 if not found
int RequestHandler::find_delim(const char *arr, const char delim){
  int index = 0;
  char curr_char = arr[index++];
  while (curr_char != '\0') {
    if (curr_char == delim) return index-1;
    curr_char = arr[index++];
  }

  return -1;
}

// Read the response from the server
int RequestHandler::rcv_response(){
  int bytes_read, end_index;
  char buffer[BUFFER_SIZE];
  std::string resp_head, msg, curr_read;

  // Handle the Response Header
  int msg_bytes = 0;
  bool first_msg = true;
  bytes_read = SSL_read(ssl, buffer, BUFFER_SIZE);

  if (bytes_read < 0) {
      const int err_code = SSL_get_error(ssl, bytes_read);
      if (err_code == SSL_ERROR_WANT_READ) return 0;
      if (err_code == SSL_ERROR_WANT_WRITE) return 0;
      if (err_code == SSL_ERROR_ZERO_RETURN || err_code == SSL_ERROR_SYSCALL || err_code == SSL_ERROR_SSL)
      return 0;
  }
  buffer[bytes_read] = '\0';
  msg = std::string(buffer);

  // Locate the first index after the header, store the header msg 
  int msg_begin = msg.find("\r\n\r\n") + 4;
  resp_head = msg.substr(0, msg_begin-4);
  if (print)
    printf("RESPONSE HEADER\n---------------\n%s\n\nRESPONSE MESSAGE\n----------------\n", resp_head.c_str());
  msg = msg.substr(msg_begin);
  bool first_read = true;
  bool chunked = (resp_head.find("chunked") != -1);
  int status_index = resp_head.find("HTTP") + 9;
  status_code = stoi(resp_head.substr(status_index, 3));

  // Continue reading response from server until finished
  int size_end_index, size_bytes;
  size_bytes = resp_size = 0;
  do {
    if (!first_read){
      bytes_read = SSL_read(ssl, buffer, BUFFER_SIZE);
      buffer[bytes_read] = '\0';
      msg = std::string(buffer);
    } else {
      first_read = false;
    }

    // Server is using chunked transfer-encoding, parse the
    // leading HEX message sizes and include in total resp size
    if (chunked){
      if((size_end_index = msg.find("\r\n")) != -1){
        msg = msg.substr(size_end_index + 2);
        size_bytes += (size_end_index + 2);
        // int msg_size = strtol(msg.substr(0, size_end_index).c_str(), 0, 16);
      }
    }
    if (print) std::cout << msg;
    resp_size += msg.length();
  } while (bytes_read > 0);
  // Final message chunk, sum with transfer-encoding sizes
  if (print) std::cout << msg;
  resp_size += msg.length();
  resp_size += size_bytes;

  return 1;     
}

// Send response and recieve request from server
int RequestHandler::send_rcv(){
  if (send_request(generate_req()) != 1) {
      printf("Failed to send HTTPS request to \"%s\"", host);
      return 0;
  }
  // printf("SERVER RESPONSE\n---------------\n");
  if (rcv_response() != 1){
      printf("Failed to receive response from \"%s\"", host);
      return 0;
  }
  return 1;
}
