#include <stdio.h>                                                                                                                                         
#include <stdlib.h>                                                                                                                                        
#include <string.h>                                                                                                                                        
#include <unistd.h>                                                                                                                                        
#include <sys/types.h>                                                                                                                                     
#include <sys/socket.h>                                                                                                                                    
#include <arpa/inet.h>                                                                                                                                     
#include <netinet/in.h>                                                                                                                                    
#include <fcntl.h>                                                                                                                                         
#include <sys/wait.h>                                                                                                                                      
                                                                                                                                                           
#define FROM_PATH 0                                                                                                                                        
#define PADDING_SIZE 300                                                                                                                                   
#define MAX_CLIENTS 20                 
#define HTTP_REQUEST_BUFFER 1024 * 10
                                                                                                                                                           
#define SHARE_BUFFER_PATH "./.src/share-buffer.txt"                                                                                                             
                                                                                                                                                           
#if FROM_PATH                                                                                                                                              
        #undef SHARE_BUFFER_PATH                                                                                                                           
        #define SHARE_BUFFER_PATH "~/cmdTools/tmp/share-buffer.txt"                                                                                        
#endif                                                                                                                                                     
                                                                                                                                                           
                                                                                                                                                           
void error_and_exit(const char* error){                                                                                                                    
        printf("[ERROR] : %s\n", error);                                                                                                                   
        exit(1);                                                                                                                                           
}                                                                                                                                                          
                                                                                                                                                           
                                                                                                                                                           
int main(int argc, char* argv[]){                                                                                                                          
                                                                                                                                                           
        if(argc < 2 || strncmp(argv[1], "-h", 2) == 0 || strncmp(argv[1], "-help", 5) == 0){                                                               
                printf("Usage : share [OPTION] [FILE]\n");                                                                                                 
                printf("share is a tool that allows serving content (markup or plain text) over HTTP.\n\n");                                               
                printf("SERVING BY PASTING CONTENT:\nExample - To share some content over HTTP at PORT 4993, run : share 4993\nPaste your content and hit <
enter> followed by <ctrl c> to start serving. \nThe serving stays live on PORT 4993 until you kill the process.\n\n");                                     
                printf("SERVING A FILE:\nExample - To server a file say `../data/data.txt` over HTTP at PORT 3829, run : share 3829 ../data/data.txt \nThe 
serving stays live until you kill the process\n\n");                                                                                                       
                exit(0);                                                                                                                                   
        }                                                                                                                                                  
                                                                                                                                                           
                                                                                                                                                           
        const int PORT = atoi(argv[1]);                                                                                                                    
        char* buffer;                                                                                                                                      

        const char* headers = "HTTP/1.1 200 OK\nContent-Type : text/html; charset=UTF-8\n";                                                                
        const char* styling = "";                                                                                                                          
"<style>\                                                                                                                                                  
        pre{\                                                                                                                                              
                font-family : consolas;\                                                                                                                   
                background : gray;\                                                                                                                        
        }\                                                                                                                                                 
</style>";                                                                                                                                                 
                                                                                                                                                           
        if(argc > 2){                                                                                                                                      
                FILE* fptr = fopen(argv[2], "r");                                                                                                          
                if(!fptr) error_and_exit("Couldn't find file");                                                                                            
                fseek(fptr, 0, SEEK_END);                                                                                                                  
                const long FILE_SIZE = ftell(fptr);                                                                                                        
                printf("Share-Buffer-File size : %ld\n", FILE_SIZE);                                                                                       
                                                                                                                                                           
                fseek(fptr, 0, SEEK_SET);                                                                                                                  
                buffer = (char*)(malloc(FILE_SIZE + PADDING_SIZE));                                                                                        
                bzero(buffer, FILE_SIZE);                                                                                                                  
                                                                                                                                                           
                int bytes_read = 0;                                                                                                                        
                char* ptr = buffer;                                                                                                                        
                while(bytes_read++ < FILE_SIZE){                                                                                                           
                        fread(ptr, 1, 1, fptr);                                                                                                            
                        ptr++;                                                                                                                             
                }                                                                                                                                          

                fclose(fptr);                                                                                                                              
                                                                                                                                                           
        }else{                                                                                                                                             

                int res = system("cat > "SHARE_BUFFER_PATH"");                                                                                             
                                                                                                                                                           
                FILE* fptr = fopen(SHARE_BUFFER_PATH, "r");                                                                                                
                if(!fptr) error_and_exit("Could not find file");                                                                                           
                                                                                                                                                           
                fseek(fptr, 0, SEEK_END);                                                                                                                  
                const long FILE_SIZE = ftell(fptr);                                                                                                        
                                                                                                                                                           
                fseek(fptr, 0, SEEK_SET);                                                                                                                  
                printf("Share-Buffer-File size : %ld\n", FILE_SIZE);                                                                                       
                buffer = (char*)(malloc(FILE_SIZE + 100));                                                                                                 
                bzero(buffer, FILE_SIZE+100);                                                                                                              


                int bytes_read = 0;                                                                                                                        
                char* ptr = buffer;                                                                                                                        
                while(bytes_read++ < FILE_SIZE){                                                                                                           
                        fread(ptr, 1, 1, fptr);                                                                                                            
                        ptr++;                                                                                                                             
                }                                                                                                                                          
                                                                                                                                                           

                fclose(fptr);                                                                                                                              
        }                                                                                                                                                  
                                                                                                                                                           
                                                                                                                                                           
        char response[strlen(headers) + strlen(styling) + strlen(buffer) + PADDING_SIZE];                                                                  
        bzero(response, strlen(headers) + strlen(styling) + strlen(buffer) + PADDING_SIZE);                                                                

        sprintf(response, "%s\n<!DOCTYPE html>%s<pre><code>%s</code></pre></html>", headers, styling, buffer);                                             
                                                                                                                                                           
        printf("Response Size : %ld\n", strlen(response));                                                                                                 
        //printf("%s\n", response);                                                                                                                        
                                                                                                                                                           
                                                                                                                                                           
        int server_sockfd;                                                                                                                                 
        struct sockaddr_in address;                                                                                                                        
                                                                                                                                                           
        if((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) error_and_exit("Couldn't create socket\n");                                              

        address.sin_port = htons(PORT);                                                                                                                    
        address.sin_family = AF_INET;                                                                                                                      
        address.sin_addr.s_addr = INADDR_ANY;                                                                                                              
                                                                                                                                                           
        // Bind                                                                                                                                            
        if(bind(server_sockfd, (struct sockaddr*)(&address), sizeof(address)) < 0) error_and_exit("Bind error, try with another PORT\n");                  
                                                                                                                                                           
        // Listen                                                                                                                                          
        if(listen(server_sockfd, 6) < 0) error_and_exit("Listen error\n");                                                                                 

        printf("Serving at PORT %d\n\n", PORT);                                                                                                            
                                                                                                                                                           
        int address_len = sizeof(address);                                                                                                                 
        char request[HTTP_REQUEST_BUFFER] = {0};                                                                                                                          
                                                                                                                                                           
        while(1){                                                                                                                                          
                int new_socket;                                                                                                                            
//              if((new_socket = accept(server_sockfd, (struct sockaddr*)(&address), (socklen_t*)(&address_len))) < 0) continue;                           
                new_socket = accept(server_sockfd, (struct sockaddr*)(&address), (socklen_t*)(&address_len));                                              
                recv(new_socket, request, 1024, 0);                                                                                                        
                printf("Request : %s\n\n", request);                                                                                                       
                bzero(request, HTTP_REQUEST_BUFFER);                                                                                                                      
                                                                                                                                                           
                send(new_socket, response, strlen(response), 0);                                                                                           
                close(new_socket);                                                                                                                         
                                                                                                                          
                                                                                                                                                           
        }                                                                                                                                                  
                                                                                                                                                           
                                                                                                                                                           
        close(server_sockfd);                                                                                                                              
                                                                                                                                                           
                                                                                                                                                           
                                                                                                                                                           
        return 0;                                                                                                                                          
}                                  
