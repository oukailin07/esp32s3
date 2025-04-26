idf_component_register(SRCS "main.c" "./WIFI/ConnectWIFI.c" "./dns_server/my_dns_server.c" "./web_server/webserver.c" 
INCLUDE_DIRS "." "./WIFI" "./dns_server" "./web_server"  
EMBED_FILES "./web_server/index.html"
)