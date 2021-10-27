project(grpc_iface_demo)

find_package(Qt5Core)

SET(GRPC_INCLUDE
	f:/work/vcpkg/installed/x64-windows/include
	.
)

SET(GRPC_SOURCES
	./grpc/service.pb.cc
	./grpc/service.grpc.pb.cc
)

SET(GRPC_LIBS
	F:/work/vcpkg/installed/x64-windows/lib/abseil_dll.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_flags.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_flags_commandlineflag.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_flags_commandlineflag_internal.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_flags_config.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_flags_internal.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_flags_marshalling.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_flags_parse.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_flags_private_handle_accessor.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_flags_program_name.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_flags_reflection.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_flags_usage.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_flags_usage_internal.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_random_internal_distribution_test_util.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_statusor.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_strerror.lib
	F:/work/vcpkg/installed/x64-windows/lib/absl_wyhash.lib
	F:/work/vcpkg/installed/x64-windows/lib/address_sorting.lib
	F:/work/vcpkg/installed/x64-windows/lib/cares.lib
	F:/work/vcpkg/installed/x64-windows/lib/gpr.lib
	F:/work/vcpkg/installed/x64-windows/lib/grpc.lib
	F:/work/vcpkg/installed/x64-windows/lib/grpc_csharp_ext.lib
	F:/work/vcpkg/installed/x64-windows/lib/grpc_plugin_support.lib
	F:/work/vcpkg/installed/x64-windows/lib/grpc_unsecure.lib
	F:/work/vcpkg/installed/x64-windows/lib/grpc_upbdefs.lib
	F:/work/vcpkg/installed/x64-windows/lib/grpc++.lib
	F:/work/vcpkg/installed/x64-windows/lib/grpc++_alts.lib
	F:/work/vcpkg/installed/x64-windows/lib/grpc++_error_details.lib
	F:/work/vcpkg/installed/x64-windows/lib/grpc++_reflection.lib
	F:/work/vcpkg/installed/x64-windows/lib/grpc++_unsecure.lib
	F:/work/vcpkg/installed/x64-windows/lib/grpcpp_channelz.lib
	F:/work/vcpkg/installed/x64-windows/lib/libcrypto.lib
	F:/work/vcpkg/installed/x64-windows/lib/libprotobuf.lib
	#F:/work/vcpkg/installed/x64-windows/lib/libprotobuf-lite.lib
	F:/work/vcpkg/installed/x64-windows/lib/libprotoc.lib
	F:/work/vcpkg/installed/x64-windows/lib/libssl.lib
	F:/work/vcpkg/installed/x64-windows/lib/re2.lib
	F:/work/vcpkg/installed/x64-windows/lib/upb.lib
	F:/work/vcpkg/installed/x64-windows/lib/upb_fastdecode.lib
	F:/work/vcpkg/installed/x64-windows/lib/upb_handlers.lib
	F:/work/vcpkg/installed/x64-windows/lib/upb_json.lib
	F:/work/vcpkg/installed/x64-windows/lib/upb_pb.lib
	F:/work/vcpkg/installed/x64-windows/lib/upb_reflection.lib
	F:/work/vcpkg/installed/x64-windows/lib/upb_textformat.lib
	F:/work/vcpkg/installed/x64-windows/lib/zlib.lib
	wsock32 ws2_32
)

add_executable(server main_server.cpp ${GRPC_SOURCES})
target_include_directories(server PRIVATE ${GRPC_INCLUDE})
target_link_libraries(server Qt5::Core)
target_link_libraries(server ${GRPC_LIBS})

add_executable(client main_client.cpp ${GRPC_SOURCES})
target_include_directories(client PRIVATE ${GRPC_INCLUDE})
target_link_libraries(client Qt5::Core)
target_link_libraries(client ${GRPC_LIBS})
