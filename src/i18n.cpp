// Copyright (C) 2025 Langning Chen
//
// This file is part of paper.
//
// paper is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// paper is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with paper.  If not, see <https://www.gnu.org/licenses/>.

#include "i18n.hpp"
#include "io.hpp"
#include <windows.h>

namespace I18N
{
    static Language currentLanguage = Language::ENGLISH;

    static std::unordered_map<std::string, std::unordered_map<Language, std::string>> translations = {
        // Application messages
        {"app_starting", {{Language::ENGLISH, "Application starting..."}, {Language::CHINESE, "正在启动应用..."}}},
        {"app_terminating", {{Language::ENGLISH, "Application terminating normally"}, {Language::CHINESE, "应用正常关闭"}}},
        {"paper", {{Language::ENGLISH, "PAPER - Pen Adb Password Easily Reset"}, {Language::CHINESE, "PAPER - 一键重置词典笔 ADB 密码"}}},

        // Debug messages
        {"binding_signal_handlers", {{Language::ENGLISH, "Binding signal handlers..."}, {Language::CHINESE, "正在绑定信号处理器..."}}},
        {"signal_handlers_bound", {{Language::ENGLISH, "Signal handlers bound successfully"}, {Language::CHINESE, "信号处理器绑定成功"}}},
        {"checking_admin_privileges", {{Language::ENGLISH, "Checking administrator privileges..."}, {Language::CHINESE, "正在检查管理员权限..."}}},
        {"not_admin_requesting_elevation", {{Language::ENGLISH, "Not running as administrator, requesting elevation"}, {Language::CHINESE, "非管理员模式，正在请求提权..."}}},
        {"attempting_elevation", {{Language::ENGLISH, "Attempting to elevate privileges..."}, {Language::CHINESE, "正在尝试提升权限..."}}},
        {"elevation_request_sent", {{Language::ENGLISH, "Elevation request sent, exiting current process"}, {Language::CHINESE, "提权请求已发送，正在退出当前进程"}}},
        {"already_admin", {{Language::ENGLISH, "Already running with administrator privileges"}, {Language::CHINESE, "已在管理员权限下运行"}}},

        // Packet capture
        {"starting_packet_capture", {{Language::ENGLISH, "Starting packet capture process..."}, {Language::CHINESE, "正在启动抓包进程..."}}},
        {"delta_url_extracted", {{Language::ENGLISH, "Delta URL extracted"}, {Language::CHINESE, "已提取增量更新网址"}}},

        // Hash processing
        {"calculating_hash", {{Language::ENGLISH, "Calculating hash..."}, {Language::CHINESE, "正在计算哈希值..."}}},
        {"calculating_full_md5", {{Language::ENGLISH, "Calculating full file MD5..."}, {Language::CHINESE, "正在计算完整文件的 MD5 值..."}}},
        {"searching_hash_patterns", {{Language::ENGLISH, "Searching for hash patterns in file"}, {Language::CHINESE, "正在文件中搜索哈希模式"}}},
        {"file_size", {{Language::ENGLISH, "File size"}, {Language::CHINESE, "文件大小"}}},
        {"bytes", {{Language::ENGLISH, "bytes"}, {Language::CHINESE, "字节"}}},
        {"using_buffer_size", {{Language::ENGLISH, "Using buffer size"}, {Language::CHINESE, "当前缓冲区大小"}}},
        {"with_overlap", {{Language::ENGLISH, "with overlap"}, {Language::CHINESE, "及重叠部分"}}},

        // HTTP Server
        {"stopping_http_server", {{Language::ENGLISH, "Stopping HTTP server..."}, {Language::CHINESE, "正在停止 HTTP 服务器..."}}},
        {"initializing_winsock", {{Language::ENGLISH, "Initializing WinSock..."}, {Language::CHINESE, "正在初始化 WinSock..."}}},
        {"creating_server_socket", {{Language::ENGLISH, "Creating server socket..."}, {Language::CHINESE, "正在创建服务器套接字..."}}},
        {"setting_socket_options", {{Language::ENGLISH, "Setting socket options..."}, {Language::CHINESE, "正在设置套接字选项..."}}},
        {"binding_to_port", {{Language::ENGLISH, "Binding to port"}, {Language::CHINESE, "正在绑定端口"}}},
        {"starting_listen", {{Language::ENGLISH, "Starting to listen for connections..."}, {Language::CHINESE, "正在开始监听连接..."}}},
        {"server_started_press_x", {{Language::ENGLISH, "Server started, press [x] to stop"}, {Language::CHINESE, "服务器已启动，按 [x] 键停止"}}},
        {"server_listening", {{Language::ENGLISH, "Server listening on all interfaces, port"}, {Language::CHINESE, "服务器正在监听所有网络接口，端口"}}},
        {"failed_accept_connection", {{Language::ENGLISH, "Failed to accept client connection"}, {Language::CHINESE, "客户端连接失败"}}},
        {"new_client_connected", {{Language::ENGLISH, "New client connection accepted"}, {Language::CHINESE, "已接受新的客户端连接"}}},
        {"received_request_bytes", {{Language::ENGLISH, "Received request of"}, {Language::CHINESE, "收到请求，大小"}}},
        {"no_data_received", {{Language::ENGLISH, "No data received from client"}, {Language::CHINESE, "未收到客户端数据"}}},
        {"client_connection_closed", {{Language::ENGLISH, "Client connection closed"}, {Language::CHINESE, "客户端连接已关闭"}}},
        {"processing_http_request", {{Language::ENGLISH, "Processing HTTP request for path"}, {Language::CHINESE, "正在处理 HTTP 请求路径"}}},
        {"http_method", {{Language::ENGLISH, "HTTP method"}, {Language::CHINESE, "HTTP 方法"}}},
        {"serving_image_file", {{Language::ENGLISH, "Serving image file"}, {Language::CHINESE, "正在提供固件文件"}}},
        {"serving_register_data", {{Language::ENGLISH, "Serving register data"}, {Language::CHINESE, "正在提供注册数据"}}},
        {"serving_ota_data", {{Language::ENGLISH, "Serving OTA update data"}, {Language::CHINESE, "正在提供 OTA 更新数据"}}},
        {"serving_ota_report", {{Language::ENGLISH, "Serving OTA report"}, {Language::CHINESE, "正在提供 OTA 报告"}}},
        {"request_not_found_404", {{Language::ENGLISH, "Request not found, sending 404"}, {Language::CHINESE, "请求的资源不存在，返回 404"}}},
        {"failed_send_response", {{Language::ENGLISH, "Failed to send HTTP response"}, {Language::CHINESE, "发送 HTTP 响应失败"}}},
        {"sent_http_response", {{Language::ENGLISH, "Sent HTTP response"}, {Language::CHINESE, "HTTP 响应发送成功"}}},

        // HTTP Request parsing
        {"parsing_http_request", {{Language::ENGLISH, "Parsing HTTP request"}, {Language::CHINESE, "正在解析 HTTP 请求"}}},
        {"parsed_method_path_version", {{Language::ENGLISH, "Parsed method, path and version"}, {Language::CHINESE, "已解析方法、路径和版本"}}},
        {"parsing_http_headers", {{Language::ENGLISH, "Parsing HTTP headers"}, {Language::CHINESE, "正在解析 HTTP 标头"}}},
        {"parsed_header", {{Language::ENGLISH, "Parsed header"}, {Language::CHINESE, "已解析标头"}}},
        {"parsing_request_body", {{Language::ENGLISH, "Parsing request body"}, {Language::CHINESE, "正在解析请求数据体"}}},
        {"request_body_size", {{Language::ENGLISH, "Request body size"}, {Language::CHINESE, "请求数据体大小"}}},
        {"http_request_parsed", {{Language::ENGLISH, "HTTP request parsed successfully"}, {Language::CHINESE, "HTTP 请求解析完成"}}},
        {"parsing_range_header", {{Language::ENGLISH, "Parsing Range header"}, {Language::CHINESE, "正在解析 Range 标头"}}},
        {"range_header_parsed", {{Language::ENGLISH, "Range header parsed"}, {Language::CHINESE, "Range 标头解析完成"}}},

        // HTTP Response generation
        {"setting_response_body", {{Language::ENGLISH, "Setting response body"}, {Language::CHINESE, "正在设置响应数据体"}}},
        {"content_length_set", {{Language::ENGLISH, "Content-Length header set to"}, {Language::CHINESE, "Content-Length 标头设置为"}}},
        {"generating_response_header", {{Language::ENGLISH, "Generating response header"}, {Language::CHINESE, "正在生成响应标头"}}},
        {"response_status_code", {{Language::ENGLISH, "Response status code"}, {Language::CHINESE, "响应状态码"}}},
        {"adding_response_header", {{Language::ENGLISH, "Adding response header"}, {Language::CHINESE, "正在添加响应标头"}}},
        {"response_header_generated", {{Language::ENGLISH, "Response header generated"}, {Language::CHINESE, "响应标头生成完成"}}},
        {"generating_full_response", {{Language::ENGLISH, "Generating full HTTP response"}, {Language::CHINESE, "正在生成完整 HTTP 响应"}}},
        {"response_size", {{Language::ENGLISH, "Response size"}, {Language::CHINESE, "响应大小"}}},

        {"preparing_file_response", {{Language::ENGLISH, "Preparing file response for"}, {Language::CHINESE, "正在准备文件响应"}}},
        {"processing_range_request", {{Language::ENGLISH, "Processing range request"}, {Language::CHINESE, "正在处理范围请求"}}},
        {"range", {{Language::ENGLISH, "Range"}, {Language::CHINESE, "范围"}}},
        {"length", {{Language::ENGLISH, "length"}, {Language::CHINESE, "长度"}}},
        {"serving_full_file", {{Language::ENGLISH, "Serving full file"}, {Language::CHINESE, "正在提供完整文件"}}},
        {"sending_http_headers", {{Language::ENGLISH, "Sending HTTP headers"}, {Language::CHINESE, "正在发送 HTTP 标头"}}},
        {"failed_send_headers", {{Language::ENGLISH, "Failed to send HTTP headers"}, {Language::CHINESE, "发送 HTTP 标头失败"}}},
        {"starting_file_transfer", {{Language::ENGLISH, "Starting file transfer..."}, {Language::CHINESE, "正在开始文件传输..."}}},
        {"failed_send_file_data", {{Language::ENGLISH, "Failed to send file data"}, {Language::CHINESE, "发送文件数据失败"}}},
        {"sent_image_file", {{Language::ENGLISH, "Sent image file"}, {Language::CHINESE, "固件文件发送完毕"}}},
        {"file_transfer_completed", {{Language::ENGLISH, "File transfer completed, total sent"}, {Language::CHINESE, "文件传输完成，总计发送"}}},

        // Packet capture messages
        {"packet_received", {{Language::ENGLISH, "Packet received, length"}, {Language::CHINESE, "收到数据包，长度"}}},
        {"target_packet_found", {{Language::ENGLISH, "Target packet found, breaking capture loop"}, {Language::CHINESE, "已找到目标数据包，正在中断抓包"}}},
        {"packet_too_small_network", {{Language::ENGLISH, "Packet too small for network layer analysis"}, {Language::CHINESE, "数据包过小，无法进行网络层分析"}}},
        {"non_ipv4_packet", {{Language::ENGLISH, "Non-IPv4 packet detected"}, {Language::CHINESE, "检测到非 IPv4 数据包"}}},
        {"non_ipv4_packet_header", {{Language::ENGLISH, "Non-IPv4 packet in IP header"}, {Language::CHINESE, "IP 标头中包含非 IPv4 数据包"}}},
        {"non_tcp_packet", {{Language::ENGLISH, "Non-TCP packet detected"}, {Language::CHINESE, "检测到非 TCP 数据包"}}},
        {"network_layer_passed", {{Language::ENGLISH, "Network layer analysis passed"}, {Language::CHINESE, "网络层分析通过"}}},
        {"packet_too_small_transport", {{Language::ENGLISH, "Packet too small for transport layer analysis"}, {Language::CHINESE, "数据包过小，无法进行传输层分析"}}},
        {"invalid_tcp_header", {{Language::ENGLISH, "Invalid TCP header offset"}, {Language::CHINESE, "无效的 TCP 标头偏移量"}}},
        {"non_http_port", {{Language::ENGLISH, "Non-HTTP port detected: src"}, {Language::CHINESE, "检测到非 HTTP 端口：源"}}},
        {"dst", {{Language::ENGLISH, "dst"}, {Language::CHINESE, "目标"}}},
        {"transport_layer_passed", {{Language::ENGLISH, "Transport layer analysis passed, remaining payload"}, {Language::CHINESE, "传输层分析通过，剩余有效负载"}}},
        {"analyzing_packet", {{Language::ENGLISH, "Analyzing packet of"}, {Language::CHINESE, "正在分析数据包"}}},
        {"packet_rejected", {{Language::ENGLISH, "Packet rejected by layer analysis or no payload"}, {Language::CHINESE, "数据包被层分析拒绝或无有效负载"}}},
        {"http_payload_preview", {{Language::ENGLISH, "HTTP payload preview"}, {Language::CHINESE, "HTTP 有效负载预览"}}},
        {"ota_request_matched", {{Language::ENGLISH, "OTA request pattern matched!"}, {Language::CHINESE, "OTA 请求模式匹配成功！"}}},
        {"product_url", {{Language::ENGLISH, "Product URL"}, {Language::CHINESE, "产品网址"}}},
        {"json_body_parsed", {{Language::ENGLISH, "JSON body parsed successfully"}, {Language::CHINESE, "JSON 数据体解析成功"}}},
        {"failed_parse_json", {{Language::ENGLISH, "Failed to parse JSON body"}, {Language::CHINESE, "解析 JSON 数据体失败"}}},
        {"ota_request_not_matched", {{Language::ENGLISH, "OTA request pattern not matched"}, {Language::CHINESE, "OTA 请求模式不匹配"}}},
        {"initializing_capture", {{Language::ENGLISH, "Initializing packet capture system"}, {Language::CHINESE, "正在初始化抓包系统"}}},
        {"finding_devices", {{Language::ENGLISH, "Finding network devices..."}, {Language::CHINESE, "正在查找网络设备..."}}},
        {"error_finding_devices", {{Language::ENGLISH, "Error while finding network devices"}, {Language::CHINESE, "查找网络设备时出错"}}},
        {"searching_hotspot", {{Language::ENGLISH, "Searching for hotspot interface (192.168.137.1)..."}, {Language::CHINESE, "正在搜索热点接口 (192.168.137.1)..."}}},
        {"checking_device", {{Language::ENGLISH, "Checking device"}, {Language::CHINESE, "正在检查设备"}}},
        {"found_target_interface", {{Language::ENGLISH, "Found target interface"}, {Language::CHINESE, "已找到目标接口"}}},
        {"no_interface_found", {{Language::ENGLISH, "No available interface found, make sure you have opened the mobile hotspot."}, {Language::CHINESE, "未找到可用接口，请确保已开启移动热点。"}}},
        {"opening_capture_handle", {{Language::ENGLISH, "Opening capture handle for device"}, {Language::CHINESE, "正在为设备打开抓包句柄"}}},
        {"unable_open_adapter", {{Language::ENGLISH, "Unable to open the adapter"}, {Language::CHINESE, "无法打开网络适配器"}}},
        {"checking_datalink", {{Language::ENGLISH, "Checking data link type..."}, {Language::CHINESE, "正在检查数据链路类型..."}}},
        {"non_ethernet_link", {{Language::ENGLISH, "Non-Ethernet link layer detected. Packet parsing might be off."}, {Language::CHINESE, "检测到非以太网链路层，数据包解析可能有误。"}}},
        {"setting_packet_filter", {{Language::ENGLISH, "Setting up packet filter..."}, {Language::CHINESE, "正在设置数据包过滤器..."}}},
        {"compiling_filter", {{Language::ENGLISH, "Compiling filter"}, {Language::CHINESE, "正在编译过滤器"}}},
        {"unable_compile_filter", {{Language::ENGLISH, "Unable to compile the packet filter"}, {Language::CHINESE, "无法编译数据包过滤器"}}},
        {"setting_filter", {{Language::ENGLISH, "Setting packet filter..."}, {Language::CHINESE, "正在设置数据包过滤器..."}}},
        {"error_setting_filter", {{Language::ENGLISH, "Error setting the filter"}, {Language::CHINESE, "设置过滤器时出错"}}},
        {"waiting_update_packets", {{Language::ENGLISH, "Waiting for update packets..."}, {Language::CHINESE, "正在等待更新数据包..."}}},
        {"starting_capture_loop", {{Language::ENGLISH, "Starting packet capture loop..."}, {Language::CHINESE, "正在启动抓包循环..."}}},
        {"captured_update_request", {{Language::ENGLISH, "Captured update request for product"}, {Language::CHINESE, "已抓取到产品更新请求"}}},
        {"closing_capture_handle", {{Language::ENGLISH, "Closing packet capture handle"}, {Language::CHINESE, "正在关闭抓包句柄"}}},

        // Error and confirmation messages
        {"received_signal", {{Language::ENGLISH, "Received signal"}, {Language::CHINESE, "收到信号"}}},
        {"admin_privileges_required", {{Language::ENGLISH, "This action requires administrative privileges. Elevate now?"}, {Language::CHINESE, "此操作需要管理员权限，是否立即提权？"}}},
        {"user_declined_elevation", {{Language::ENGLISH, "User declined to elevate privileges"}, {Language::CHINESE, "用户已拒绝提权"}}},
        {"failed_get_module_name", {{Language::ENGLISH, "Failed to get module file name"}, {Language::CHINESE, "获取模块文件名失败"}}},
        {"failed_elevate_privileges", {{Language::ENGLISH, "Failed to elevate privileges"}, {Language::CHINESE, "提权失败"}}},
        {"failed_allocate_sid", {{Language::ENGLISH, "Failed to allocate and initialize Admin SID"}, {Language::CHINESE, "分配并初始化管理员 SID 失败"}}},
        {"failed_check_token", {{Language::ENGLISH, "Failed to check token membership"}, {Language::CHINESE, "检查令牌成员身份失败"}}},
        {"occurs_at", {{Language::ENGLISH, "Occurs at"}, {Language::CHINESE, "发生于"}}},
        {"press_any_key_exit", {{Language::ENGLISH, "Press any key to exit..."}, {Language::CHINESE, "按任意键退出..."}}},

        // Download messages
        {"preparing_modified_request", {{Language::ENGLISH, "Preparing modified request body for OTA check"}, {Language::CHINESE, "正在为 OTA 检查准备修改后的请求数据体"}}},
        {"modified_version_to", {{Language::ENGLISH, "Modified version to"}, {Language::CHINESE, "已将版本修改为"}}},
        {"initializing_wininet", {{Language::ENGLISH, "Initializing WinINet for HTTP request"}, {Language::CHINESE, "正在初始化 WinINet 以发送 HTTP 请求"}}},
        {"connecting_to_server", {{Language::ENGLISH, "Connecting to iotapi.abupdate.com"}, {Language::CHINESE, "正在连接到 iotapi.abupdate.com"}}},
        {"opening_http_request_for", {{Language::ENGLISH, "Opening HTTP request for"}, {Language::CHINESE, "正在为以下路径打开 HTTP 请求"}}},
        {"request_body_size", {{Language::ENGLISH, "Request body size"}, {Language::CHINESE, "请求数据体大小"}}},
        {"sending_http_request", {{Language::ENGLISH, "Sending HTTP request..."}, {Language::CHINESE, "正在发送 HTTP 请求..."}}},
        {"reading_http_response", {{Language::ENGLISH, "Reading HTTP response..."}, {Language::CHINESE, "正在读取 HTTP 响应..."}}},
        {"response_size", {{Language::ENGLISH, "Response size"}, {Language::CHINESE, "响应大小"}}},
        {"parsing_json_response", {{Language::ENGLISH, "Parsing JSON response..."}, {Language::CHINESE, "正在解析 JSON 响应..."}}},
        {"json_response_parsed", {{Language::ENGLISH, "JSON response parsed successfully"}, {Language::CHINESE, "JSON 响应解析成功"}}},
        {"response_json", {{Language::ENGLISH, "Response JSON"}, {Language::CHINESE, "响应的 JSON 数据"}}},
        {"checking_file_exists", {{Language::ENGLISH, "Checking if file already exists"}, {Language::CHINESE, "正在检查文件是否存在"}}},
        {"file_exists_skip_download", {{Language::ENGLISH, "Image file already exists. Do you want to skip the download?"}, {Language::CHINESE, "固件文件已存在，是否跳过下载？"}}},
        {"download_skipped", {{Language::ENGLISH, "Download skipped by user"}, {Language::CHINESE, "用户已跳过下载"}}},
        {"downloading_image_file", {{Language::ENGLISH, "Downloading image file..."}, {Language::CHINESE, "正在下载固件文件..."}}},
        {"download_url", {{Language::ENGLISH, "Download URL"}, {Language::CHINESE, "下载网址"}}},
        {"target_filename", {{Language::ENGLISH, "Target filename"}, {Language::CHINESE, "目标文件名"}}},
        {"download_completed", {{Language::ENGLISH, "Download completed"}, {Language::CHINESE, "下载完成"}}},
        {"failed_initialize_wininet", {{Language::ENGLISH, "Failed to initialize WinINet"}, {Language::CHINESE, "初始化 WinINet 失败"}}},
        {"failed_open_url", {{Language::ENGLISH, "Failed to open URL"}, {Language::CHINESE, "打开网址失败"}}},
        {"failed_create_output_file", {{Language::ENGLISH, "Failed to create output file"}, {Language::CHINESE, "创建输出文件失败"}}},
        {"could_not_get_content_length", {{Language::ENGLISH, "Could not get content length, proceeding without progress info"}, {Language::CHINESE, "无法获取内容长度，将在无进度信息的情况下继续"}}},

        // Hash processing messages
        {"found_sha256_hash_at", {{Language::ENGLISH, "Found SHA256 hash pattern at position"}, {Language::CHINESE, "在位置找到 SHA256 哈希模式"}}},
        {"found_md5_hash_at", {{Language::ENGLISH, "Found MD5 hash pattern at position"}, {Language::CHINESE, "在位置找到 MD5 哈希模式"}}},
        {"hash_pattern_search_completed", {{Language::ENGLISH, "Hash pattern search completed, found"}, {Language::CHINESE, "哈希模式搜索完成，共找到"}}},
        {"patterns", {{Language::ENGLISH, "patterns"}, {Language::CHINESE, "个模式"}}},
        {"calculating_md5_for_file", {{Language::ENGLISH, "Calculating MD5 for entire file"}, {Language::CHINESE, "正在计算整个文件的 MD5"}}},
        {"cannot_open_file", {{Language::ENGLISH, "Cannot open file"}, {Language::CHINESE, "无法打开文件"}}},
        {"md5_calculated_for", {{Language::ENGLISH, "MD5 calculated for"}, {Language::CHINESE, "已计算 MD5，文件大小"}}},
        {"calculating_md5_segment", {{Language::ENGLISH, "Calculating MD5 for file segment"}, {Language::CHINESE, "正在计算文件分段的 MD5"}}},
        {"segment_md5_calculated", {{Language::ENGLISH, "Segment MD5 calculated for"}, {Language::CHINESE, "分段 MD5 已计算，大小"}}},
        {"calculating_sha1_for_file", {{Language::ENGLISH, "Calculating SHA1 for file"}, {Language::CHINESE, "正在计算文件的 SHA1"}}},
        {"sha1_calculated_for", {{Language::ENGLISH, "SHA1 calculated for"}, {Language::CHINESE, "已计算 SHA1，文件大小"}}},
        {"finding_password", {{Language::ENGLISH, "Finding password..."}, {Language::CHINESE, "正在查找密码..."}}},
        {"starting_password_search", {{Language::ENGLISH, "Starting password pattern search in"}, {Language::CHINESE, "开始在文件中搜索密码模式"}}},
        {"no_passwords_found", {{Language::ENGLISH, "No passwords found"}, {Language::CHINESE, "未找到密码"}}},
        {"multiple_password_patterns", {{Language::ENGLISH, "Multiple password patterns found"}, {Language::CHINESE, "找到多个密码模式"}}},
        {"found_password_at_offset", {{Language::ENGLISH, "Found password pattern at offset"}, {Language::CHINESE, "在偏移量处找到密码模式"}}},
        {"hash_length", {{Language::ENGLISH, "Hash length"}, {Language::CHINESE, "哈希长度"}}},
        {"characters", {{Language::ENGLISH, "characters"}, {Language::CHINESE, "个字符"}}},
        {"input_new_password", {{Language::ENGLISH, "Please input new password"}, {Language::CHINESE, "请输入新密码"}}},
        {"password_cannot_be_empty", {{Language::ENGLISH, "Password cannot be empty."}, {Language::CHINESE, "密码不能为空。"}}},
        {"generating_hash_for_password", {{Language::ENGLISH, "Generating hash for new password..."}, {Language::CHINESE, "正在为新密码生成哈希..."}}},
        {"new_hash_generated", {{Language::ENGLISH, "New hash generated"}, {Language::CHINESE, "新哈希已生成"}}},
        {"password_hash_replacement_completed", {{Language::ENGLISH, "Password hash replacement completed"}, {Language::CHINESE, "密码哈希替换完成"}}},

        // Host file management
        {"enabling_host_redirect", {{Language::ENGLISH, "Enabling host redirection for iotapi.abupdate.com"}, {Language::CHINESE, "正在为 iotapi.abupdate.com 启用主机重定向"}}},
        {"disabling_host_redirect", {{Language::ENGLISH, "Disabling host redirection for iotapi.abupdate.com"}, {Language::CHINESE, "正在为 iotapi.abupdate.com 禁用主机重定向"}}},
        {"opening_hosts_file", {{Language::ENGLISH, "Opening hosts file"}, {Language::CHINESE, "正在打开 hosts 文件"}}},
        {"cannot_open_hosts_file", {{Language::ENGLISH, "Cannot open hosts file. Make sure you have administrator privileges."}, {Language::CHINESE, "无法打开 hosts 文件，请确保以管理员权限运行。"}}},
        {"reading_hosts_file", {{Language::ENGLISH, "Reading hosts file content"}, {Language::CHINESE, "正在读取 hosts 文件内容"}}},
        {"host_entry_already_exists", {{Language::ENGLISH, "Host entry already exists"}, {Language::CHINESE, "主机条目已存在"}}},
        {"adding_host_entry", {{Language::ENGLISH, "Adding host entry"}, {Language::CHINESE, "正在添加主机条目"}}},
        {"host_entry_added", {{Language::ENGLISH, "Host entry added successfully"}, {Language::CHINESE, "主机条目添加成功"}}},
        {"host_entry_not_found", {{Language::ENGLISH, "Host entry not found"}, {Language::CHINESE, "未找到主机条目"}}},
        {"removing_host_entry", {{Language::ENGLISH, "Removing host entry"}, {Language::CHINESE, "正在删除主机条目"}}},
        {"host_entry_removed", {{Language::ENGLISH, "Host entry removed successfully"}, {Language::CHINESE, "主机条目删除成功"}}},
        {"writing_hosts_file", {{Language::ENGLISH, "Writing hosts file"}, {Language::CHINESE, "正在写入 hosts 文件"}}},
        {"hosts_file_updated", {{Language::ENGLISH, "Hosts file updated successfully"}, {Language::CHINESE, "hosts 文件更新成功"}}},
        {"failed_write_hosts_file", {{Language::ENGLISH, "Failed to write hosts file"}, {Language::CHINESE, "写入 hosts 文件失败"}}},
        {"flushing_dns_cache", {{Language::ENGLISH, "Flushing DNS cache..."}, {Language::CHINESE, "正在刷新 DNS 缓存..."}}},
        {"loading_dnsapi_dll", {{Language::ENGLISH, "Loading dnsapi.dll"}, {Language::CHINESE, "正在加载 dnsapi.dll"}}},
        {"dns_cache_flushed", {{Language::ENGLISH, "DNS cache flushed successfully"}, {Language::CHINESE, "DNS 缓存刷新成功"}}},
        {"failed_load_dnsapi", {{Language::ENGLISH, "Failed to load dnsapi.dll"}, {Language::CHINESE, "加载 dnsapi.dll 失败"}}},
        {"failed_get_dns_function", {{Language::ENGLISH, "Failed to get DnsFlushResolverCache function"}, {Language::CHINESE, "获取 DnsFlushResolverCache 函数失败"}}},
        {"failed_flush_dns_cache", {{Language::ENGLISH, "Failed to flush DNS cache"}, {Language::CHINESE, "刷新 DNS 缓存失败"}}},

        // UI elements
        {"confirm_prefix", {{Language::ENGLISH, "[CONFIRM]"}, {Language::CHINESE, "[确认]"}}},
        {"confirm_suffix", {{Language::ENGLISH, "[y/N]"}, {Language::CHINESE, "[是(y)/否(n)]"}}},
        {"input_prefix", {{Language::ENGLISH, "[INPUT]"}, {Language::CHINESE, "[输入]"}}},
        {"debug_prefix", {{Language::ENGLISH, "[DEBUG]"}, {Language::CHINESE, "[调试]"}}},
        {"info_prefix", {{Language::ENGLISH, "[INFO]"}, {Language::CHINESE, "[信息]"}}},
        {"warn_prefix", {{Language::ENGLISH, "[WARN]"}, {Language::CHINESE, "[警告]"}}},
        {"error_prefix", {{Language::ENGLISH, "[ERROR]"}, {Language::CHINESE, "[错误]"}}},
    };

    void Initialize()
    {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        if (IO::Confirm("Use Chinese language? / 使用中文界面？"))
            SetLanguage(Language::CHINESE);
        else
            SetLanguage(Language::ENGLISH);
    }

    void SetLanguage(Language lang)
    {
        currentLanguage = lang;
    }
}
std::string t(const std::string &key)
{
    auto it = I18N::translations.find(key);
    if (it != I18N::translations.end())
    {
        auto langIt = it->second.find(I18N::currentLanguage);
        if (langIt != it->second.end())
            return langIt->second;
    }
    auto engIt = I18N::translations.find(key);
    if (engIt != I18N::translations.end())
    {
        auto englishIt = engIt->second.find(I18N::Language::ENGLISH);
        if (englishIt != engIt->second.end())
            return englishIt->second;
    }
    return key;
}
