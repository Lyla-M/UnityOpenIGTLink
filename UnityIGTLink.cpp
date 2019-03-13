#include "UnityIGTLink.h"
#include <iostream>
#include <math.h>
#include <cstdlib>

#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"
#include "igtlServerSocket.h"
#include "igtlClientSocket.h"
#include "igtlTrackingDataMessage.h"
#include "igtl_tdata.h"

#define UnityIGTLinkAPI _declspec(dllexport)

igtl::ServerSocket::Pointer  UnityServerSocket;
igtl::Socket::Pointer UnitySendSocket;

extern "C" {

	UnityIGTLinkAPI void Connect(int port, double fps, void(*ConnectionStatus)(int value))
	{
		//------------------------------------------------------------

		//int interval = (int)(1000.0 / fps);

		//------------------------------------------------------------
		// Establish Connection
		igtl::ServerSocket::Pointer serversocket;
		serversocket = igtl::ServerSocket::New();
		int r = serversocket->CreateServer(port);
		UnityServerSocket = serversocket;
		UnitySendSocket = serversocket->WaitForConnection(100000);
		if (UnitySendSocket.IsNotNull())
		{
		ConnectionStatus(1);
		}
	}



		//------------------------------------------------------------
	void SendData(igtl::Matrix4x4& matrix, igtl::ServerSocket::Pointer serversocket, igtl::Socket::Pointer clientsocket)
	{
	/*	SendTransform(someMatrix, "ProbeToTracker", someSocket);
		SendTransform(someOtherMatrix, "ReferenceToTracker", someSocket);*/
		//------------------------------------------------------------
		
			// Allocate Transform Message Class
		    igtl::TrackingDataMessage::Pointer trackingSendMsg = igtl::TrackingDataMessage::New();
			igtl::TrackingDataElement::Pointer trackingElement0 = igtl::TrackingDataElement::New();
			trackingElement0->SetName("Tracker2ToReference");
			trackingElement0->SetType(IGTL_TDATA_TYPE_6D);
			trackingElement0->SetMatrix(matrix);
			//------------------------------------------------------------
			// Allocate TimeStamp class
			igtl::TimeStamp::Pointer ts;
			ts = igtl::TimeStamp::New();
			//------------------------------------------------------------
			//Package Data then send
			trackingSendMsg = igtl::TrackingDataMessage::New();
			trackingSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_1);
			trackingSendMsg->SetDeviceName("Tracker8");
			ts->GetTime();
			trackingSendMsg->SetTimeStamp(ts);
			trackingSendMsg->AddTrackingDataElement(trackingElement0);

			trackingSendMsg->Pack();
			UnitySendSocket->Send(trackingSendMsg->GetPackPointer(), trackingSendMsg->GetPackSize());

			
			//igtl::Sleep(1); // wait
		//}

		//------------------------------------------------------------
		// Close connection

		//socket->CloseSocket();
		
	}
	UnityIGTLinkAPI void GetData(float *Rorientation, int Osize, float *Rposition, int Psize, void(*CheckPosition)(float ReceivedPosition))
	{

		// build matrx 
		igtl::Matrix4x4 matrix;
		igtl::QuaternionToMatrix(Rorientation, matrix);
		matrix[0][3] = Rposition[0];
		matrix[1][3] = Rposition[1];
		matrix[2][3] = Rposition[2];
		
		float receivedZ = matrix[0][3];
        CheckPosition(receivedZ);

		SendData(matrix,UnityServerSocket, UnitySendSocket);
	}
}
	

UnityIGTLink::UnityIGTLink()
{
}

UnityIGTLink::~UnityIGTLink()
{
}
