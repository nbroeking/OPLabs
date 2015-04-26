//
//  TestResults.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/14/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "TestResults.h"

@implementation TestResults
@synthesize dns, latency, packetloss, throughputUpload, throughputDownload, packetlossUnderLoad, latencyUnderLoad, mobileIdentifier, valid, routerIdentifier,latencySD;
@synthesize setID;

//Initlize everythng to 0 when it is a normal init
-(instancetype)init{
    if (self = [super init]) {
        
        valid = false;
        dns = 0.0;
        latency = 0.0;
        packetloss = 0.0;
        throughputUpload = 0.0;
        throughputDownload = 0.0;
        latencyUnderLoad = NULL;
        packetlossUnderLoad = 0.0;
        setID = -1;
    }
    return self;
}

//Print all the values for debug purposes
-(void) print {
    NSLog( @"%@",[[NSString alloc] initWithFormat:@"state=finished\ndns_response_avg=%f\npacket_loss=%f\nlatency_avg=%f\nlatencyStd=%f\nupload_throughputs=%fMbps\ndownload_throughputs=%fMbps\npacket_loss_under_load=%f\nthroughput_latency=%@",dns,packetloss,latency,latencySD,throughputUpload /1000/1000 *8, throughputDownload /1000/1000*8, packetlossUnderLoad, latencyUnderLoad]);
}
//Return all the values as a body of a html post so we can send the results to the controller
-(NSString *)getPost{
    
    if( !valid){
        return @"state=error";
    }
    
    return [[NSString alloc] initWithFormat:@"state=finished&dns_response_avg=%f&packet_loss=%f&latency_avg=%f&upload_throughputs=%f&download_throughputs=%f&packet_loss_under_load=%f&throughput_latency=%@&latency_sdev=%f",dns,packetloss,latency,throughputUpload, throughputDownload, packetlossUnderLoad, latencyUnderLoad, latencySD];
    
}
@end
