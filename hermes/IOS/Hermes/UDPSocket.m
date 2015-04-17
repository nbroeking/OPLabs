//
//  UDPSocket.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/16/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "UDPSocket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

@interface UDPSocket ()

@property(strong, nonatomic) TestSettings *testSettings;
@property(assign, nonatomic) BOOL valid;

@end

@implementation UDPSocket
{
    CFHostRef _cfHost;
    int sock;
}
@synthesize testSettings;
@synthesize valid;
-(instancetype)init :(TestSettings*)settings{
    
    self = [super init];
    self.valid = false;
    if( self){
        
        //Initlize self
        [self setTestSettings:settings];
        
        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            NSLog(@"Error creating the socket");
            return self;
        }
        
        struct sockaddr_in myaddr;
        
        memset((char *)&myaddr, 0, sizeof(myaddr));
        myaddr.sin_family = AF_INET;
        myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        myaddr.sin_port = htons(0);
        
        if (bind(sock, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
            NSLog(@"Error Binding Socket");
            return self;
        }
        struct timeval tv;
        
        tv.tv_sec = 2;  /* 30 Secs Timeout */
        tv.tv_usec = 0;  // Not init'ing this can cause strange errors
    
        if( setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval)) < 0){
            NSLog(@"Error seting timeout");
            return self;
        }
        
        valid = true;
    }
    return self;
}

-(BOOL)sendData:(NSData *)data to:(NSString *)to{
    
    ssize_t bytesWritten;
    
    struct sockaddr_in myaddr;
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(53);
    
    if(inet_pton(AF_INET, [to UTF8String], &myaddr.sin_addr.s_addr)<=0)
    {
        NSLog(@"Could not get IP address of server in UDP Socket");
        return false;
    }
    
    bytesWritten = sendto(sock, [data bytes], [data length], 0, (const struct sockaddr*)&myaddr, sizeof(myaddr));
    
    if (bytesWritten < 0) {
        NSLog(@"Error sending Bytes");
        return false;
    } else  if (bytesWritten == 0) {
        NSLog(@"Error sending bytes because we wrote 0 bytes");
        return false;
    }
    
    //No error
    return true;
}

-(NSData *)recvData:(NSInteger)timeout
{
    struct sockaddr_storage addr;
    socklen_t               addrLen;
    uint8_t                 buffer[1024];
    ssize_t                 bytesRead;
    
    addrLen = sizeof(addr);
    
    bytesRead = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &addr, &addrLen);
    
    if (bytesRead < 0) {
        NSLog(@"Error reading bytes");
        return NULL;
    } else if (bytesRead == 0) {
        NSLog(@"Socket had 0 bytes read");
        return NULL;
    }

    NSData *data = [[NSData alloc] initWithBytes:buffer length: bytesRead];
    return data;
}

-(void)shutdown{
    
    close(sock);
}
@end
