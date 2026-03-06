target("intollib-win64")
    set_toolchains("clang-win-amd64")
    set_kind("shared")
    set_targetdir("output/win-amd64")
    add_linkdirs("lib/win-msvc-amd64")
    add_links("SDL3-static", "wgpu_native");

    add_syslinks("kernel32.lib", 
        "ucrt.lib", "user32.lib", "Ole32.lib", "gdi32.lib", "advapi32.lib",
        "shell32.lib", "imm32.lib", "winmm.lib", "setupapi.lib", "version.lib",
        "OleAut32.lib", "ntdll.lib", "Ws2_32.lib", "opengl32.lib", "userenv.lib",
        "Propsys.lib", "RuntimeObject.lib")

    add_files("src/osapi/win/*.c", "src/osapi/*.c");
    add_files("src/extern/**/*.c");
    add_files("src/abs/sdl3/*.c", "src/abs/sdl3/**/*.c")
    add_files("src/*.c")
    add_includedirs("includes")

target("test-win64")
    set_kind("binary")
    set_targetdir("output/win-amd64")
    set_filename("testapp.exe")
    add_linkdirs("output/win-amd64")
    add_links("intollib-win64.lib")
    
    add_files("examples/test/*.c")
    add_includedirs("includes")



toolchain("clang-win-amd64")
    set_kind("standalone")
    set_toolset("cc", "clang")
    set_toolset("ld", "lld-link")
    set_toolset("as", "nasm")
toolchain_end()