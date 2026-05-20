# miracle

`miracle` is a small compositor project for learning how [`Mir`](https://github.com/canonical/mir) and [`Miriway`](https://github.com/Miriway/Miriway) are put together.

The goal here is understanding, experimentation, and building familiarity with Miral-based compositor structure. It is not intended to copy Miriway's features or to become a production-ready Linux desktop compositor.

## Project scope

- Learn from Mir and Miriway by implementing small, focused pieces.
- Keep the code compact enough to study and change easily.
- Prefer clarity and experimentation over completeness.

## How to run

To build `miracle`:

```bash
mkdir -p build
cd build
cmake ..
make -j$(nproc)
```

To run:

```bash
WAYLAND_DISPLAY=wayland-99 ./miracle
```

## Current comparison with Miriway

This comparison is between my project, `miracle`, and `Miriway`:

| Feature | Miriway | miracle | Notes |
|---|---|---|---|
| Base compositor | Yes | Yes | Both are Mir/Miral compositors. |
| Window management policy | Custom floating policy | `FloatingWindowManager` | `miracle` uses Miral's stock floating policy; Miriway adds its own window management behavior on top of Miral. |
| Startup apps | Yes | Yes | `miracle` supports `startup-app=...`; Miriway also supports startup/session hooks and shell components. |
| Config reload on change | Yes | Yes | Both reload config at runtime. |
| Custom keybindings | Yes | Yes | Both support config-driven shortcuts. |
| Launch external commands from keybinds | Yes | Yes | Both can spawn commands or apps from shortcuts. |
| Internal WM commands | Rich | Minimal | Miriway supports dock, maximize, workspaces, always-on-top, and exit; `miracle` currently supports `@exit`. |
| Dynamic workspaces | Yes | No | Miriway includes a workspace manager and workspace navigation. |
| Move focused window between workspaces | Yes | No | Miriway supports Shift variants for workspace actions. |
| Window docking / snapping | Yes | No | Miriway can dock windows left/right. |
| Toggle maximized | Yes | No | Implemented in Miriway’s custom policy. |
| Toggle always-on-top | Yes | No | Present in Miriway’s custom policy. |
| Pointer-driven snap/maximize on move | Yes | No | Miriway hooks pointer and move handling for this. |
| X11/Xwayland support | Yes | No explicit support | Miriway enables `X11Support{}`; `miracle` does not currently. |
| Shell components (panels, wallpaper, docks) | Yes | No | Miriway supports `shell-component=` with privileged protocol access. |
| Auto-restart shell components | Yes | No | Miriway restarts shell components with throttling. |
| Privileged shell-only keybinds | Yes | No | Miriway separates shell-privileged commands from regular app commands. |
| Wayland extension management | Advanced | No explicit management | Miriway selectively enables public and shell-only protocols. |
| `ext-workspace-v1` protocol | Yes | No | Miriway exports workspace state over Wayland. |
| Lockscreen / session lock | Yes | No | Miriway supports lockscreen integration. |
| Lock on idle timeout | Yes | No | Miriway can trigger the lockscreen on idle. |
| Accessibility helpers | Yes | No | Miriway includes magnifier, bounce keys, slow keys, sticky keys, and hover click. |
| Input config (keymap, touchpad, pointer tuning) | Yes | No explicit support | Miriway wires in broader input configuration support. |
| Cursor theme / scale / output filter | Yes | No explicit support | Present in Miriway runtime setup. |
| Decoration policy | Configurable | Fixed `prefer_csd()` | Miriway supports multiple decoration strategies; `miracle` currently hardcodes one. |
| Session startup/shutdown hooks | Yes | No | Miriway supports environment-driven session hooks. |
| Packaging/session integration | Yes | Minimal | Miriway includes desktop/session/snap/systemd integration; `miracle` is currently much smaller in scope. |
