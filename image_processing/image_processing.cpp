#include "component.h"
#include "image_processing_ifaces.h"

#include "external_data.h"
#include "image.h"
#include "add.h"

extern "C" __declspec(dllexport) void RegisterInterfaceType(InterfaceTypeMap &interfaceTypeMap)
{
    InterfaceType interfaceType;

    interfaceType.InterfaceId = CLIENT_CIID_EXTERNAL_DATA;
    interfaceType.InterfaceName = TEXT("����");
    interfaceTypeMap[interfaceType.InterfaceId] = interfaceType;

    interfaceType.InterfaceId = CLIENT_CIID_IMAGE;
    interfaceType.InterfaceName = TEXT("ͼ��");
    interfaceTypeMap[interfaceType.InterfaceId] = interfaceType;

    interfaceType.InterfaceId = CLIENT_CIID_IMAGE_ALGORITHM;
    interfaceType.InterfaceName = TEXT("ͼ���㷨");
    interfaceTypeMap[interfaceType.InterfaceId] = interfaceType;
}

extern "C" __declspec(dllexport) void RegisterComponentType(ComponentTypeMap &componentTypeMap)
{
    ComponentType componentType;

    componentType.TypeId = ExternalData::s_ComponentId;
    componentType.TypeName = ExternalData::s_ComponentName;
    componentType.InterfaceId = CLIENT_CCID_EXTERNAL_DATA;
    componentType.Factory = (ComponentFactory)ExternalData::Factory;
    componentTypeMap[componentType.TypeId] = componentType;

    componentType.TypeId = Image::s_ComponentId;
    componentType.TypeName = Image::s_ComponentName;
    componentType.InterfaceId = CLIENT_CCID_IMAGE;
    componentType.Factory = (ComponentFactory)Image::Factory;
    componentTypeMap[componentType.TypeId] = componentType;

    componentType.TypeId = Add::s_ComponentId;
    componentType.TypeName = Add::s_ComponentName;
    componentType.InterfaceId = CLIENT_CCID_ADD;
    componentType.Factory = (ComponentFactory)Add::Factory;
    componentTypeMap[componentType.TypeId] = componentType;
}
