#ifndef WLROOTS_HEADERS_HPP
#define WLROOTS_HEADERS_HPP

#define WLR_USE_UNSTABLE

extern "C"
{
        #include <wlr/backend.h>
        #include <wlr/render/allocator.h>
        #include <wlr/render/wlr_renderer.h>
        #include <wlr/types/wlr_cursor.h>
        #include <wlr/types/wlr_compositor.h>
        #include <wlr/types/wlr_output_layout.h>
        #include <wlr/types/wlr_xdg_shell.h>
        #include <wlr/util/log.h>
        #include <wayland-server-core.h>
}

#include "wlr_scene_wrapper.hpp"

#endif
