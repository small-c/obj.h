
set_kind("binary")
add_rules("mode.release")
add_includedirs("./")

if is_os("windows") then
    target("win32_app")
        add_files("tests/win32_app/*.c")
        add_links("kernel32", "user32")
end

target("foo")
    add_files("tests/foo.c")

target("dog")
    add_files("tests/dog.c")

target("const_cast")
    add_files("tests/const_cast.c")

target("animal")
    add_files("tests/animal/*.c")

target("shape")
    add_files("tests/shape/*.c")