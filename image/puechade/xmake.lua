set_languages("cxx20")

target("lib")
    set_kind("static")
    add_includedirs("libs/include")
    add_files("libs/src/*.cpp")

target("psnr")
    set_kind("binary")
    add_deps("lib")
    add_files("src/main_psnr.cpp")
    add_includedirs("include", "libs/include")

target("ber")
    set_kind("binary")
    add_deps("lib")
    add_files("src/main_ber.cpp")
    add_includedirs("include", "libs/include")

target("histogram")
    set_kind("binary")
    add_deps("lib")
    add_files("src/main_histogram.cpp")
    add_includedirs("include", "libs/include")

target("permut")
    set_kind("binary")
    add_deps("lib")
    add_files("src/main_permut.cpp")
    add_includedirs("include", "libs/include")

target("rpermut")
    set_kind("binary")
    add_deps("lib")
    add_files("src/main_rpermut.cpp")
    add_includedirs("include", "libs/include")

target("encrypt")
    set_kind("binary")
    add_deps("lib")
    add_files("src/main_encrypt.cpp")
    add_includedirs("include", "libs/include")

target("decrypt")
    set_kind("binary")
    add_deps("lib")
    add_files("src/main_decrypt.cpp")
    add_includedirs("include", "libs/include")

target("decrypt_bf")
    set_kind("binary")
    add_deps("lib")
    add_files("src/main_decrypt_bf.cpp")
    add_includedirs("include", "libs/include")
    
target("substitute")
    set_kind("binary")
    add_deps("lib")
    add_files("src/main_substitute.cpp")
    add_includedirs("include", "libs/include")

target("nn")
    set_kind("binary")
    add_deps("lib")
    add_files("src/main_nn.cpp")
    add_includedirs("include", "libs/include")

target("cnfilter")
    set_kind("binary")
    add_deps("lib")
    add_files("src/main_cnfilter.cpp")
    add_includedirs("include", "libs/include")