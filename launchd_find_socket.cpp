#include <launch.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <jni.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdarg.h>
#include "launchd_fd.h"

#define IO_EXCEPTION  "java/io/IOException"
#define INETADDRESS   "java/net/InetAddress"
struct IterFinder
{
  bool m_found;
  int  m_fd_result;
};
void iterateFn2(  const launch_data_t elem, const char * key, void * a )
{
  struct IterFinder * pF = (IterFinder*)a;
  launch_data_t listening_fd;
  fprintf( stderr, "Elem2 = %s\n", key );
  if (launch_data_array_get_count( elem )!=1) {
	fprintf(stderr,"launchd check-in: Expected 1 socket from launchd, got %u)\n",
		(unsigned)launch_data_array_get_count(elem));
	return;
      }
  listening_fd=launch_data_array_get_index(elem, 0);
  int fd_result = launch_data_get_fd(listening_fd);
  if( fd_result > 0 )
    {
      pF->m_found = true;
      pF->m_fd_result = fd_result;
    }
  fprintf( stderr, "Got fd %d\n", fd_result );

}

void iterateFn(  const launch_data_t elem, const char * key, void * a )
{
  fprintf( stderr, "Elem = %s\n", key );
  if( strcmp( key, "Sockets") == 0 )
    {
      fprintf( stderr, "sockets = %d\n", launch_data_dict_get_count( elem ));
      launch_data_t listening_fd_array, listening_fd;

      launch_data_dict_iterate( elem, iterateFn2, a );
      listening_fd_array = launch_data_dict_lookup( elem, ":0");
      if (NULL == listening_fd_array) {
	fprintf(stderr,"launchd 2 check-in: No known sockets found to answer requests on!\n");
	return;
      }
    
      if (launch_data_array_get_count(listening_fd_array)!=1) {
	fprintf(stderr,"launchd check-in: Expected 1 socket from launchd, got %u)\n",
		(unsigned)launch_data_array_get_count(listening_fd_array));
	return;
      }
    
      listening_fd=launch_data_array_get_index(listening_fd_array, 0);
      fprintf( stderr, "Got fd %d\n", launch_data_get_fd(listening_fd) );



    }
}
bool launch_find_socket( launch_data_t checkin_response, int * fd )
{
  struct IterFinder finder;
  finder.m_found = false;
  finder.m_fd_result = 0;
  launch_data_dict_iterate( checkin_response, iterateFn, &finder  );
  if( finder.m_found == true )
    {
      *fd = finder.m_fd_result;
      return true;
    }
  return false;
}

