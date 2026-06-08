#ifndef COMPOSITOR_HPP
#define COMPOSITOR_HPP

#include "wlroots_headers.hpp"
#include "wlr_scene_wrapper.hpp"
#include <stdexcept>
#include <wayland-server-core.h>
#include <wlr/util/log.h>
#include <iostream>

class CompositorServer {
        private:
                struct wl_display    *   _display    {nullptr};
                struct wlr_backend   *   _backend    {nullptr};
                struct wlr_renderer  *   _renderer   {nullptr};
                struct wlr_allocator *   _allocator  {nullptr};
                struct wlr_scene     *   _scene  {nullptr};
                struct wlr_xdg_shell *   _xdg_shell  {nullptr};
                struct wlr_output_layout * _output_layout {nullptr};

                wl_listener on_new_output;
                wl_listener on_new_toplevel;

                wl_event_loop* _event_loop {nullptr};

                static void handle_new_output_wl(wl_listener *listener, void *data)
                {
                        CompositorServer *server = wl_container_of(listener, server, on_new_output);
                        wlr_output *output = static_cast<wlr_output*>(data);
                        server->handle_new_output(output);
                }

                static void handle_new_toplevel_wl(wl_listener *listener, void *data)
                {
                        CompositorServer *server = wl_container_of(listener, server, on_new_toplevel);
                        wlr_xdg_toplevel *toplevel = static_cast<wlr_xdg_toplevel*>(data);
                        server->handle_new_toplevel(toplevel);
                }

                void handle_new_output(wlr_output* output)
                {
                        std::cout << "New display detected: " << output->name << "\n";
                        wlr_output_init_render(output, _allocator, _renderer);
                        wlr_output_state state;
                        wlr_output_state_init(&state);

                        wlr_output_mode* mode = wlr_output_preferred_mode(output);
                        if (mode)
                                wlr_output_state_set_mode(&state, mode);
                        wlr_output_state_set_enabled(&state, true);
                        wlr_output_commit_state(output, &state);
                        wlr_output_state_finish(&state);

                        wlr_output_layout_add_auto(_output_layout, output);
                        wlr_scene_output_create(_scene, output);
                }

                void handle_new_toplevel(wlr_xdg_toplevel* toplevel)
                {
                        std::cout << "Tracking new window" << "\n";
                        wlr_scene_xdg_surface_create(&_scene->tree, toplevel->base);
                }

        public:
                CompositorServer()
                {
                        wlr_log_init(WLR_DEBUG, nullptr);
                        _display = wl_display_create();
                        if (!_display)
                                throw std::runtime_error("[error] failed to create display");

                        _event_loop = wl_display_get_event_loop(_display);
                        if (!_event_loop)
                                throw std::runtime_error("[error] failed to create event_loop");

                        _backend = wlr_backend_autocreate(_event_loop, nullptr);
                        if (!_backend)
                                throw std::runtime_error("[error] failed to create backend");

                        _renderer = wlr_renderer_autocreate(_backend);
                        wlr_renderer_init_wl_display(_renderer, _display);
                        _allocator = wlr_allocator_autocreate(_backend, _renderer);

                        _scene = wlr_scene_create();
                        _output_layout = wlr_output_layout_create(_display);

                        on_new_output = { };
                        on_new_output.notify = handle_new_output_wl;
                        wl_signal_add(&_backend->events.new_output, &on_new_output);

                        _xdg_shell = wlr_xdg_shell_create(_display, 3);
                        if (!_xdg_shell)
                                throw std::runtime_error("[error] failed to create xdg_shell");

                        on_new_toplevel = { };
                        on_new_toplevel.notify = handle_new_toplevel_wl;
                        wl_signal_add(&_xdg_shell->events.new_toplevel, &on_new_toplevel);
                }

                void run()
                {
                        const char *socket = wl_display_add_socket_auto(_display);
                        if (!socket)
                                throw std::runtime_error("[error] failed to add display socket");
                        std::cout << "Running compositor on wayland socket: " << socket << "\n";

                        if (!wlr_backend_start(_backend))
                                throw std::runtime_error("[error] failed to start backend");

                        std::cout << "Compositor started. Set WAYLAND_DISPLAY=" << socket << "\n";

                        wl_display_run(_display);
                }

                ~CompositorServer()
                {
                        wl_display_destroy_clients(_display);
                        wl_display_destroy(_display);
                }
};

#endif
