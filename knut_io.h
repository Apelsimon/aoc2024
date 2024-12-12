#ifndef KNUT_IO_INCLUDE_H
#define KNUT_IO_INCLUDE_H

#include "knut.h"

#include <stdbool.h>
#include <stdint.h>

int knut_io_init();
int knut_io_cleanup();

typedef enum {
    KNUT_IO_ADDR_FAMILY_IPV4 = 0
} knut_io_addr_family_t;

typedef enum {
    KNUT_IO_SOCKET_TYPE_STREAM = 0
} knut_io_socket_type_t;

typedef enum {
    KNUT_IO_PROTOCOL_TYPE_TCP = 0
} knut_io_protocol_type_t;

typedef struct {
    uint32_t value;
} knut_io_ipv4_addr_t;

typedef struct {
    uint32_t value[4];
} knut_io_ipv6_addr_t;

typedef struct {
    union {
        knut_io_ipv4_addr_t ipv4;
        knut_io_ipv6_addr_t ipv6;
    } addr;
    uint16_t port;
} knut_io_endpoint_t;

#define KNUT_IO_MAX_NUM_ADDRINFO 8

typedef struct {
    knut_io_endpoint_t endpoints[KNUT_IO_MAX_NUM_ADDRINFO];
    knut_io_addr_family_t families[KNUT_IO_MAX_NUM_ADDRINFO];
    uint8_t num_entires;
} knut_io_addrinfo_t;

typedef struct {
    knut_io_addr_family_t family;
    knut_io_socket_type_t socket_type;
    knut_io_protocol_type_t protocol;
    const char* node_name;
    const char* service_name;
} knut_io_getadddrinfo_args_t;

int knut_io_getaddrinfo(const knut_io_getadddrinfo_args_t* args, knut_io_addrinfo_t* info);

#ifdef _WIN32

#pragma warning(push)
#pragma warning(disable: 5105)
#include <winsock2.h>
#pragma warning(pop)
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

// TODO: struct not neeeded?
typedef struct {
    SOCKET handle;
} knut_io_socket_t;

#else

typedef struct {
    int handle;
} knut_io_socket_t;

#endif

knut_io_socket_t knut_io_socket(knut_io_addr_family_t family, knut_io_socket_type_t socktype, knut_io_protocol_type_t protocol);
bool knut_io_socket_is_valid(knut_io_socket_t socket);
int knut_io_close(knut_io_socket_t socket);
int knut_io_connect(knut_io_socket_t socket, knut_io_addr_family_t family, const knut_io_endpoint_t* endpoint);
int knut_io_bind(knut_io_socket_t socket, knut_io_addr_family_t family, const knut_io_endpoint_t* endpoint);
int knut_io_listen(knut_io_socket_t socket);
knut_io_socket_t knut_io_accept(knut_io_socket_t socket);
int knut_io_send(knut_io_socket_t socket, const char* buffer, int buffer_size);
int knut_io_recv(knut_io_socket_t socket, char* buffer, int buffer_size);

int knut_io_read_binary(knut_buffer_char_t* buffer, const char* path);

#endif // KNUT_IO_INCLUDE_H

// ==============================================================================
// ==============================================================================
// ==============================================================================
// ==============================================================================
// ==============================================================================
// ==============================================================================

#if defined(KNUT_IO_IMPLEMENTATION) && !defined(KNUT_IO_IMPLEMENTATION_DONE)
#define KNUT_IO_IMPLEMENTATION_DONE

#ifndef KNUT_IMPLEMENTATION_DONE
#error "'knut.h' must be included with KNUT_IMPLEMENTATION before this header can be used" 
#endif
#include "knut.h"

#include <assert.h>
#include <stdio.h>

#ifdef _WIN32

int knut_io_init()
{
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2,2), &wsaData);
}

int knut_io_cleanup()
{
    return WSACleanup();
}

static int to_ai_family(knut_io_addr_family_t family)
{
    switch (family)
    {
    case KNUT_IO_ADDR_FAMILY_IPV4:
    {
        return AF_INET;
    }
    default:
        knut_exit_if(false, "TODO");
        break;
    }

    return -1;
}

static knut_io_addr_family_t to_addr_family(int family)
{
    switch (family)
    {
    case AF_INET:
    {
        return KNUT_IO_ADDR_FAMILY_IPV4;
    }
    default:
        knut_exit_if(false, "TODO");
        break;
    }

    return KNUT_IO_ADDR_FAMILY_IPV4;
}

static int to_ai_socktype(knut_io_socket_type_t type)
{
    switch (type)
    {
    case KNUT_IO_SOCKET_TYPE_STREAM:
    {
        return SOCK_STREAM;
    }
    default:
        knut_exit_if(false, "TODO");
        break;
    }

    return -1;
}

static int to_ai_protocol(knut_io_protocol_type_t type)
{
    switch (type)
    {
    case KNUT_IO_PROTOCOL_TYPE_TCP:
    {
        return IPPROTO_TCP;
    }
    default:
        knut_exit_if(false, "TODO");
        break;
    }

    return -1;
}

int knut_io_getaddrinfo(const knut_io_getadddrinfo_args_t* args, knut_io_addrinfo_t* info)
{
    struct addrinfo* _info = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = to_ai_family(args->family);
    hints.ai_socktype = to_ai_socktype(args->socket_type);
    hints.ai_protocol = to_ai_protocol(args->protocol);
    
    if (args->node_name == NULL)
    {
        hints.ai_flags = AI_PASSIVE;
    }
    
    const int result = getaddrinfo(args->node_name, args->service_name, &hints, &_info);

    if (result != 0)
    {
        freeaddrinfo(_info);
        return result;
    }

    struct addrinfo* current = _info;
    ZeroMemory(info, sizeof(*info));

    for (uint64_t i = 0; current != NULL && i <= KNUT_IO_MAX_NUM_ADDRINFO; ++i)
    {
        knut_exit_if(i >= KNUT_IO_MAX_NUM_ADDRINFO, "TODO");
        const knut_io_addr_family_t family = to_addr_family(current->ai_family);

        switch (family)
        {
        case KNUT_IO_ADDR_FAMILY_IPV4:
        {
            struct sockaddr_in* addr = (struct sockaddr_in*)current->ai_addr;
            info->endpoints[i].addr.ipv4.value = addr->sin_addr.s_addr;
            info->endpoints[i].port = addr->sin_port;
            info->families[i] = family;
            break;
        }
        default:
            knut_exit_if(false, "TODO");
            break;
        }

        ++info->num_entires;
        current = current->ai_next;
    }

    freeaddrinfo(_info);
    return result;
}

knut_io_socket_t knut_io_socket(knut_io_addr_family_t family, knut_io_socket_type_t socktype, knut_io_protocol_type_t protocol)
{
    knut_io_socket_t sock = {
        .handle = socket(to_ai_family(family), to_ai_socktype(socktype), to_ai_protocol(protocol))
    };

    return sock;
}

bool knut_io_socket_is_valid(knut_io_socket_t socket)
{
    return socket.handle != INVALID_SOCKET;
}

int knut_io_close(knut_io_socket_t socket)
{
    return closesocket(socket.handle);
}

typedef union {
    struct sockaddr_in ipv4;
    struct sockaddr_in6 ipv6;
} sockaddr_union_t;

sockaddr_union_t to_sockaddr_union(knut_io_addr_family_t family, const knut_io_endpoint_t* endpoint)
{
    sockaddr_union_t sockaddrs = {0};

    if (family == KNUT_IO_ADDR_FAMILY_IPV4)
    {
        struct sockaddr_in* addr = &sockaddrs.ipv4;
        addr->sin_family = AF_INET;
        addr->sin_addr.s_addr = endpoint->addr.ipv4.value;
        addr->sin_port = endpoint->port;
    }
    else
    {
        knut_exit_if(false, "TODO");
    }

    return sockaddrs;
}

int knut_io_connect(knut_io_socket_t socket, knut_io_addr_family_t family, const knut_io_endpoint_t* endpoint)
{
    sockaddr_union_t sockaddrStorage = to_sockaddr_union(family, endpoint);
    const bool is_ipv4 = family == KNUT_IO_ADDR_FAMILY_IPV4;
    struct sockaddr* sockaddr = is_ipv4 ? (struct sockaddr*)&sockaddrStorage.ipv4 : (struct sockaddr*)&sockaddrStorage.ipv6;
    int namelen = is_ipv4 ? sizeof(sockaddrStorage.ipv4) : sizeof(sockaddrStorage.ipv6);

    return connect(socket.handle, sockaddr, namelen);
}

int knut_io_bind(knut_io_socket_t socket, knut_io_addr_family_t family, const knut_io_endpoint_t* endpoint)
{
    sockaddr_union_t sockaddrStorage = to_sockaddr_union(family, endpoint);
    const bool is_ipv4 = family == KNUT_IO_ADDR_FAMILY_IPV4;
    struct sockaddr* sockaddr = is_ipv4 ? (struct sockaddr*)&sockaddrStorage.ipv4 : (struct sockaddr*)&sockaddrStorage.ipv6;
    int namelen = is_ipv4 ? sizeof(sockaddrStorage.ipv4) : sizeof(sockaddrStorage.ipv6);

    return bind(socket.handle, sockaddr, namelen);
}

int knut_io_listen(knut_io_socket_t socket)
{
    return listen(socket.handle, SOMAXCONN);
}

knut_io_socket_t knut_io_accept(knut_io_socket_t socket)
{
    knut_io_socket_t s = {
        .handle = accept(socket.handle, NULL, NULL)
    };
    return s;
}

int knut_io_send(knut_io_socket_t socket, const char* buffer, int buffer_size)
{
    return send(socket.handle, buffer, buffer_size, 0);
}

int knut_io_recv(knut_io_socket_t socket, char* buffer, int buffer_size)
{
    return recv(socket.handle, buffer, buffer_size, 0);
}

#else
    #error "TODO: impl non win"
#endif // ifdef _WIN32

#define KNUT_IO_DEFAULT_BUFFER_SIZE 1024

int knut_io_read_binary(knut_buffer_char_t* buffer, const char* path)
{
    uint16_t capacity = KNUT_IO_DEFAULT_BUFFER_SIZE;
    buffer->ptr = calloc(capacity, sizeof(*buffer->ptr));
    buffer->size = 0;

    char batch_buffer[KNUT_IO_DEFAULT_BUFFER_SIZE] = {0};
    uint64_t write_pos = 0;
    int result = 0;

    FILE* file_handle = fopen(path, "rb");

    if (file_handle == NULL)
    {
        result = -1;
        goto done;
    }

    uint64_t bytes_read;
    while ((bytes_read = fread(batch_buffer, sizeof(batch_buffer[0]), KNUT_IO_DEFAULT_BUFFER_SIZE, 
        file_handle)) > 0)
    {
        while (capacity - write_pos < bytes_read)
        {
            KNUT_ASSERT(capacity <= (UINT64_MAX / 2), "[knut_io_read_binary] Capacity overflow");
            capacity *= 2;
            buffer->ptr = realloc(buffer->ptr, capacity * sizeof(*buffer->ptr));
        }

        memcpy(buffer->ptr + write_pos, batch_buffer, bytes_read);
        write_pos += bytes_read;
        buffer->size += bytes_read;
    }

    if (!feof(file_handle))
    {
        result = -1;
        goto done;
    }

    buffer->ptr = realloc(buffer->ptr, buffer->size * sizeof(*buffer->ptr));

done:
    if (result != 0)
    {
        knut_buffer_char_free(buffer);
    }

    fclose(file_handle);
    return result;
}

#endif // KNUT_IO_IMPLEMENTATION