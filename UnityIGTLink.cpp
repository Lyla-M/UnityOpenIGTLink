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
	void SendProbeToReference(igtl::Matrix4x4& Pmatrix, igtl::ServerSocket::Pointer serversocket, igtl::Socket::Pointer clientsocket)
	{

		//------------------------------------------------------------
		
			// Allocate Transform Message Class
		    igtl::TransformMessage::Pointer PTransformMessage = igtl::TransformMessage::New();

			//------------------------------------------------------------
			//Package Data then send
			PTransformMessage->SetHeaderVersion(IGTL_HEADER_VERSION_1);
			PTransformMessage->InitPack();
			PTransformMessage->SetDeviceName("ProbeToReference");
			PTransformMessage->SetMatrix(Pmatrix);
			//ts->GetTime();
			//TransformMessage->SetTimeStamp(ts);
			PTransformMessage->Pack();
			UnitySendSocket->Send(PTransformMessage->GetPackPointer(), PTransformMessage->GetPackSize());			
			//igtl::Sleep(1); // wait
	
		//------------------------------------------------------------
		// Close connection

		//socket->CloseSocket();
		
	}
	void SendNeedleToReference(igtl::Matrix4x4& Nmatrix, igtl::ServerSocket::Pointer serversocket, igtl::Socket::Pointer clientsocket)
	{

		//------------------------------------------------------------

			// Allocate Transform Message Class
		igtl::TransformMessage::Pointer NTransformMessage = igtl::TransformMessage::New();

		//------------------------------------------------------------
		//Package Data then send
		NTransformMessage->SetHeaderVersion(IGTL_HEADER_VERSION_1);
		NTransformMessage->InitPack();
		NTransformMessage->SetDeviceName("NeedleToReference");
		NTransformMessage->SetMatrix(Nmatrix);
		//ts->GetTime();
		//TransformMessage->SetTimeStamp(ts);
		NTransformMessage->Pack();
		UnitySendSocket->Send(NTransformMessage->GetPackPointer(), NTransformMessage->GetPackSize());
		//igtl::Sleep(1); // wait

	//------------------------------------------------------------
	// Close connection

	//socket->CloseSocket();

	}
	void SendKidneyToReference(igtl::Matrix4x4& Kmatrix, igtl::ServerSocket::Pointer serversocket, igtl::Socket::Pointer clientsocket)
	{

		//------------------------------------------------------------

			// Allocate Transform Message Class
		igtl::TransformMessage::Pointer KTransformMessage = igtl::TransformMessage::New();

		//------------------------------------------------------------
		//Package Data then send
		KTransformMessage->SetHeaderVersion(IGTL_HEADER_VERSION_1);
		KTransformMessage->InitPack();
		KTransformMessage->SetDeviceName("KidneyToReference");
		KTransformMessage->SetMatrix(Kmatrix);
		//ts->GetTime();
		//TransformMessage->SetTimeStamp(ts);
		KTransformMessage->Pack();
		UnitySendSocket->Send(KTransformMessage->GetPackPointer(), KTransformMessage->GetPackSize());
		//igtl::Sleep(1); // wait

	//------------------------------------------------------------
	// Close connection

	//socket->CloseSocket();

	}
	UnityIGTLinkAPI void GetData(float *Rorientation, int Osize, float *Rposition, int Psize, void(*CheckPosition)(float ReceivedPosition))
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
		
		float receivedZ = Pmatrix[0][3];
        CheckPosition(receivedZ);

		SendProbeToReference(Pmatrix, UnityServerSocket, UnitySendSocket);
		SendNeedleToReference(Nmatrix, UnityServerSocket, UnitySendSocket);
		SendKidneyToReference(Kmatrix, UnityServerSocket, UnitySendSocket);
	}
}
	

UnityIGTLink::UnityIGTLink()
{
}

UnityIGTLink::~UnityIGTLink()
{
}
