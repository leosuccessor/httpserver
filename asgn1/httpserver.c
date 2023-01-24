//-----------------------------------------------------------------------------
// Leo DeAnda, ledeanda
// 2022 Fall CSE130 Assignment 1
// httpserver.c
// Main file for web server
//-----------------------------------------------------------------------------

#include "bind.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define buffsize 2048
struct stat st;

void get_request(int accepted_socket, char *pathfile) {
  int file_exist;
  file_exist = open(pathfile, O_RDONLY | O_DIRECTORY);

  // Checks to see if the file actually exists
  if (file_exist == -1) {
    file_exist = open(pathfile, O_RDONLY);
  }

  // Only comes here if the reading permission is not allowed for file
  else {
    write(accepted_socket,
          "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n",
          57);
    close(file_exist);
    return;
  }

  // If the file does not exist, this results in a not found response
  if (file_exist < 0) {
    if (errno == ENOENT) {
      write(accepted_socket,
            "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n",
            56);
      close(file_exist);
      return;
    }
  }

  int perms = access(pathfile, R_OK);
  if (perms < 0) {
    write(accepted_socket,
          "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n",
          57);
    close(file_exist);
    return;
  }
  char buff[buffsize];
  int get_file;
  int bytes_wrote;

  // Gets the size of the file
  fstat(file_exist, &st);

  // Reset the buffer, so it is empty.
  memset(buff, 0, buffsize);
  // file_exist = open(pathfile, O_RDONLY, S_IRWXU);
  snprintf((char *)buff, sizeof(buff),
           "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", st.st_size);
  write(accepted_socket, (char *)buff, strnlen((char *)buff, buffsize));

  // Reads from the file name given, and writes to the socket
  while ((get_file = read(file_exist, buff, buffsize)) > 0) {
    bytes_wrote = 0;

    // All bytes could not have gotten written to socket
    while (bytes_wrote < get_file) {
      bytes_wrote += write(accepted_socket, buff, get_file);
    }
  }
  close(file_exist);
  return;
}

void put_request(int socket_accepted, int cont_length, char *filepath,
                 char *msgchunk) {
  int access_to_file, fd;
  uint8_t buff[buffsize + 1] = {0};

  // This is to check if the file already exists
  // or we have to create the file.
  access_to_file = access(filepath, F_OK);
  // check for direcrtyor

  int file_status = 0;
  fd = open(filepath, O_WRONLY | O_TRUNC);

  // This is to create the file
  if (fd == -1) {
    fd = open(filepath, O_WRONLY | O_TRUNC | O_CREAT);

    // File status 1 means that the file was just created.
    file_status = 1;
  }

  // Checks to see if you have the perms to write to file
  int perms = access(filepath, W_OK);

  // This is to print any of the message body if it was
  // apart of the header.
  int read_bytes = 0;
  int msgchunklen = strnlen(msgchunk, buffsize);
  if (msgchunklen > cont_length) {
    write(fd, msgchunk, cont_length);

  } else if (msgchunklen > 0) {
    write(fd, msgchunk, msgchunklen);
  }

  int byteswrritten = msgchunklen;
  int holder;

  // Loop to read through the rest of the message body and write
  // to the file.
  while ((read_bytes = read(socket_accepted, buff, buffsize - 1)) > 0) {
    byteswrritten += read_bytes;

    // If the bytes written is greater than the content length
    // we have to write the amount of bytes, such that it does
    // not go over the content length
    if (byteswrritten >= cont_length) {
      int wantwrite = cont_length - (byteswrritten - read_bytes);
      holder = write(fd, buff, wantwrite);
      break;
    } else {
      holder = write(fd, buff, read_bytes);
    }

    // So it does not hang on read
    if (read_bytes == cont_length) {
      break;
    }
  }

  // Printing the response to socket after writing everything
  // Putting before, causes tests to fail for some reason
  if (file_status == 0) {
    write(socket_accepted, "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n",
          42);
  } else if (perms == -1) {
    write(socket_accepted,
          "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n",
          57);
  } else {
    write(socket_accepted,
          "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n", 52);
  }
  // Close file
  close(fd);
  return;

  // These are to write the response http to the socket.
}
void head_request(int accepted_socket, char *pathfile) {
  int file_exist;
  file_exist = open(pathfile, O_RDONLY | O_DIRECTORY);

  // Checks to see if the file actually exists
  if (file_exist == -1) {
    file_exist = open(pathfile, O_RDONLY);
  }

  // Only comes here if the reading permission is not allowed for file
  else {
    write(accepted_socket,
          "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n",
          57);
    close(file_exist);
    return;
  }

  // If the file does not exist, this results in a not found response
  if (file_exist < 0) {
    if (errno == ENOENT) {
      write(accepted_socket,
            "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n",
            56);
      close(file_exist);
      return;
    }
  }

  int perms = access(pathfile, R_OK);
  if (perms < 0) {
    write(accepted_socket,
          "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n",
          57);
    close(file_exist);
    return;
  }

  uint8_t buff[buffsize + 1];
  fstat(file_exist, &st);
  snprintf((char *)buff, sizeof(buff),
           "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", st.st_size);
  write(accepted_socket, (char *)buff, strnlen((char *)buff, buffsize));
}

void parse_request(int accepted_socket) {

  // Some the variables needed for the header
  // The sizes of '8' and '19' were listed
  // in the spec as the max size of the
  // variable. It shows 20 for file path to 
  // leave room for '/'
  char req[8] = {0}, filepath[20] = {0}, version[9] = {0},
       buff[buffsize + 1] = {0}, buff_hold_holder[buffsize + 1] = {0};
  char *string_holder;
  char *random_string_holder = buff;

  int req_read = read(accepted_socket, buff, buffsize);
  memcpy(buff_hold_holder, buff, req_read);


  // Have a buffer holder so that I do not mess with the
  // buff itself
  char *buff_hold = buff_hold_holder;

  // Every properly formatted request will contain '\r\n\r\n'
  // so this if statement is to see if it is present in the header.
  // And/or the header is out of range of 2048

  if ((random_string_holder = strstr(buff, "\r\n\r\n")) == NULL) {

    // This is just to read the rest of the request, already
    // know it is a bad request, but have to read rest of request.
    while ((req_read = read(accepted_socket, buff, buffsize) > 0)) {
      req_read = 1;
    }
    write(accepted_socket,
          "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n",
          61);
    return;
  }

  // The next 3 strtok_r get the 3 request lines
  // and stores them in their respective variables
  string_holder = strtok_r(buff_hold, " ", &buff_hold);
  strncpy(req, string_holder, 8);

  string_holder = strtok_r(buff_hold, " ", &buff_hold);
  strncpy(filepath, string_holder, 20);

  string_holder = strtok_r(buff_hold, "\r\n", &buff_hold);
  strncpy(version, string_holder, 9);

  // This if checks to see if the http version is the
  // correct version. It is checked early because if this is
  // incorrect, the entire request is invalid.
  // Check to see if there is a slash in the file name which is not allowed
  if (strncmp(version, "HTTP/1.1", 8) != 0 ||
      (strstr(&filepath[1], "/") != NULL)) {
    write(accepted_socket,
          "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad "
          "Request\n",
          61);
    return;
  }

  // threelen refers to the length of the first 3 variables
  // that were just taken. It starts off at 4 because we take
  // into account '\r\n\r\n'.
  int threelen = 4;
  threelen += strnlen(version, 11);
  threelen += strnlen(req, 8);
  threelen += strnlen(filepath, 20);

  // The following is to find the length of the header that
  // we pulled and not include the message body in this length
  // if it happened to be chunked during the read. It checks
  // for the '\r\n\r\n' and will store the length in the variable
  int headerlen = 0;
  for (uint16_t i = 0; i < strnlen(buff, buffsize); i += 1) {
    if (buff[i] == '\n' && buff[i + 1] == '\r') {
      headerlen = i + 3;
      break;
    }
  }

  // This next chunk is to check if the key: values that
  // were sent are all properly formatted/formed.
  // If they aren't, the process will stop.
  int malf = headerlen - threelen;
  char *malformed = &buff[threelen];
  int tracker = 1;
  for (uint16_t i = 0; i < malf - 1; i += 1) {
    if (malformed[i] == ':' && malformed[i + 1] == ' ') {
      tracker = 0;
    } else if (malformed[i] == '\n' && tracker == 0) {
      tracker = 1;
    } else if (malformed[i] == '\n' && tracker == 1) {
      write(accepted_socket,
            "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad "
            "Request\n",
            61);
      return;
    }
  }

  // This is if the request method was a 'GET'
  if (strncmp(req, "GET", 3) == 0) {
    get_request(accepted_socket, &filepath[1]);
  }

  // This is if the request method was a 'PUT'
  else if (strncmp(req, "PUT", 3) == 0) {

    // Check if a content length was provided.
    random_string_holder = strstr(buff, "Content-Length: ");
    if (random_string_holder == NULL) {
      write(accepted_socket,
            "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad "
            "Request\n",
            61);
      return;
    }

    int cont_length = -1;
    int flag = 0;

    // To find where ContentLength is at in the request
    while ((buff_hold = strtok_r(string_holder, "\r\n", &string_holder)) !=
           NULL) {
      // This is to get the content length number
      if (strncmp(buff_hold, "Content-Length", 14) == 0) {
        char *length_holder = buff_hold;
        if ((strtok_r(length_holder, " ", &length_holder)) != NULL) {
          cont_length = atoi(length_holder);

	  // This is to check if the content length was only a
	  // positive integer. Not a letter or negative
          if (strncmp(length_holder, "0", 1) != 0 && cont_length == 0) {
            flag = 1;
          }
          break;
        }
      }
      // To get rid of the space
      string_holder += 1;
    }

    if (flag == 1 || cont_length < 0) {
      write(accepted_socket,
            "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad "
            "Request\n",
            61);
      return;
    }

    // Use the headerlength found previously to get the message
    // of the request buffer.
    char *msgchunk = &buff[headerlen];
    put_request(accepted_socket, cont_length, &filepath[1], msgchunk);

  }

  // This is if the request method was a 'HEAD'
  else if (strncmp(req, "HEAD", 4) == 0) {
    head_request(accepted_socket, &filepath[1]);
  }

  // If it was not GET, PUT, or HEAD that means
  // it is a function that is not implemented
  else {
    write(accepted_socket,
          "HTTP/1.1 501 Not Implemented\r\nContent-Length: 16\r\n\r\nNot "
          "Implemented\n",
          68);
    return;
  }

  return;
}

int main(int argc, char *argv[]) {

  // To get the port number
  if (argc < 2) {
    fprintf(stderr,
            "httpserver: wrong arguments: ./httpserver port_num\nusage: "
            "./httpserver <port>\n");

    return -1;
  }

  int socket;
  socket = create_listen_socket(atoi(argv[1]));

  // If the port number is too small, under 1024
  if (socket == -1) {
    fprintf(stderr, "httpserveer: invalid port number: %s\n", argv[1]);
    return -1;
  }

  // If the port is already is use
  if (socket == -3) {
    fprintf(stderr, "httpserveer: bind: Permission denied\n");
    return -1;
  }

  // Infinite loop that holds the server up
  for (;;) {
    int accepted_socket = accept(socket, NULL, NULL);
    parse_request(accepted_socket);
    close(accepted_socket);
  }
}
