#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>

#include <ruby.h>

VALUE NanoReq = Qnil;

typedef struct nano_req {
    int fd;
    int rcvfd;

} nano_req;

static nano_req *get_nano_req(VALUE self) {
    nano_req* p;
    Data_Get_Struct(self, nano_req, p);
    return p;
}

VALUE nanoreq_close (VALUE self)
{
    nano_req *req = get_nano_req (self);
    nn_close (req->fd);
    printf("shutdown fd\n");
    return Qnil;    
}

static void nanoreq_free (nano_req *self)
{
    //nano_req *req = get_nano_req (self);
    ruby_xfree(self);
}

VALUE nanoreq_alloc (VALUE self)
{
    return Data_Wrap_Struct (self, NULL, nanoreq_free, ALLOC(nano_req));
}

VALUE nanoreq_init (VALUE self, VALUE address)
{
    const char *adr = StringValuePtr(address);
    nano_req *req = get_nano_req (self);
    req->fd = nn_socket (AF_SP, NN_REQ);

    nn_connect (req->fd, adr);
   
    int fd; 
    size_t fd_sz = sizeof(req->rcvfd);
    nn_getsockopt (req->fd, NN_SOL_SOCKET, NN_RCVFD, &fd, &fd_sz);
    req->rcvfd = fd; 
    
    return Qnil;
}

VALUE nanoreq_send (VALUE self, VALUE _msg)
{
    const char *msg = StringValuePtr(_msg);
    nano_req *req = get_nano_req (self);

    nn_send (req->fd, msg, strlen(msg), 0);
    return Qnil;
}

VALUE nanoreq_recv (VALUE self) 
{

    nano_req *req = get_nano_req (self);
    printf("this is my fd: %d\n", req->rcvfd);
    rb_thread_wait_fd(req->rcvfd);

    char *buf = NULL;

    int n = nn_recv (req->fd, &buf, NN_MSG, NN_DONTWAIT);
    char *dst = malloc(sizeof(char)*(n+1));

    memcpy (dst, buf, n);
    dst[n] = '\0';
    nn_freemsg (buf);

    return rb_str_new2 (dst);
}

void Init_nanoreq()
{
    NanoReq = rb_define_class ("NanoReq", rb_cObject);

    rb_define_alloc_func (NanoReq, nanoreq_alloc);
    rb_define_private_method (NanoReq, "initialize", RUBY_METHOD_FUNC(nanoreq_init), 1);
    rb_define_method (NanoReq, "send", RUBY_METHOD_FUNC(nanoreq_send), 1);
    rb_define_method (NanoReq, "recv", RUBY_METHOD_FUNC(nanoreq_recv), 0);
    rb_define_method (NanoReq, "close", RUBY_METHOD_FUNC(nanoreq_close), 0);
}


