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

using luabridge::LuaRef;

struct ixlbx_ref_base {

    ixlbx_ref_base() : m_this_ref(NULL) { }

    void ref() {
        if (this->m_this_ref == NULL)
            this->m_this_ref = new RefCountedPtr<ixlbx_ref_base>(this); }

    void un_ref() {
        if (this->m_this_ref != NULL);
            delete this->m_this_ref; }

    // KEEP IN MIND ...
    virtual ~ixlbx_ref_base() { }

    private:

        RefCountedPtr<ixlbx_ref_base> *m_this_ref;
};

struct ixlbx_timer : public ixlbx_ref_base {

    ixlbx_timer(ixc_context *context, LuaRef cb) :
            m_impl(ixut_timer_create(context)), m_ref_cb(cb) { }

    void start(double interval) {
        this->ref();
        ixut_timer_start(this->m_impl, interval, [] (ixut_timer *timer, void *args) {
            ixlbx_timer *self = (ixlbx_timer *) args;
            self->m_ref_cb(self);
            self->un_ref();
        }, (void *) this);
    }

    ~ixlbx_timer() {
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
        endNamespace();
}
