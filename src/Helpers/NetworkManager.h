//
//  NetworkHelper.hpp
//  LineDancer
//
//  Created by Kris Meeusen on 21/08/2017.
//
//

#pragma once

//osc
#include "cinder/osc/Osc.h"
#include "cinder/System.h"
#include "cinder/Signals.h"
#include "../VO/PointsPackage.h"

#include <map>
#include <queue>

using Receiver = ci::osc::ReceiverUdp;
using Sender = ci::osc::SenderUdp;
using protocol = asio::ip::udp;

class NetworkManager{
    
    
    // OSC
   
   
    std::string mOwnIpAdress;
    std::string mLastIpNr;
    int groupId = 0;
    float lastBroadcast;
    bool queueClear= false;

    void setupOSCSender();

    
    std::string extractLastIpNr(std::string& fullIp);
    
    std::queue<PointsPackage> pointsQueue;
    std::mutex mPointsQueueLock;
    
    std::queue<PointsPackage> shapesQueue;
    std::mutex mShapesQueueLock;
    
    void sendAlive();

public:
    
    void sendErase();

    NetworkManager();
    
    //threaded OSC server
    std::shared_ptr<asio::io_service>        mIoService;
    std::shared_ptr<asio::io_service::work>    mWork;
    std::thread                                mThread;
    
    ci::osc::UdpSocketRef    mSocket;
    ci::osc::ReceiverUdp mReceiver;
    ci::osc::SenderUdp mSender;
    std::map<uint64_t, protocol::endpoint> mConnections;
    
    ci::signals::Signal<void(PointsPackage pointPackage)>   onReceivePoints;
    ci::signals::Signal<void(PointsPackage pointPackage)> onReceiveShapes;
//    ci::signals::Signal<void(std::string&)>   onNewConnection;
    //ci::signals::Signal<void(std::string&)>   onAlivePing;
    ci::signals::Signal<void()>   onErase;

    std::map<std::string,float> mAliveIps;

    bool setup();
    void update();
    void setNextGroup();

    std::string const getLastMyIpNr();
    int const getGroupId();
    bool isMessageAllowed(const ci::osc::Message &msg);

    void sendOscMessage(std::string command,ci::vec3 point);
    void sendPoints(std::vector<ci::vec3> &points, bool isEraserOn, ci::Color color, int frameId);
    void sendTwoPointShape(cinder::vec3& point1,cinder::vec3& point2, std::string shape,std::string color);
    void cleanup() ;
    
};


