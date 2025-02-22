#include "pch.h"

#include "exception.h"
#include "memoryarea.h"

using namespace XKLib;

auto MemoryArea::ProtectionFlags::ToOwn(const mapf_t flags) -> mapf_t
{
#ifdef WINDOWS
    mapf_t own_flags;

    switch (flags)
    {
        case PAGE_EXECUTE:
        {
            own_flags = MemoryArea::ProtectionFlags::EXECUTE;
            break;
        }
        case PAGE_EXECUTE_READ:
        {
            own_flags = MemoryArea::ProtectionFlags::EXECUTE
                        | MemoryArea::ProtectionFlags::READ;
            break;
        }
        case PAGE_EXECUTE_READWRITE:
        {
            own_flags = MemoryArea::ProtectionFlags::EXECUTE
                        | MemoryArea::ProtectionFlags::READ
                        | MemoryArea::ProtectionFlags::WRITE;
            break;
        }
        case PAGE_READONLY:
        {
            own_flags = MemoryArea::ProtectionFlags::READ;
            break;
        }
        case PAGE_READWRITE:
        {
            own_flags = MemoryArea::ProtectionFlags::READ
                        | MemoryArea::ProtectionFlags::WRITE;
            break;
        }
        case PAGE_EXECUTE_WRITECOPY:
        {
            own_flags = MemoryArea::ProtectionFlags::EXECUTE
                        | MemoryArea::ProtectionFlags::WRITE;
            break;
        }
        case PAGE_WRITECOPY:
        {
            own_flags = MemoryArea::ProtectionFlags::WRITE;
            break;
        }
        default:
        {
            own_flags = MemoryArea::ProtectionFlags::NONE;
            break;
        }
    }

    return own_flags;

#else
    return flags
           & (MemoryArea::ProtectionFlags::EXECUTE
              | MemoryArea::ProtectionFlags::READ
              | MemoryArea::ProtectionFlags::WRITE);
#endif
}

auto MemoryArea::ProtectionFlags::ToOS(const mapf_t flags) -> mapf_t
{
#ifdef WINDOWS
    int os_flags;

    switch (flags)
    {
        case MemoryArea::ProtectionFlags::EXECUTE:
        {
            os_flags = PAGE_EXECUTE;
            break;
        }
        case MemoryArea::ProtectionFlags::EXECUTE
          | MemoryArea::ProtectionFlags::READ:
        {
            os_flags = PAGE_EXECUTE_READ;
            break;
        }
        case MemoryArea::ProtectionFlags::EXECUTE
          | MemoryArea::ProtectionFlags::READ
          | MemoryArea::ProtectionFlags::WRITE:
        {
            os_flags = PAGE_EXECUTE_READWRITE;
            break;
        }
        case MemoryArea::ProtectionFlags::READ:
        {
            os_flags = PAGE_READONLY;
            break;
        }
        case MemoryArea::ProtectionFlags::READ
          | MemoryArea::ProtectionFlags::WRITE:
        {
            os_flags = PAGE_READWRITE;
            break;
        }
        case MemoryArea::ProtectionFlags::EXECUTE
          | MemoryArea::ProtectionFlags::WRITE:
        {
            os_flags = PAGE_EXECUTE_WRITECOPY;
            break;
        }
        case MemoryArea::ProtectionFlags::WRITE:
        {
            os_flags = PAGE_WRITECOPY;
            break;
        }
        default:
        {
            os_flags = 0;
            break;
        }
    }

    return view_as<mapf_t>(os_flags);
#else
    return flags;
#endif
}

auto MemoryArea::operator==(const MemoryArea& area) const -> bool
{
    return begin() == area.begin() and end() == area.end();
}

auto XKLib::MemoryArea::name() const -> const std::string&
{
    return _name;
}

auto MemoryArea::setAddress(const ptr_t address) -> void
{
    _address = address;
}

auto MemoryArea::setSize(const std::size_t size) -> void
{
    _size = size;
}

auto XKLib::MemoryArea::setName(const std::string& name) -> void
{
    _name = name;
}
