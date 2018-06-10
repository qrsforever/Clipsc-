/***************************************************************************
 *  Instance.cpp - Class Instance Impl
 *
 *  Created: 2018-06-09 08:46:47
 *
 *  Copyright QRS
 ****************************************************************************/

#include "Instance.h"

#include "Environment.h"

extern "C" {
#include "clips.h"
};

namespace CLIPS {

Instance::Instance(Environment &environment, void *cobj)
#ifndef DEBUG_MEMORY
    : ClipsObject(cobj)
#else
    : ClipsObject("Instance", cobj)
#endif
    , m_environment(environment)
{
    if (m_cobj)
        EnvIncrementInstanceCount(m_environment.cobj(), m_cobj);
}

Instance::pointer Instance::create(Environment &environment, void *obj)
{
    return Instance::pointer(new Instance(environment, obj));
}

Instance::~Instance()
{
    if (m_cobj)
        EnvDecrementInstanceCount(m_environment.cobj(), m_cobj);

    /* Ugly API */
    if (!refcount()) {
        if (TRUE == EnvUnmakeInstance(m_environment.cobj(), m_cobj)) {
            printf("#################true##############\n");
            m_cobj = 0;
        } else
            printf("#################false#############\n");
    }
}

std::string Instance::name()
{
    if (m_cobj)
        return EnvGetInstanceName(m_environment.cobj(), m_cobj);
    return std::string();
}

std::string Instance::formatted()
{
    char format[1025];
    if (m_cobj) {
        EnvGetInstancePPForm(m_environment.cobj(), format, 1024, m_cobj);
        if ('\0' != format[0])
            return format;
    }
    return std::string();
}

Instance::pointer Instance::next()
{
    void *nxt;
    if (m_cobj) {
        nxt = EnvGetNextInstance(m_environment.cobj(), m_cobj);
        if (nxt)
            return Instance::create(m_environment, nxt);
    }
    return Instance::pointer();
}

Values Instance::send(const std::string &msg, const std::string &args)
{
    if (!m_cobj)
        return Values();

    DATA_OBJECT indata;
    DATA_OBJECT outdata;

    SetType(indata, INSTANCE_ADDRESS);
    SetValue(indata, m_cobj);

    EnvSend(m_environment.cobj(), &indata, msg.c_str(), args.c_str(), &outdata);
    return data_object_to_values(&outdata);
}

unsigned int Instance::refcount() const
{
    if (!m_cobj) return 0;

    INSTANCE_TYPE *ins = (INSTANCE_TYPE*)m_cobj;
    return ins->header.busyCount;
}

#ifdef DEBUG_OBJECT
void Instance::debug()
{

}
#endif

} /* namespace */
