#pragma once 

#include "vulkan/vulkan.h"

namespace Rendering
{
    namespace Vulkan
    {
        class Instance
        {   
            public:
                Instance();
                ~Instance();

                Instance(const Instance& instance);
                Instance& operator=(const Instance&) = delete;

                VkInstance Get() const;

            private:
                VkInstance Handle = VK_NULL_HANDLE;
        };
    }
}