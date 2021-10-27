# set the project name
project(grpc_iface_demo)

find_package(Qt5Core)

#set(PKG_CONFIG_EXECUTABLE f:/work/pkgconfig/pkg-config.exe)

#set(CMAKE_CXX_FLAGS_RELEASE "/MT")
#set(CMAKE_CXX_FLAGS_DEBUG "/MTd")

#find_package(PkgConfig)
#find_package(absl CONFIG REQUIRED PATHS f:/work/vcpkg/installed/x64-windows-static/share/absl)
#find_package(c-ares CONFIG REQUIRED PATHS f:/work/vcpkg/installed/x64-windows-static/share/c-ares)
#find_package(Protobuf CONFIG REQUIRED PATHS f:/work/vcpkg/installed/x64-windows-static/share/protobuf)
#find_package(re2 CONFIG REQUIRED PATHS f:/work/vcpkg/installed/x64-windows-static/share/re2)
#find_package(upb CONFIG REQUIRED PATHS f:/work/vcpkg/installed/x64-windows-static/share/upb)
#find_package(gRPC CONFIG REQUIRED PATHS f:/work/vcpkg/installed/x64-windows/cmake)
#find_package(modules CONFIG REQUIRED)

# Note: 6 target(s) were omitted.


add_executable(server
	main_server.cpp
	./grpc/service.pb.cc
	./grpc/service.grpc.pb.cc
)
target_include_directories(server PRIVATE f:/work/vcpkg/installed/x64-windows/include)
target_include_directories(server PRIVATE .)
target_link_libraries(server Qt5::Core)

target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/abseil_dll.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_flags.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_flags_commandlineflag.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_flags_commandlineflag_internal.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_flags_config.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_flags_internal.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_flags_marshalling.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_flags_parse.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_flags_private_handle_accessor.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_flags_program_name.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_flags_reflection.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_flags_usage.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_flags_usage_internal.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_random_internal_distribution_test_util.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_statusor.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_strerror.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/absl_wyhash.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/address_sorting.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/cares.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/gpr.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/grpc.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/grpc_csharp_ext.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/grpc_plugin_support.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/grpc_unsecure.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/grpc_upbdefs.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/grpc++.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/grpc++_alts.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/grpc++_error_details.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/grpc++_reflection.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/grpc++_unsecure.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/grpcpp_channelz.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/libcrypto.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/libprotobuf.lib)
#target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/libprotobuf-lite.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/libprotoc.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/libssl.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/re2.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/upb.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/upb_fastdecode.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/upb_handlers.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/upb_json.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/upb_pb.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/upb_reflection.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/upb_textformat.lib)
target_link_libraries(server F:/work/vcpkg/installed/x64-windows/lib/zlib.lib)
target_link_libraries(server wsock32 ws2_32)


#target_link_libraries(server PUBLIC gRPC::gpr gRPC::upb gRPC::grpc gRPC::grpc++)
#target_link_libraries(server PUBLIC re2::re2 c-ares::cares)



#add_executable(client main_client.cpp)
#target_link_libraries(client Qt5::Core)
