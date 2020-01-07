#include <iomanip>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstdlib>

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlServerSocket.h"
#include "igtlClientSocket.h"
#include "igtlTrackingDataMessage.h"
#include "igtl_tdata.h"

#define UnityIGTLinkSendAPI _declspec(dllexport)

igtl::ServerSocket::Pointer  UnityServerSocket;
igtl::Socket::Pointer UnitySendSocket;
igtl::TimeStamp::Pointer ts;

extern "C" {
	typedef void(_stdcall* CallbackServer)(int Svalue);
	UnityIGTLinkSendAPI void _stdcall ServerConnect(int serverport, double fps, CallbackServer ServerConnectionStatus)
		//UnityIGTLinkSendAPI void ServerConnect(int serverport, double fps, void(*ServerConnectionStatus)(int Svalue))
	{


		//int interval = (int)(1000.0 / fps);

		// Establish Connection
		igtl::ServerSocket::Pointer serversocket;
		serversocket = igtl::ServerSocket::New();
		int r = serversocket->CreateServer(serverport);
		UnityServerSocket = serversocket;
		UnitySendSocket = serversocket->WaitForConnection(10000);

		if (UnitySendSocket.IsNotNull())
		{
			ServerConnectionStatus(1);
		}

	}
	//------------------------------------------------------------------------------------------------------------------------

	void SendProbeToReference(igtl::Matrix4x4& Pmatrix, igtl::ServerSocket::Pointer serversocket, igtl::Socket::Pointer clientsocket)
	{
		// Allocate Transform Message Class
		igtl::TransformMessage::Pointer PTransformMessage = igtl::TransformMessage::New();

		//Package Data then send
		PTransformMessage->SetHeaderVersion(IGTL_HEADER_VERSION_1);
		PTransformMessage->InitPack();
		PTransformMessage->SetDeviceName("ProbeToTracker");
		PTransformMessage->SetMatrix(Pmatrix);
		ts = igtl::TimeStamp::New();
		ts->GetTime();
		PTransformMessage->SetTimeStamp(ts);
		PTransformMessage->Pack();
		UnitySendSocket->Send(PTransformMessage->GetPackPointer(), PTransformMessage->GetPackSize());
		//igtl::Sleep(1); // wait
	    // Close connection
        //socket->CloseSocket();
	}
	UnityIGTLinkSendAPI void _stdcall CloseSendSocket()
	{
		UnityServerSocket->CloseSocket();
		UnitySendSocket->CloseSocket();		
	}
	//------------------------------------------------------------
	void SendNeedleToReference(igtl::Matrix4x4& Nmatrix, igtl::ServerSocket::Pointer serversocket, igtl::Socket::Pointer clientsocket)
	{
        // Allocate Transform Message Class
		igtl::TransformMessage::Pointer NTransformMessage = igtl::TransformMessage::New();
		//Package Data then send
		NTransformMessage->SetHeaderVersion(IGTL_HEADER_VERSION_1);
		NTransformMessage->InitPack();
		NTransformMessage->SetDeviceName("NeedleToTracker");
		NTransformMessage->SetMatrix(Nmatrix);
		ts = igtl::TimeStamp::New();
		ts->GetTime();
		NTransformMessage->SetTimeStamp(ts);
		NTransformMessage->Pack();
		UnitySendSocket->Send(NTransformMessage->GetPackPointer(), NTransformMessage->GetPackSize());
		//igtl::Sleep(1); // wait	
	    // Close connection
	    //socket->CloseSocket();
	}
	//------------------------------------------------------------
	void SendKidneyToReference(igtl::Matrix4x4& Kmatrix, igtl::ServerSocket::Pointer serversocket, igtl::Socket::Pointer clientsocket)
	{
        // Allocate Transform Message Class
		igtl::TransformMessage::Pointer KTransformMessage = igtl::TransformMessage::New();
		//Package Data then send
		KTransformMessage->SetHeaderVersion(IGTL_HEADER_VERSION_1);
		KTransformMessage->InitPack();
		KTransformMessage->SetDeviceName("KidneyToTracker");
		KTransformMessage->SetMatrix(Kmatrix);
		ts = igtl::TimeStamp::New();
		ts->GetTime();
		KTransformMessage->SetTimeStamp(ts);
		KTransformMessage->Pack();
		UnitySendSocket->Send(KTransformMessage->GetPackPointer(), KTransformMessage->GetPackSize());
		//igtl::Sleep(1); // wait	
    	// Close connection
	    //socket->CloseSocket();
	}
	void SendGelblockToReference(igtl::Matrix4x4& Gmatrix, igtl::ServerSocket::Pointer serversocket, igtl::Socket::Pointer clientsocket)
	{
		// Allocate Transform Message Class
		igtl::TransformMessage::Pointer GTransformMessage = igtl::TransformMessage::New();
		//Package Data then send
		GTransformMessage->SetHeaderVersion(IGTL_HEADER_VERSION_1);
		GTransformMessage->InitPack();
		GTransformMessage->SetDeviceName("GelblockToTracker");
		GTransformMessage->SetMatrix(Gmatrix);
		ts = igtl::TimeStamp::New();
		ts->GetTime();
		GTransformMessage->SetTimeStamp(ts);
		GTransformMessage->Pack();
		UnitySendSocket->Send(GTransformMessage->GetPackPointer(), GTransformMessage->GetPackSize());
		//igtl::Sleep(1); // wait	
		// Close connection
		//socket->CloseSocket();
	}
	//------------------------------------------------------------------------------------------------------------------------
	typedef void(_stdcall* CallbackPosition)(float ReceivedPosition);
	UnityIGTLinkSendAPI void _stdcall GetData(float *Rorientation, int Osize, float *Rposition, int Psize, CallbackPosition CheckPosition)
	//UnityIGTLinkSendAPI void GetData(float *Rorientation, int Osize, float *Rposition, int Psize, void(*CheckPosition)(float ReceivedPosition))
	{

		// build matrx 
		igtl::Matrix4x4 Pmatrix;
		float Porientation[] = { Rorientation[0],Rorientation[1],Rorientation[2],Rorientation[3] };
		igtl::QuaternionToMatrix(Porientation, Pmatrix);
		Pmatrix[0][3] = Rposition[0];
		Pmatrix[1][3] = Rposition[1];
		Pmatrix[2][3] = Rposition[2];

		igtl::Matrix4x4 Nmatrix;
		float Norientation[] = { Rorientation[4],Rorientation[5],Rorientation[6],Rorientation[7] };
		igtl::QuaternionToMatrix(Norientation, Nmatrix);
		Nmatrix[0][3] = Rposition[3];
		Nmatrix[1][3] = Rposition[4];
		Nmatrix[2][3] = Rposition[5];

		igtl::Matrix4x4 Kmatrix;
		float Korientation[] = { Rorientation[8],Rorientation[9],Rorientation[10],Rorientation[11] };
		igtl::QuaternionToMatrix(Korientation, Kmatrix);
		Kmatrix[0][3] = Rposition[6];
		Kmatrix[1][3] = Rposition[7];
		Kmatrix[2][3] = Rposition[8];

		igtl::Matrix4x4 Gmatrix;
		float Gorientation[] = { Rorientation[12],Rorientation[13],Rorientation[14],Rorientation[15] };
		igtl::QuaternionToMatrix(Gorientation, Gmatrix);
		Gmatrix[0][3] = Rposition[9];
		Gmatrix[1][3] = Rposition[10];
		Gmatrix[2][3] = Rposition[11];
		
		float receivedX = Nmatrix[0][3];
        CheckPosition(receivedX);

		SendProbeToReference(Pmatrix, UnityServerSocket, UnitySendSocket);
		SendNeedleToReference(Nmatrix, UnityServerSocket, UnitySendSocket);
		SendKidneyToReference(Kmatrix, UnityServerSocket, UnitySendSocket);
		SendGelblockToReference(Gmatrix, UnityServerSocket, UnitySendSocket);
	}

	
}

	
