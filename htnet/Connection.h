//
// Connection.h
//
// Connection: This class forms a easy to use interface to the berkeley
//             tcp socket library. All the calls are basically the same, 
//             but the parameters do not have any stray _addr or _in
//             mixed in...
//
// Part of the ht://Dig package   <http://www.htdig.org/>
// Copyright (c) 1999 The ht://Dig Group
// For copyright details, see the file COPYING in your distribution
// or the GNU Public License version 2 or later 
// <http://www.gnu.org/copyleft/gpl.html>
//
// $Id: Connection.h,v 1.3 1999/10/07 04:42:23 ghutchis Exp $
//

#ifndef _Connection_h_
#define	_Connection_h_

#include "Object.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

class String;

class Connection : public Object
{
public:
    // Constructors & Destructors
    Connection();
    Connection(int socket);
    ~Connection();

    // (De)initialization
    int				open(int priv = 0);
    int				close();
    int				ndelay();
    int				nondelay();
    int                         timeout(int value);

    // Port stuff
    int				assign_port(int port = 0);
    int				assign_port(char *service);
    int				get_port();
    inline int			is_privileged();

    // Host stuff
    int				assign_server(const String& name);
    int				assign_server(unsigned int addr = INADDR_ANY);
    char				*get_server()		{return server_name;}

    // Connection establishment
    int				connect(int allow_EINTR = 0);
    Connection			*accept(int priv = 0);
    Connection			*accept_privileged();

    // Registration things
    int				bind();
    int				listen(int n = 5);

    // IO
    String		*read_line(String &, char *terminator = "\n");
    char		*read_line(char *buffer, int maxlength, char *terminator = "\n");
    String		*read_line(char *terminator = "\n");
    virtual int		read_char();
    int			write_line(char *buffer, char *eol = "\n");
    
    int			write(char *buffer, int maxlength = -1);
    int			read(char *buffer, int maxlength);
    
    int			read_partial(char *buffer, int maxlength);
    int			write_partial(char *buffer, int maxlength);
    void		stop_io()		{need_io_stop = 1;}

    // Access to socket number
    char				*socket_as_string();
    int				get_socket()		{return sock;}
    int				isopen()		{return sock >= 0;}
    int				isconnected()		{return connected;}

    // Access to info about remote socket
    char				*get_peerip();
    char				*get_peername();

    // A method to re-initialize the buffer
    virtual void        flush();

private:
    //
    // For buffered IO we will need a buffer
    //
    enum {BUFFER_SIZE = 8192};
    char			buffer[BUFFER_SIZE];
    int			pos, pos_max;

    int				sock;
    struct sockaddr_in		server;
    int				connected;
    char			*peer;
    char			*server_name;
    int				need_io_stop;
    int                         timeout_value;
};


//*************************************************************************
// inline int Connection::is_privileged()
// PURPOSE:
//   Return whether the port is priveleged or not.
//
inline int Connection::is_privileged()
{
    return server.sin_port < 1023;
}


//
// Get arround the lack of gethostip() library call...  There is a gethostname()
// call but we want the IP address, not the name!
// The call will put the ASCII string representing the IP address in the supplied
// buffer and it will also return the 4 byte unsigned long equivalent of it.
// The ip buffer can be null...
//
unsigned int gethostip(char *ip = 0, int length = 0);

#endif
