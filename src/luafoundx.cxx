//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/21.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include "luafoundx.hxx"

#include "lua_inc.hxx"
#include <LuaBridge/RefCountedPtr.h>

#include "common.hxx"
#include "timer.hxx"
#include "childprocess.hxx"
#include "sock.hxx"

using luabridge::LuaRef;

struct ixlbx_ref_base {

    ixlbx_ref_base() : m_this_ref(NULL) { }

    virtual void ref() {
        if (this->m_this_ref == NULL)
            this->m_this_ref = new RefCountedPtr<ixlbx_ref_base>(this); }

    virtual void un_ref() {
        if (this->m_this_ref != NULL) {
            RefCountedPtr<ixlbx_ref_base> *t = m_this_ref;
            this->m_this_ref = NULL;
            delete t;
        }
    }

    // KEEP IN MIND ...
    virtual ~ixlbx_ref_base() { }

    private:

        RefCountedPtr<ixlbx_ref_base> *m_this_ref;
};

using BaseRefCountPtr = RefCountedPtr<ixlbx_ref_base>;

struct ixlbx_timer : public ixlbx_ref_base {

    ixlbx_timer(ixc_context *context, LuaRef cb) :
            m_impl(ixut_timer_create(context)), m_ref_cb(cb) { }

    void start(double interval) {
        this->ref();
        ixut_timer_start(this->m_impl, interval, [] (ixut_timer *timer, void *args) {
            ixlbx_timer *self = (ixlbx_timer *) args;
            self->m_ref_cb(RefCountedPtr<ixlbx_timer>(self));
//             ev_timer_stop(self->m_impl->context->evl, self->m_impl->_timer);
            self->un_ref();
        }, (void *) this);
    }

    ~ixlbx_timer() {
        printf("freeing ixlbx_timer ...\n");
        ixut_timer_free(this->m_impl); }

    private:

        ixut_timer *m_impl;
        luabridge::LuaRef m_ref_cb;

};

struct ixlbx_fork {

private:
    luabridge::LuaRef m_ref_cb;

public:
    ixlbx_fork(LuaRef cb) : m_ref_cb(cb), pid(ixut_fork([] (void *args) {
            ixlbx_fork *self = (ixlbx_fork *) args;
            self->m_ref_cb(self); }, (void *) this)) { }

    pid_t pid;

};

struct ixlbx_child_watcher : public ixlbx_ref_base {

    ixlbx_child_watcher(ixc_context *context, LuaRef cb) :
            m_impl(ixut_childw_create(context)), m_ref_cb(cb) { }

    ~ixlbx_child_watcher() {
        ixut_childw_free(this->m_impl); }

    void start(pid_t pid) {
        this->ref();
        ixut_childw_start(this->m_impl, pid, [] (ixut_child_watcher *watcher, void *args) {
            ixlbx_child_watcher *self = (ixlbx_child_watcher *) args;
            self->m_ref_cb(self);
            self->un_ref();
        }, (void *) this);
    }

    ixut_child_exitstatus get_status() {
        return ixut_childw_getstatus(m_impl); }

    private:

        ixut_child_watcher *m_impl;
        luabridge::LuaRef m_ref_cb;

};

struct ixlbx_tcp_base : public ixlbx_ref_base {

    ixlbx_tcp_base(ixc_context *context);

    ixlbx_tcp_base *start(const char *ip, unsigned short port) {
        ixfd_commonsock_tcp_createnbind(m_impl, ip, port);
        ixfd_commonsock_tcp_listen(m_impl);
        this->ref();
        return this;
    }

    ~ixlbx_tcp_base() {
        printf("freeing ixlbx_tcp_base ...\n");
        ixfd_commonsock_free(m_impl); }

    ixlbx_tcp_base *on_read(LuaRef cb) {
        this->m_ref_cbread = LuaRef(cb);
        return this;
    }

    ixlbx_tcp_base *on_accept(LuaRef cb) {
        this->m_ref_cbaccept = LuaRef(cb);
        return this;
    }

    ixlbx_tcp_base *on_close(LuaRef cb) {
        this->m_ref_cbclose = LuaRef(cb);
        return this;
    }

    void close() {
        ixfd_commonsock_unbind(m_impl);
        this->un_ref();
    }

    private:

    ixfd_sock *m_impl;
    luabridge::LuaRef m_ref_cbread;
    luabridge::LuaRef m_ref_cbaccept;
    luabridge::LuaRef m_ref_cbclose;

    friend class ixlbx_socket_context;

};

struct ixlbx_socket_context {

    using LuaRefPtr = RefCountedPtr<ixlbx_socket_context>;

    ixlbx_socket_context(ixfd_conn_ctx *src) : m_impl(src), data(src->sock->context->state) {
        src->data = this;
        this->ref(); }

    ~ixlbx_socket_context() {
        printf("freeing ixlbx_socket_context ...\n");
        ixfd_commonsock_freectx(m_impl); }

    void write(const std::string& content, LuaRef cb) {
        LuaRef *cb_ = new LuaRef(cb);
        ixfd_commonsock_write(m_impl, content.c_str(), content.length(),
            [] (ixfd_conn_ctx *ctx, void *args) {
                LuaRef *cb__ = (LuaRef *) args;
                if (cb__ && !cb__->isNil())
                    (*cb__) (LuaRefPtr((ixlbx_socket_context *) ctx->data));
                delete cb__;
            }, cb_);
    }

    void close() {
        ixfd_commonsock_close(m_impl);
        this->un_ref(); }

    bool is_active() {
        return m_impl->active; }

    ixlbx_tcp_base *sock() { return (ixlbx_tcp_base *) m_impl->sock->data; }

    LuaRef data;

    private:

    virtual void ref() {
        if (this->m_this_ref == NULL)
            this->m_this_ref = new RefCountedPtr<ixlbx_socket_context>(this); }

    virtual void un_ref() {
        if (this->m_this_ref != NULL) {
            delete this->m_this_ref;
            this->m_this_ref = NULL;
        }
    }

    RefCountedPtr<ixlbx_socket_context> *m_this_ref = NULL;
    ixfd_conn_ctx *m_impl;

    friend class ixlbx_tcp_base;

};

ixlbx_tcp_base::ixlbx_tcp_base(ixc_context *context)
    : m_impl(ixfd_commonsock_create(context)), m_ref_cbread(context->state),
      m_ref_cbaccept(context->state), m_ref_cbclose(context->state) {
    m_impl->data = this;

    m_impl->cb_read = [] (ixfd_conn_ctx *ctx, const char *data, size_t len) {
        LuaRef cb = ((ixlbx_tcp_base *) (ctx->sock->data))->m_ref_cbread;
        if (cb && !cb.isNil())
            (cb) (RefCountedPtr<ixlbx_socket_context>((ixlbx_socket_context *) (ctx->data)),
                      data, len);
    };

    m_impl->cb_accept = [] (ixfd_conn_ctx *ctx, void *data) {
        LuaRef cb = ((ixlbx_tcp_base *) (ctx->sock->data))->m_ref_cbaccept;
        if (cb && !cb.isNil())
            (cb) (RefCountedPtr<ixlbx_socket_context>(new ixlbx_socket_context(ctx)));
    };

    m_impl->cb_close = [] (ixfd_conn_ctx *ctx, void *data) {
        ixlbx_socket_context *parent = (ixlbx_socket_context *) ctx->data;
        LuaRef cb = ((ixlbx_tcp_base *) (ctx->sock->data))->m_ref_cbclose;
        if (cb && !cb.isNil())
            (cb) (RefCountedPtr<ixlbx_socket_context>(parent));
        parent->un_ref();
    };
}

void ixlbx_reg_interface(lua_State *state) {
    luabridge::getGlobalNamespace(state).
        beginNamespace("iojxx").
            beginClass<ixlbx_ref_base>("base").
                addConstructor<void (*)(), RefCountedPtr<ixlbx_ref_base>>().
            endClass().
            beginClass<ixlbx_timer>("timer").
                addConstructor<void (*)(ixc_context *, LuaRef), RefCountedPtr<ixlbx_timer>>().
                addFunction("start", &ixlbx_timer::start).
            endClass().
            beginClass<ixlbx_fork>("fork").
                addConstructor<void (*)(LuaRef)>().
                addData("pid", &ixlbx_fork::pid).
            endClass().
            beginClass<ixlbx_child_watcher>("child_watcher").
                addConstructor<void (*)(ixc_context *, LuaRef), RefCountedPtr<ixlbx_child_watcher>>().
                addFunction("start", &ixlbx_child_watcher::start).
                addFunction("get_status", &ixlbx_child_watcher::get_status).
            endClass().
            beginClass<ixlbx_tcp_base>("ixlbx_tcp_base").
                addConstructor<void (*)(ixc_context *), RefCountedPtr<ixlbx_tcp_base>>().
                addFunction("start", &ixlbx_tcp_base::start).
                addFunction("on_read", &ixlbx_tcp_base::on_read).
                addFunction("on_accept", &ixlbx_tcp_base::on_accept).
                addFunction("on_close", &ixlbx_tcp_base::on_close).
                addFunction("close", &ixlbx_tcp_base::close).
            endClass().
            beginClass<ixlbx_socket_context>("ixlbx_socket_context").
                addFunction("write", &ixlbx_socket_context::write).
                addFunction("close", &ixlbx_socket_context::close).
                addFunction("is_active", &ixlbx_socket_context::is_active).
                addFunction("sock", &ixlbx_socket_context::sock).
                addData("data", &ixlbx_socket_context::data).
            endClass().
        endNamespace();
}
