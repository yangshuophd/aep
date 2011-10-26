/**
* @file
* @brief 匀加速直线运动类cpp文件.
* @author ruoxi
*
* 实现了匀加速直线运动类.
*/

#include "acceleration_linear_motion.h"

AccelerationLinearMotion::AccelerationLinearMotion()
{
}

AccelerationLinearMotion::~AccelerationLinearMotion()
{
}

RC AccelerationLinearMotion::GetInterface(UINT32 iid, void **iface)
{
    if (CIID_ICOMPONENT == iid)
    {
        *iface = static_cast<IComponent *>(this);
    }
    else if (CLIENT_CIID_MOTION == iid)
    {
        *iface = static_cast<IMotion *>(this);
    }
    else
    {
        *iface = 0;
        return RC::COMPONENT_GETINTERFACE_ERROR;
    }
    return OK;
}

RC AccelerationLinearMotion::GetAttribute(UINT32 aid, void **attr)
{
    if (ATTR_VELOCITY == aid)
    {
        *attr = m_Velocity;
    }
    else if (ATTR_ACCELERATION == aid)
    {
        *attr = m_Acceleration;
    }
    else
    {
        *attr = 0;
        return RC::COMPONENT_GETATTRIBUTE_ERROR;
    }
    return OK;
}

RC AccelerationLinearMotion::SetAttribute(UINT32 aid, void *attr)
{
    if (ATTR_VELOCITY == aid)
    {
        m_Velocity = static_cast<Vector *>(attr);
    }
    else if (ATTR_ACCELERATION == aid)
    {
        m_Acceleration = static_cast<Vector *>(attr);
    }
    else
    {
        return RC::COMPONENT_SETATTRIBUTE_ERROR;
    }
    return OK;
}

RC AccelerationLinearMotion::GetCurrentVelocity(Vector &velocity)
{
    velocity = (*m_Velocity);
    return OK;
}

RC AccelerationLinearMotion::Move(Vector &coordinate, double time)
{
    coordinate += ((*m_Velocity) * time + (*m_Acceleration) * time * time * 0.5);
    (*m_Velocity) += ((*m_Acceleration) * time);
    return OK;
}

LPCWSTR AccelerationLinearMotionTypeName = TEXT("Acceleration linear motion");

UINT32 AccelerationLinearMotionAttributeList[] =
{
    #undef DEFINE_ATTR
    /** @brief 重定义匀加速直线运动属性的外包宏DEFINE_ATTR. */
    #define DEFINE_ATTR(a, msg) AccelerationLinearMotion::a,
    /** @brief 用来开启允许包含acceleration_linear_motion_attrs.h头文件的开关. */
    #define __USE_ACCELERATION_LINEAR_MOTION_ATTRS__
    #include "acceleration_linear_motion_attrs.h"
};

AccelerationLinearMotion *AccelerationLinearMotionFactory()
{
    return new AccelerationLinearMotion();
}
