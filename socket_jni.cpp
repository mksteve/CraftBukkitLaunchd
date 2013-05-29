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
#include "org_bukkit_craftbukkit_v1_5_R3_util_LaunchdSocketImpl.h"
#include "org_bukkit_craftbukkit_v1_5_R3_util_LaunchdServerSocket.h"


#define IO_EXCEPTION  "java/io/IOException"
#define INETADDRESS   "java/net/InetAddress"

void Log( JNIEnv * e, jobject & logger, jmethodID & channel, const char * format, ... );


JNIEXPORT void JNICALL  Java_org_bukkit_craftbukkit_v1_15_1R3_util_LaunchdServerSocket_blessLaunchdFileId(JNIEnv * e, jclass cl, jobject fd)
{
  int res = launchd_display_fd();
  int hServerSocket = res;
  if( res >= 0 )
    {
      jclass jc = e->GetObjectClass (fd);
      jfieldID id = e->GetFieldID (jc, "fd", "I");
      e->SetIntField(fd, id, hServerSocket);
    }
  else
    {
      jclass io_exception = e->FindClass( IO_EXCEPTION );
      e->ThrowNew( io_exception, "Failed to find fd" );
    }
}

void JNICALL  Java_org_bukkit_craftbukkit_v1_15_1R3_util_LaunchdSocketImpl_sendUrgentData(JNIEnv * e, jobject jo, jint data)
{
  jclass io_exception = e->FindClass( IO_EXCEPTION );
  jclass jc =   e->GetObjectClass (jo);
  jfieldID id = e->GetFieldID (jc, "fd", "Ljava/io/FileDescriptor;");
  jobject fd  = e->GetObjectField( jo,  id );
  jfieldID id_fd = e->GetFieldID( jc, "fd", "I" );
  jint fd_sock = e->GetIntField( fd, id_fd );

  char buffer = data;
  int written = write( fd_sock, &buffer, 1 );
  if( written > 0 )
    {
    }
  else
    {
      fprintf( stderr, "Failed to write %d, %d\n", written, errno );
      e->ThrowNew( io_exception, "Failed to write" );
      return;
    }
}
void JNICALL  Java_org_bukkit_craftbukkit_v1_15_1R3_util_LaunchdSocketImpl_close (JNIEnv * e, jobject jo)
{
  jclass io_exception = e->FindClass( IO_EXCEPTION );
  jclass jc =   e->GetObjectClass (jo);
  jfieldID id = e->GetFieldID (jc, "fd", "Ljava/io/FileDescriptor;");
  jobject fd  = e->GetObjectField( jo,  id );
  jclass fdc = e->GetObjectClass( fd );
  jfieldID id_fd = e->GetFieldID( fdc, "fd", "I" );
  jint fd_sock = e->GetIntField( fd, id_fd );

  if( fd_sock >= 0 )
    {
      close( fd_sock );
      e->SetIntField( fd, id_fd, -1 );
    }
  else
    {
      e->ThrowNew( io_exception, "Closing invalid fd" );
    }

}

jint JNICALL Java_org_bukkit_craftbukkit_v1_15_1R3_util_LaunchdSocketImpl_available(JNIEnv * e, jobject jo)
{
  jclass io_exception = e->FindClass( IO_EXCEPTION );
  jclass jc =   e->GetObjectClass (jo);
  jfieldID id = e->GetFieldID (jc, "fd", "Ljava/io/FileDescriptor;");
  jobject fd  = e->GetObjectField( jo,  id );
  jfieldID id_fd = e->GetFieldID( jc, "fd", "I" );
  jint fd_sock = e->GetIntField( fd, id_fd );
  size_t nbytes;
  if( ioctl(fd_sock, FIONREAD, (char*)&nbytes) < 0 )
    {
      e->ThrowNew( io_exception, "available failed" );
    }
  return nbytes;
}
void Log( JNIEnv * e, jobject & logger, jmethodID & channel, const char * format, ... )
{
  va_list argptr;
  va_start(argptr, format);
  char buffer[8000];
  vsprintf( buffer, format, argptr );
  va_end( argptr );
  jstring str = e->NewStringUTF( buffer );
  e->CallVoidMethod( logger, channel, str );

}
#define SOCK_MAXADDRLEN 255
void JNICALL Java_org_bukkit_craftbukkit_v1_15_1R3_util_LaunchdSocketImpl_accept (JNIEnv * e, jobject jo, jobject si)
{
  fprintf(stderr, "In accept" );
  jclass clogger = e->FindClass( "java/util/logging/Logger" );
  fprintf(stderr, "got logger" );
  jmethodID loggerID = e->GetStaticMethodID( clogger,"getLogger", "(Ljava/lang/String;)Ljava/util/logging/Logger;" );
  fprintf(stderr, "got method %p\n", loggerID );
  jstring name = e->NewStringUTF( "Minecraft" );
  fprintf(stderr, "got name %p\n", name );
  jobject jlogger = e->CallStaticObjectMethod( clogger, loggerID, name );
  fprintf(stderr, "got logger %p\n", jlogger );
  jmethodID warn = e->GetMethodID( clogger, "warning", "(Ljava/lang/String;)V" );
  fprintf(stderr, "got warning %p\n", warn );

  Log( e, jlogger, warn, "accept called\n" );
  jclass io_exception = e->FindClass( IO_EXCEPTION );
  Log( e, jlogger, warn, "xaccept io_exception = %p\n", io_exception );
  jclass jc =   e->GetObjectClass (jo);
  Log( e, jlogger, warn, "accept jc = %p\n", jc );
  jfieldID id = e->GetFieldID (jc, "fd", "Ljava/io/FileDescriptor;");
  Log( e, jlogger, warn,"accept id = %p\n", id );
  jobject fd  = e->GetObjectField( jo,  id );
  Log( e, jlogger, warn, "accept fd = %p\n", fd );
  jclass fdc = e->GetObjectClass( fd );
  Log( e, jlogger, warn, "accept fdc = %p\n", fdc );
  jfieldID id_fd = e->GetFieldID( fdc, "fd", "I" );
  Log( e, jlogger, warn, "accept id_fd = %p\n", id_fd );
  jint fd_sock = e->GetIntField( fd, id_fd );
  Log( e, jlogger, warn,"accept fd_sock = %d\n", fd_sock );

  char sockaddr_buffer[ SOCK_MAXADDRLEN];
  sockaddr *addr = (sockaddr*)sockaddr_buffer;
  socklen_t len = sizeof( sockaddr_buffer );
  int res;
  if( (res = accept( fd_sock, addr, &len )) >= 0 )
    {
      jclass sic = e->GetObjectClass( si );
      jfieldID fdidsi = e->GetFieldID( sic, "fd","Ljava/io/FileDescriptor;");
      jobject sifd = e->GetObjectField( si, fdidsi );

      e->SetIntField( sifd, id_fd, res );


      // Now fill out inet structure
      jint addr_port;
      jbyteArray addr_bytes;

      if (addr->sa_family == AF_INET)
	{
	  sockaddr_in * pin = (sockaddr_in*)addr;
	  sockaddr_in6 * pin2 = (sockaddr_in6*)addr;
	  addr_bytes = e->NewByteArray(sizeof( pin->sin_addr ) );
	  Log( e, jlogger, warn,"accept addr size  = %d\n", sizeof( pin2->sin6_addr)  );
	  e->SetByteArrayRegion( addr_bytes, 0,sizeof(pin->sin_addr),  (jbyte*)&pin->sin_addr  );
	  addr_port = ntohs (pin->sin_port);
	}
      else if (addr->sa_family == AF_INET6)
	{
	  sockaddr_in6 * pin = (sockaddr_in6*)addr;
	  addr_bytes = e->NewByteArray(sizeof( pin->sin6_addr ) );
	  Log( e, jlogger, warn,"accept addr size  = %d\n", sizeof( pin->sin6_addr)  );
	  e->SetByteArrayRegion( addr_bytes, 0,sizeof(pin->sin6_addr),  (jbyte*)&pin->sin6_addr  );
	  addr_port = ntohs (pin->sin6_port);
	}
      else
	{
	  e->ThrowNew( io_exception, "accept - invalid addr type" );
	}

      {
	jclass ina = e->FindClass( INETADDRESS );
	jmethodID getByAddress = e->GetStaticMethodID( ina,  "getByAddress", "([B)Ljava/net/InetAddress;" );
	jobject inet_addr = e->CallStaticObjectMethod( ina, getByAddress, addr_bytes );
	jfieldID si_addr_id = e->GetFieldID( sic, "address", "Ljava/net/InetAddress;" );
        Log( e, jlogger, warn, "accept si_addr_id = %p\n", si_addr_id );
	e->SetObjectField( si, si_addr_id, inet_addr );
	jfieldID si_port = e->GetFieldID( sic, "port", "I" );
	e->SetIntField( si, si_port, addr_port );
      }
    }
  else
    {
      e->ThrowNew( io_exception, "Accept failed" );
    }

}

JNIEXPORT void JNICALL Java_org_bukkit_craftbukkit_v1_15_1R3_util_LaunchdSocketImpl_listen
  (JNIEnv *e , jobject, jint)
{
  //  jclass io_exception = e->FindClass( IO_EXCEPTION );
  //  e->ThrowNew( io_exception, "listen not implemented" );


}

JNIEXPORT jint JNICALL Java_org_bukkit_craftbukkit_v1_15_1R3_util_LaunchdSocketImpl_read__
  (JNIEnv * e, jobject jo)
{
  jclass io_exception = e->FindClass( IO_EXCEPTION );
  jclass jc =   e->GetObjectClass (jo);
  jfieldID id = e->GetFieldID (jc, "fd", "Ljava/io/FileDescriptor;");
  jobject fd  = e->GetObjectField( jo,  id );
  jclass fdc = e->GetObjectClass( fd );
  jfieldID id_fd = e->GetFieldID( fdc, "fd", "I" );
  jint fd_sock = e->GetIntField( fd, id_fd );
  //  fprintf( stderr, "read fd_sock = %d\n", fd_sock );

  unsigned char buffer[1];
  int rd_size = read( fd_sock, buffer, 1 );
  if( rd_size > 0 )
    {
      return buffer[0];
    }
  return -1;
}
JNIEXPORT jint JNICALL Java_org_bukkit_craftbukkit_v1_15_1R3_util_LaunchdSocketImpl_read___3BII
  (JNIEnv * e, jobject jo , jbyteArray jba , jint offset, jint count)
{
  jclass io_exception = e->FindClass( IO_EXCEPTION );
  jclass jc =   e->GetObjectClass (jo);
  jfieldID id = e->GetFieldID (jc, "fd", "Ljava/io/FileDescriptor;");
  jobject fd  = e->GetObjectField( jo,  id );
  jclass fdc = e->GetObjectClass( fd );
  jfieldID id_fd = e->GetFieldID( fdc, "fd", "I" );
  jint fd_sock = e->GetIntField( fd, id_fd );
  //  fprintf( stderr, "read fd_sock = %d\n", fd_sock );

  try {
    jbyte* bytes = new jbyte[ count ];
    int cnt = read( fd_sock, bytes, count );
    e->SetByteArrayRegion( jba, offset, count, bytes );
    delete[] bytes;
    return cnt;
  }
  catch(...)
    {
      e->ThrowNew( io_exception, "read - no memory" );

    }


}
JNIEXPORT void JNICALL Java_org_bukkit_craftbukkit_v1_15_1R3_util_LaunchdSocketImpl_write__I  (JNIEnv *e, jobject jo, jint dta )
{
  jclass io_exception = e->FindClass( IO_EXCEPTION );
  jclass jc =   e->GetObjectClass (jo);
  jfieldID id = e->GetFieldID (jc, "fd", "Ljava/io/FileDescriptor;");
  jobject fd  = e->GetObjectField( jo,  id );
  jclass fdc = e->GetObjectClass( fd );
  jfieldID id_fd = e->GetFieldID( fdc, "fd", "I" );
  jint fd_sock = e->GetIntField( fd, id_fd );
  //  fprintf( stderr, "write fd_sock = %d\n", fd_sock );
  
  char bytes[1];
  bytes[0] = dta;
  int cnt = write( fd_sock, bytes, 1 );


}

JNIEXPORT void JNICALL Java_org_bukkit_craftbukkit_v1_15_1R3_util_LaunchdSocketImpl_write___3BII
  (JNIEnv *e, jobject jo, jbyteArray jba, jint offset, jint count )
{
  jclass io_exception = e->FindClass( IO_EXCEPTION );
  jclass jc =   e->GetObjectClass (jo);
  jfieldID id = e->GetFieldID (jc, "fd", "Ljava/io/FileDescriptor;");
  jobject fd  = e->GetObjectField( jo,  id );
  jclass fdc = e->GetObjectClass( fd );
  jfieldID id_fd = e->GetFieldID( fdc, "fd", "I" );
  jint fd_sock = e->GetIntField( fd, id_fd );
  //  fprintf( stderr, "write fd_sock = %d\n", fd_sock );

  try {
    jbyte* bytes = new jbyte[ count ];
    e->GetByteArrayRegion( jba, offset, count, bytes );
    int cnt = write( fd_sock, bytes, count );

    delete[] bytes;
  }
  catch(...)
    {
      e->ThrowNew( io_exception, "read - no memory" );

    }


}
