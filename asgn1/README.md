# Assignment 1 - httpserver

This program is able to create a local hosted httpserver. It has the ability to get files,\
put new information into files, and get the header for files. 
I seperated each of these into their own function for modularity and \
utilized parsing to get parts of a buffer that was read into. The buffer 
size I used was the size fo 2048.\
When you use 'PUT' for my implementation, it does not print the response\
like the implementation, but it still has the proper codes, just does\
not print

## Building

To build 'httpserver', run the following.

'''
make httpserver
'''

To format the file, run the following.

'''
make format
'''

To clean files after making split, run the following.

'''
make clean
'''

## Running

To run the httpserver, type the folowing:

'''
./httpserver (port number)
'''

This will start the locally hosted server and you can type the following into a local browser\
to see the server:

'''
localhost:(port number)/(file name)
'''

Going to the server allows you to get the information from the file specified.\
\
To put information into a specified file, you will need to use a seperate console on the same\
machine and type the following

'''
printf "PUT /(file name) HTTP/1.1\r\nContent-Length: (content-length of information)\r\n\r\n(information)"
|nc localhost (port number)
'''

This should help to display the HTTP protocol of what you just put into the file.\
\
For the "HEAD" function, it is similar to that of PUT, do the following.\
'''
printf "HEAD /(file name) HTTP/1.1\r\n\r\n | nc localhost (port number)
'''





