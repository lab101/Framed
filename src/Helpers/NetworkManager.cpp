//
//  NetworkHelper.cpp
//  LineDancer
//
//  Created by Kris Meeusen on 21/08/2017.
//
//

#include "NetworkManager.h"
#include "cinder/Utilities.h"
#include "GlobalSettings.h"

using namespace ci;

using namespace ci::app;
using namespace std;
using namespace asio;
using namespace asio::ip;
using namespace ci::osc;

NetworkManager::NetworkManager()
	: mIoService(new asio::io_service), mWork(new asio::io_service::work(*mIoService)),

	mSocket(new udp::socket(*mIoService, udp::endpoint(udp::v4(), 3100))),

	mReceiver(3000, asio::ip::udp::v4(), *mIoService),
	mSender(mSocket, udp::endpoint(address_v4::broadcast(), 3000)) {
	mSocket->set_option(asio::socket_base::broadcast(true));
}


bool NetworkManager::setup() {


	mOwnIpAdress = System::getIpAddress();
	mLastIpNr = extractLastIpNr(mOwnIpAdress);


	mReceiver.setListener("/alive", [&](const osc::Message& msg) {
		isMessageAllowed(msg);
		});


	mReceiver.setListener("/erase", [&](const osc::Message& msg) {
		if (isMessageAllowed(msg)) {
			queueClear = true;
		}
		});

	mReceiver.setListener("/nrOfFrames", [&](const osc::Message& msg) {
		if (isMessageAllowed(msg)) {
			nrOfFramesChanged = msg.getArgInt32(1);
		}
		});


	mReceiver.setListener("/frameSpeed", [&](const osc::Message& msg) {
		if (isMessageAllowed(msg)) {
			frameSpeedChanged = msg.getArgInt32(1);
		}
		});
    
    mReceiver.setListener("/frameSize", [&](const osc::Message& msg) {
        if (isMessageAllowed(msg)) {
            isFrameSizeChanged = true;
            newFrameSize.x = msg.getArgInt32(1);
            newFrameSize.y = msg.getArgInt32(2);
        }
    });

	mReceiver.setListener("/points",
		[&](const osc::Message& msg) {
			if (isMessageAllowed(msg)) {

				int totals = msg.getNumArgs();

				int frameId = msg.getArgInt32(1);
				bool isEraserOn = msg.getArgBool(2);

				ci::Color color;
				color.r = msg.getArgFloat(3);
				color.g = msg.getArgFloat(4);
				color.b = msg.getArgFloat(5);


				std::vector<ci::vec3> points;
				for (int i = 6; i < totals; i += 3) {
					points.push_back(ci::vec3(msg[i].flt(), msg[i + 1].flt(), msg[i + 2].flt()));
				}
				PointsPackage newPackage;
				newPackage.setup(points, color);
				newPackage.setEraser(isEraserOn);
				newPackage.frameId = frameId;

				mPointsQueueLock.lock();
				pointsQueue.push(newPackage);
				mPointsQueueLock.unlock();
			}

		});

	mReceiver.setListener("/shape",
		[&](const osc::Message& msg) {

			if (isMessageAllowed(msg)) {


				//                                  message.setAddress("/shape");
				//
				//                           0       message.append(groupId);
				//                            1      message.append(frameId);
				//                            2      message.append(getToolStateString(toolstate));
				//
				//                             3     message.append(point1.x);
				//                             4     message.append(point1.y);
				//                             5     message.append(point1.z);
				//                             6     message.append(point2.x);
				//                             7     message.append(point2.y);
				//                             8     message.append(point2.z);
				//
				//                              9    message.append(color.r);            // 3
				//                               10   message.append(color.g);            // 4
				//                              11    message.append(color.b);            // 5

												  //std::string shape = msg.getArgString(1);
				int frameId = msg.getArgInt32(1);

				ToolState state = getToolStateByString(msg.getArgString(2));
				ci::vec3 p1(msg.getArgFloat(3), msg.getArgFloat(4), msg.getArgFloat(5));
				ci::vec3 p2(msg.getArgFloat(6), msg.getArgFloat(7), msg.getArgFloat(8));


				ci::Color color;
				color.r = msg.getArgFloat(9);
				color.g = msg.getArgFloat(10);
				color.b = msg.getArgFloat(11);


				std::vector<ci::vec3> points;
				points.push_back(p1);
				points.push_back(p2);

				PointsPackage newPackage;
				newPackage.setup(points, color);
				newPackage.frameId = frameId;
				newPackage.shape = state;

				mPointsQueueLock.lock();
				pointsQueue.push(newPackage);
				mPointsQueueLock.unlock();
			}

		});

	// For a description of the below setup, take a look at SimpleReceiver. The only difference
	// is the usage of the mutex around the connection map.
	try {
		mReceiver.bind();
	}
	catch (const osc::Exception& ex) {
		CI_LOG_E("Error binding: " << ex.what() << " val: " << ex.value());
	}


	mReceiver.listen(
		[](asio::error_code error, protocol::endpoint endpoint) -> bool {
			if (error) {
				CI_LOG_E("Error Listening: " << error.message() << " val: "
					<< error.value() << " endpoint: " << endpoint);
				return false;
			}
			else
				return true;
		});



	// Now that everything is setup, run the io_service on the other thread.
	mThread = std::thread(std::bind(
		[](std::shared_ptr<asio::io_service>& service) {
			service->run();
		}, mIoService));


	return true;
}


bool NetworkManager::isMessageAllowed(const osc::Message& msg) {


	if (msg.getNumArgs() < 1) return false;

	std::string remoteIp = msg.getSenderIpAddress().to_string();
	int incomingGroupId = msg[0].int32();

	// ignore our own packages wich return due broadcast
	if (remoteIp == mOwnIpAdress) return false;

	// check if we are in the same group.
	if (incomingGroupId != groupId) return false;


	std::string remoteIpLastDigits = extractLastIpNr(remoteIp);
	mAliveIps[remoteIpLastDigits] = ci::app::getElapsedSeconds();

	return true;
}

void NetworkManager::setNrOfFrames(int nrOfFrames) {
	osc::Message message;
	message.setAddress("/nrOfFrames");
	message.append(groupId);
	message.append(nrOfFrames);
	mSender.send(message);
}

void NetworkManager::setFrameSize(int width,int height)
{
    osc::Message message;
    message.setAddress("/frameSize");
    message.append(groupId);
    message.append(width);
    message.append(height);
    mSender.send(message);

}

void NetworkManager::setFrameSpeed(int frameSpeed) {
	osc::Message message;
	message.setAddress("/frameSpeed");
	message.append(groupId);
	message.append(frameSpeed);
	mSender.send(message);
}


void NetworkManager::setGroupId(int id) {
	groupId = id;
}

void NetworkManager::update() {
	if (ci::app::getElapsedSeconds() - lastBroadcast > 6) {
		sendAlive();
		lastBroadcast = app::getElapsedSeconds();
	}

	// thread safe events
	if (queueClear) {
		onErase.emit();
		queueClear = false;
	}

	if (nrOfFramesChanged > 0) {
		onNumberOfFramesChanged.emit(nrOfFramesChanged);
		nrOfFramesChanged = -1;
	}

	if (frameSpeedChanged > 0) {
		onFrameSpeedChanged.emit(frameSpeedChanged);
		frameSpeedChanged = -1;
	}

    if(isFrameSizeChanged){
        isFrameSizeChanged = false;
        onFrameSizeChanged.emit(newFrameSize);
    }

	mPointsQueueLock.lock();

	while (!pointsQueue.empty()) {

		onReceivePoints.emit(pointsQueue.front());
		pointsQueue.pop();
	}

	mPointsQueueLock.unlock();

	//mShapesQueueLock.lock();

//    while (!shapesQueue.empty()) {
//        onReceiveShapes.emit(shapesQueue.front());
//        shapesQueue.pop();
//    }
//
//    mShapesQueueLock.unlock();
//    while( mListener.hasWaitingMessages() ) {
//        osc::Message message;
//        mListener.getNextMessage( &message );
//
//        // return from our own broadcast
//        std::string remoteIp = message.getRemoteIp();
//
//        if( remoteIp ==  mOwnIpAdress)
//            continue;
//
//        std::string remoteLastNr = extractLastIpNr(remoteIp);
//        std::string const adress = message.getAddress();
//
//        int incomingGroupId =  message.getArgAsInt32(0);
//
//        // discard packages from other groups
//        if(incomingGroupId == groupId){
//
//            if(mAliveIps.find(remoteLastNr) == mAliveIps.end()){
//                onNewConnection.emit(remoteLastNr);
//            }
//            mAliveIps[remoteLastNr] = ci::app::getElapsedSeconds();
//            onAlivePing.emit(remoteLastNr);
//            if(adress == "points"){
//                int totals = message.getNumArgs() ;
//                bool isEraserOn = message.getArgAsInt32(1);
//                std::string color = message.getArgAsString(2);
//                std::vector<ci::vec3> points;
//                for(int i=3;i < totals;i+=3){
//                    float brushSize = message.getArgAsFloat(i+2) / GS()->performanceDownScale.value();
//                    points.push_back(ci::vec3(message.getArgAsFloat(i),message.getArgAsFloat(i+1),brushSize));
//                }
//                PointsPackage newPackage;
//                newPackage.setup(points, color);
//                newPackage.setEraser(isEraserOn);
//
//                onReceivePoints.emit(newPackage);
//            }else if(adress == "shape"){
//                std::vector<ci::vec3> points;
//                std::string shape = message.getArgAsString(1);
//                std::string color = message.getArgAsString(2);
//                for(int i=3;i <message.getNumArgs();i+=3){
//                    float brushSize = message.getArgAsFloat(i+2) / GS()->performanceDownScale.value();
//                    points.push_back(ci::vec3(message.getArgAsFloat(i),message.getArgAsFloat(i+1),brushSize));
//                }
//                PointsPackage newPackage;
//                newPackage.setup(points, color);
//                newPackage.setShape(shape);
//
//                onReceiveShapes.emit(newPackage);
//
//            }
//        }
//    }
}


std::string const NetworkManager::getLastMyIpNr() {
	return mLastIpNr;
}

std::string NetworkManager::getIPadress() {
	return mOwnIpAdress;
}

int const NetworkManager::getGroupId() {
	// add one makes it nicer for non programmers
	return groupId + 1;
}


std::string NetworkManager::extractLastIpNr(std::string& fullIp) {
	std::vector<std::string> hostSplit = ci::split(fullIp, ".");
	return hostSplit.back();
}


void NetworkManager::setupOSCSender() {
	//    mOwnIpAdress = System::getIpAddress();
	//    std::vector<std::string> hostSplit = ci::split(mOwnIpAdress, ".");
	//    std::vector<std::string> subnetSplit = ci::split(System::getSubnetMask(), ".");
	//    mLastIpNr =  hostSplit.back(); // getLastNummerIp(System::getIpAddress());
	//    std::string broadcast = "";
	//    for(int i=0; i < subnetSplit.size();++i){
	//        broadcast += subnetSplit[i] == "0" ? "255" : hostSplit[i];
	//        if(i < subnetSplit.size()-1) broadcast += ".";
	//    }
	//    int port = 3000;
	//    mSender.setup( broadcast, port, true );
	//    // mSender.setup( "127.0.0.1", port, false );
	//    lastBroadcast = app::getElapsedSeconds();
}


void NetworkManager::sendAlive() {
	osc::Message message;
	message.setAddress("/alive");
	message.append(groupId);
	mSender.send(message);
}

void NetworkManager::sendErase() {
	osc::Message message;
	message.setAddress("/erase");
	message.append(groupId);
	mSender.send(message);
}


void NetworkManager::sendPoints(std::vector<ci::vec3>& points, bool isEraserOn, ci::Color color, int frameId) {
	osc::Message message;
	message.setAddress("/points");
	message.append(groupId);            // 0
	message.append(frameId);            // 1
	message.append(isEraserOn);         // 2
	message.append(color.r);            // 3
	message.append(color.g);            // 4
	message.append(color.b);            // 5

	// points start from osc index 6
	for (vec3& p : points) {
		message.append(p.x);
		message.append(p.y);
		message.append(p.z * GS()->performanceDownScale.value());
	}
	mSender.send(message);
	lastBroadcast = app::getElapsedSeconds();
}

void NetworkManager::sendTwoPointShape(vec3& point1, vec3& point2, ToolState toolstate, ci::Color color, int frameId) {
	osc::Message message;
	message.setAddress("/shape");

	message.append(groupId);
	message.append(frameId);
	message.append(getToolStateString(toolstate));

	message.append(point1.x);
	message.append(point1.y);
	message.append(point1.z);
	message.append(point2.x);
	message.append(point2.y);
	message.append(point2.z);

	message.append(color.r);            // 3
	message.append(color.g);            // 4
	message.append(color.b);            // 5

	mSender.send(message);
	lastBroadcast = app::getElapsedSeconds();

}

void NetworkManager::cleanup() {
	mWork.reset();
	mIoService->stop();
	mThread.join();
}

